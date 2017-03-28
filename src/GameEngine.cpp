/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "Master.h"
#include "GameApp.h"
#include "GameEngine.h"
#include "PixelBuffer.h"
#include "Texture.h"
#include "GLUtil.h"

// DAVIDE - this is dangerous... the camera may end up in a horrible place...
//#define USE_SAVED_CAMERA_POS

#define NO_POSTFX

//#define ENABLE_SPACE

static const float PLANET_RADIUS = 1.0f;

// Acceleration rate due to thrusters (units/s*s)
static const float CAM_MOVE_THRUST     = PLANET_RADIUS / 10;

// Damping effect on velocity
static const float CAM_MOVE_RESISTANCE = 0.1f;

//==================================================================
static void drawSphere(
                double r,
                int longs,
                int lats,
                const float *pPosOff=nullptr )
{
    if ( pPosOff )
    {
        glPushMatrix();
        glTranslatef( pPosOff[0], pPosOff[1], pPosOff[2] );
    }

    auto output_vertex = [=](int lat, int lon)
    {
        //static const double pi = 3.14159265358979323846;
        auto u = (float)lon / (float)longs;
        auto v = (float)lat / (float)lats;

        glTexCoord2f( u, v );

        auto lo = (float)(2.0*M_PI) * u;
        auto la = (float)(1.0*M_PI) * v;

        float vec[3] {
            cos(lo)*sin(la),
            sin(lo)*sin(la),
            cos(la)
        };

        glNormal3fv(vec);

        vec[0] *= (float)r;
        vec[1] *= (float)r;
        vec[2] *= (float)r;
        glVertex3fv(vec);
    };

    /* this is probably doing almost exactly the same thing as gluSphere */
    for (int lat=0; lat<lats; ++lat)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int lon=0; lon<=longs; ++lon)
        {
            /* glColor3f(FRAND(), FRAND(), FRAND()); */
            output_vertex(lat, lon);
            output_vertex(lat+1, lon);
        }
        glEnd();
    }

    if ( pPosOff )
    {
        glPopMatrix();
    }
}

//==================================================================
static void setASUniforms(
                const ONAS_State &state,
                CShaderObject *pShader,
                const CVector &camPos,
                const CVector &lightDir )
{
    auto setUni1f = [pShader]( const char *pUniName, float val1f )
    {
        pShader->SetUniformParameter1f( pUniName, val1f );
    };

    auto setUni3f = [pShader]( const char *pUniName, const float *pVal3f )
    {
        pShader->SetUniformParameter3f( pUniName, pVal3f[0], pVal3f[1], pVal3f[2] );
    };

    state.UpdateShaderUniforms( &camPos.x, &lightDir.x, setUni1f, setUni3f );
}

//==================================================================
CGameEngine::CGameEngine()
{
    //GetApp()->MessageBox((const char *)glGetString(GL_EXTENSIONS));
    GLUtil()->Init();
    m_fFont.Init(GetGameApp()->GetHDC());

    m_pBuffer.Init(1024, 1024, 0);
    m_pBuffer.MakeCurrent();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);

    //glEnable(GL_MULTISAMPLE_ARB);

    //
    m_ASState.SetPlanetRadius( PLANET_RADIUS );

    // setup starting position
    CVector vPos( 0, 0, PLANET_RADIUS * 2.5f );
#ifdef USE_SAVED_CAMERA_POS
    if ( const auto *psz = GetApp()->GetProfileString("Camera", "Position", NULL) )
        sscanf(psz, "%f, %f, %f", &vPos.x, &vPos.y, &vPos.z);
#endif
    m_3DCamera.SetPosition(CDoubleVector(vPos));

    // setup starting orientation
    CQuaternion qOrientation(0.0f, 0.0f, 0.0f, 1.0f);
#ifdef USE_SAVED_CAMERA_POS
    if ( const auto *psz = GetApp()->GetProfileString("Camera", "Orientation", NULL) )
        sscanf(psz, "%f, %f, %f, %f", &qOrientation.x, &qOrientation.y, &qOrientation.z, &qOrientation.w);
#endif
    qOrientation.Normalize();
    m_3DCamera = qOrientation;

    // setup light source
    m_vLight = CVector(0, 0, 1000);
    m_vLightDirection = m_vLight / m_vLight.Magnitude();

    //
    CTexture::InitStaticMembers(238653, 256);

    m_nSamples = 3;     // Number of sample rays to use in integral equation
    m_fExposure = 2.0f;

    m_pbOpticalDepth.MakeOpticalDepthBuffer(
        m_ASState.m_InnerRadius,
        m_ASState.m_OuterRadius,
        m_ASState.m_RayleighScaleDepth,
        m_ASState.m_MieScaleDepth);

    //
    auto loadASShader = [this](
            CShaderObject &so,
            const std::string &srcVertName,
            const std::string &srcFragName )
    {
        so.LoadFromFile(
                "ONAS_Common.glsl",
                srcVertName + ".vert",
                srcFragName + ".frag");
    };
    loadASShader( m_shSkyFromSpace        , "ONAS_SkyFromSpace"        , "ONAS_Sky"    );
    loadASShader( m_shSkyFromAtmosphere   , "ONAS_SkyFromAtmosphere"   , "ONAS_Sky"    );
    loadASShader( m_shGroundFromSpace     , "ONAS_GroundFromSpace"     , "ONAS_Ground" );
    loadASShader( m_shGroundFromAtmosphere, "ONAS_GroundFromAtmosphere", "ONAS_Ground" );
    loadASShader( m_shSpaceFromSpace      , "ONAS_SpaceFromSpace"      , "ONAS_Space"  );
    loadASShader( m_shSpaceFromAtmosphere , "ONAS_SpaceFromAtmosphere" , "ONAS_Space"  );

    //
    {
    CPixelBuffer pb;
    pb.Init(256, 256, 1);
    pb.MakeGlow2D(40.0f, 0.1f);
    m_tMoonGlow.Init(&pb);
    }

    {
    CPixelBuffer pb;
    pb.LoadJPEG("earthmap1k.jpg");
    m_tEarth.Init(&pb);
    }
}

//==================================================================
CGameEngine::~CGameEngine()
{
    // Write the camera position and orientation to the registry
    char szBuffer[256];

    sprintf(szBuffer, "%f, %f, %f",
            m_3DCamera.GetPosition().x,
            m_3DCamera.GetPosition().y,
            m_3DCamera.GetPosition().z);

    GetApp()->WriteProfileString("Camera", "Position", szBuffer);

    sprintf(szBuffer, "%f, %f, %f, %f",
            m_3DCamera.x,
            m_3DCamera.y,
            m_3DCamera.z,
            m_3DCamera.w);

    GetApp()->WriteProfileString("Camera", "Orientation", szBuffer);

    //
    m_pBuffer.Cleanup();
}

//==================================================================
static void setProjectionMatrix()
{
    auto fov = 45.0;

    auto ratio_woh =
        (double)GetGameApp()->GetWidth() /
        (double)GetGameApp()->GetHeight();

    auto nearr = (double)PLANET_RADIUS * 0.0001;
    auto farr  = (double)PLANET_RADIUS * 10.0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( fov, ratio_woh, nearr, farr );
    glMatrixMode(GL_MODELVIEW);
}

//==================================================================
void CGameEngine::RenderFrame(int nMilliseconds)
{
    // Determine the FPS
    static char szFrameCount[20] = {0};
    static int nTime = 0;
    static int nFrames = 0;
    nTime += nMilliseconds;
    if(nTime >= 1000)
    {
        m_fFPS = (float)(nFrames * 1000) / (float)nTime;
        sprintf(szFrameCount, "%2.2f FPS", m_fFPS);
        nTime = nFrames = 0;
    }
    nFrames++;

    // Move the camera
    HandleInput(nMilliseconds * 0.001f);

    // se t the projection matrix at every frame
    setProjectionMatrix();

#if defined(NO_POSTFX)
    GLUtil()->MakeCurrent();
    glViewport(0, 0, GetGameApp()->GetWidth(), GetGameApp()->GetHeight());
#else
    m_pBuffer.MakeCurrent();
    glViewport(0, 0, 1024, 1024);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

#if 0
    glLoadMatrixf(m_3DCamera.GetViewMatrix());

    C3DObject obj;
    glMultMatrixf(obj.GetModelMatrix(&m_3DCamera));
#else
    {
    const auto mtxCS_WS = m_3DCamera.GetViewMatrix();

    C3DObject obj;
    const auto mtxWS_LS = obj.GetModelMatrix(&m_3DCamera);

    const auto mtxCS_LS = mtxCS_WS * mtxWS_LS;

    glLoadMatrixf( mtxCS_LS );
    }
#endif

    const auto camPos = (CVector)m_3DCamera.GetPosition();

#ifdef ENABLE_SPACE
    // -- space
    CShaderObject *pSpaceShader = NULL;
    if(camPos.Magnitude() < m_ASState.m_OuterRadius)
        pSpaceShader = &m_shSpaceFromAtmosphere;
    else if(camPos.z > 0.0f)
        pSpaceShader = &m_shSpaceFromSpace;

    if(pSpaceShader)
    {
        pSpaceShader->Enable();
        setASUniforms( m_ASState, pSpaceShader, camPos, m_vLightDirection );
        pSpaceShader->SetUniformParameter1i("s2Tex1", 0);
    }

    {
    auto bindScope = m_tMoonGlow.BindTexture();
    m_tMoonGlow.EnableTexture();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-4.0f, 4.0f, -50.0f);
    glTexCoord2f(0, 1);
    glVertex3f(-4.0f, -4.0f, -50.0f);
    glTexCoord2f(1, 1);
    glVertex3f(4.0f, -4.0f, -50.0f);
    glTexCoord2f(1, 0);
    glVertex3f(4.0f, 4.0f, -50.0f);
    glEnd();
    m_tMoonGlow.DisableTexture();
    }

    if(pSpaceShader)
        pSpaceShader->Disable();
#endif

    // -- ground
    CShaderObject *pGroundShader;
    if(camPos.Magnitude() >= m_ASState.m_OuterRadius)
        pGroundShader = &m_shGroundFromSpace;
    else
        pGroundShader = &m_shGroundFromAtmosphere;

    pGroundShader->Enable();
    setASUniforms( m_ASState, pGroundShader, camPos, m_vLightDirection );
    pGroundShader->SetUniformParameter1i("s2Tex1", 0);

    {
    auto bindScope = m_tEarth.BindTexture();
    m_tEarth.EnableTexture();
    drawSphere(m_ASState.m_InnerRadius, 100, 50);
    m_tEarth.DisableTexture();
    }
    pGroundShader->Disable();

    // -- sky
    CShaderObject *pSkyShader;
    if(camPos.Magnitude() >= m_ASState.m_OuterRadius)
        pSkyShader = &m_shSkyFromSpace;
    else
        pSkyShader = &m_shSkyFromAtmosphere;

    pSkyShader->Enable();
    setASUniforms( m_ASState, pSkyShader, camPos, m_vLightDirection );

    glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    drawSphere(m_ASState.m_OuterRadius, 100, 50);

    glDisable(GL_BLEND);
    glFrontFace(GL_CCW);
    pSkyShader->Disable();

    //
    glPopMatrix();
    glFlush();

#if !defined(NO_POSTFX)
    //CTexture tTest;
    //tTest.InitCopy(0, 0, 1024, 1024);

    GLUtil()->MakeCurrent();
    glViewport(0, 0, GetGameApp()->GetWidth(), GetGameApp()->GetHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    {
    //tTest.Enable();
    auto bindScope = m_pBuffer.BindTexture(m_fExposure, m_bUseHDR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f(1, 0);
    glTexCoord2f(1, 1); glVertex2f(1, 1);
    glTexCoord2f(0, 1); glVertex2f(0, 1);
    glEnd();
    }
    m_pBuffer.ReleaseTexture();
    //tTest.Disable();

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_LIGHTING);
#endif

    // Draw info in the top-left corner
    char szBuffer[256];
    m_fFont.Begin();
    glColor3d(1.0, 1.0, 1.0);
    m_fFont.SetPosition(0, 0);
    m_fFont.Print(szFrameCount);
    m_fFont.SetPosition(0, 15);
    sprintf(szBuffer, "Samples (+/-): %d", m_nSamples);
    m_fFont.Print(szBuffer);
    m_fFont.SetPosition(0, 30);
    sprintf(szBuffer, "Kr (1/Sh+1): %-4.4f", m_ASState.m_Kr);
    m_fFont.Print(szBuffer);
    m_fFont.SetPosition(0, 45);
    sprintf(szBuffer, "Km (2/Sh+2): %-4.4f", m_ASState.m_Km);
    m_fFont.Print(szBuffer);
    m_fFont.SetPosition(0, 60);
    sprintf(szBuffer, "g (3/Sh+3): %-3.3f", m_ASState.m_g);
    m_fFont.Print(szBuffer);
    m_fFont.SetPosition(0, 75);
    sprintf(szBuffer, "ESun (4/Sh+4): %-1.1f", m_ASState.m_ESun);
    m_fFont.Print(szBuffer);
    m_fFont.SetPosition(0, 90);
    sprintf(szBuffer, "Red (5/Sh+5): %-3.3f", m_ASState.m_Wavelength[0]);
    m_fFont.Print(szBuffer);
    m_fFont.SetPosition(0, 105);
    sprintf(szBuffer, "Green (6/Sh+6): %-3.3f", m_ASState.m_Wavelength[1]);
    m_fFont.Print(szBuffer);
    m_fFont.SetPosition(0, 120);
    sprintf(szBuffer, "Blue (7/Sh+7): %-3.3f", m_ASState.m_Wavelength[2]);
    m_fFont.Print(szBuffer);
    m_fFont.SetPosition(0, 135);
    sprintf(szBuffer, "Exposure (8/Sh+8): %-2.2f", m_fExposure);
    m_fFont.Print(szBuffer);
    m_fFont.End();
    glFlush();
}

void CGameEngine::OnChar(WPARAM c)
{
    switch(c)
    {
        case 'p':
            m_nPolygonMode = (m_nPolygonMode == GL_FILL) ? GL_LINE : GL_FILL;
            glPolygonMode(GL_FRONT, m_nPolygonMode);
            break;
        case 'h':
            m_bUseHDR = !m_bUseHDR;
            break;
        case '+':
            m_nSamples++;
            break;
        case '-':
            m_nSamples--;
            break;
    }
}

void CGameEngine::HandleInput(float fSeconds)
{
    auto isKeyDown = []( auto vk ) { return GetKeyState( vk ) & 0x8000; };
    auto isShiftDown = []()        { return GetKeyState( VK_SHIFT ) & 0x8000; };

    if ( isKeyDown( '1' ) )
    {
        if ( isShiftDown() ) m_ASState.m_Kr = Max(0.0f, m_ASState.m_Kr - 0.0001f); else
                             m_ASState.m_Kr += 0.0001f;
    }
    else if ( isKeyDown( '2' ) )
    {
        if ( isShiftDown() ) m_ASState.m_Km = Max(0.0f, m_ASState.m_Km - 0.0001f); else
                             m_ASState.m_Km += 0.0001f;
    }
    else if ( isKeyDown( '3' ) )
    {
        if ( isShiftDown() ) m_ASState.m_g = Max(-1.0f, m_ASState.m_g-0.001f); else
                             m_ASState.m_g = Min( 1.0f, m_ASState.m_g+0.001f);
    }
    else if ( isKeyDown( '4' ) )
    {
        if ( isShiftDown() ) m_ASState.m_ESun = Max(0.0f, m_ASState.m_ESun - 0.1f); else
                             m_ASState.m_ESun += 0.1f;
    }
    else if ( isKeyDown( '5' ) )
    {
        auto &val = m_ASState.m_Wavelength[0];

        if ( isShiftDown() ) val = Max(0.001f, val -= 0.001f); else
                             val += 0.001f;
    }
    else if ( isKeyDown( '6' ) )
    {
        auto &val = m_ASState.m_Wavelength[1];

        if ( isShiftDown() ) val = Max(0.001f, val -= 0.001f); else
                             val += 0.001f;
    }
    else if ( isKeyDown( '7' ) )
    {
        auto &val = m_ASState.m_Wavelength[2];

        if ( isShiftDown() ) val = Max(0.001f, val -= 0.001f); else
                             val += 0.001f;
    }
    else if ( isKeyDown( '8' ) )
    {
        if ( isShiftDown() ) m_fExposure = Max(0.1f, m_fExposure-0.1f); else
                             m_fExposure += 0.1f;
    }


    const float ROTATE_SPEED = 1.0f;

    // Turn left/right means rotate around the up axis
    if ( isKeyDown(VK_NUMPAD6) || isKeyDown(VK_RIGHT) )
        m_3DCamera.Rotate(m_3DCamera.GetUpAxis(), fSeconds * -ROTATE_SPEED);

    if ( isKeyDown(VK_NUMPAD4) || isKeyDown(VK_LEFT) )
        m_3DCamera.Rotate(m_3DCamera.GetUpAxis(), fSeconds * ROTATE_SPEED);

    // Turn up/down means rotate around the right axis
    if ( isKeyDown(VK_NUMPAD8) || isKeyDown(VK_UP) )
        m_3DCamera.Rotate(m_3DCamera.GetRightAxis(), fSeconds * -ROTATE_SPEED);

    if ( isKeyDown(VK_NUMPAD2) || isKeyDown(VK_DOWN) )
        m_3DCamera.Rotate(m_3DCamera.GetRightAxis(), fSeconds * ROTATE_SPEED);

    // Roll means rotate around the view axis
    if ( isKeyDown(VK_NUMPAD7) )
        m_3DCamera.Rotate(m_3DCamera.GetViewAxis(), fSeconds * -ROTATE_SPEED);

    if ( isKeyDown(VK_NUMPAD9) )
        m_3DCamera.Rotate(m_3DCamera.GetViewAxis(), fSeconds * ROTATE_SPEED);

    // Handle acceleration keys
    CVector vAccel(0.0f);
    if( isKeyDown(VK_SPACE) )
        m_3DCamera.SetVelocity(CVector(0.0f));  // Full stop
    else
    {
        // Add camera's acceleration due to thrusters
        float fThrust = CAM_MOVE_THRUST;
        if( isKeyDown(VK_CONTROL) ) fThrust *= 10.0f;

        // Thrust forward/reverse affects velocity along the view axis
        if ( isKeyDown('W') ) vAccel += m_3DCamera.GetViewAxis() *  fThrust;
        if ( isKeyDown('S') ) vAccel += m_3DCamera.GetViewAxis() * -fThrust;

        // Thrust left/right affects velocity along the right axis
        if ( isKeyDown('D') ) vAccel += m_3DCamera.GetRightAxis() *  fThrust;
        if ( isKeyDown('A') ) vAccel += m_3DCamera.GetRightAxis() * -fThrust;

        // Thrust up/down affects velocity along the up axis
//#define WORLD_UPDOWN
#ifdef WORLD_UPDOWN
        CVector v = m_3DCamera.GetPosition();
        v.Normalize();
        if ( isKeyDown('M') ) vAccel += v *  fThrust;
        if ( isKeyDown('N') ) vAccel += v * -fThrust;
#else
        if ( isKeyDown('M') ) vAccel += m_3DCamera.GetUpAxis() *  fThrust;
        if ( isKeyDown('N') ) vAccel += m_3DCamera.GetUpAxis() * -fThrust;
#endif

        m_3DCamera.Accelerate(vAccel, fSeconds, CAM_MOVE_RESISTANCE);
        CVector vPos = m_3DCamera.GetPosition();
        float fMagnitude = vPos.Magnitude();
        if(fMagnitude < m_ASState.m_InnerRadius)
        {
            vPos *= (m_ASState.m_InnerRadius * (1 + DELTA)) / fMagnitude;
            m_3DCamera.SetPosition(CDoubleVector(vPos.x, vPos.y, vPos.z));
            m_3DCamera.SetVelocity(-m_3DCamera.GetVelocity());
        }
    }
}


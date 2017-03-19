// GLUtil.cpp
//

#include "Master.h"
#include "GLUtil.h"

CGLUtil g_glUtil;
CGLUtil *CGLUtil::m_pMain = &g_glUtil;

#ifdef USE_CG
CGcontext CShaderObject::m_cgContext;
CGprofile CShaderObject::m_cgVertexProfile;
CGprofile CShaderObject::m_cgFragmentProfile;
#endif

CGLUtil::CGLUtil()
{
	// Start by clearing out all the member variables
}

CGLUtil::~CGLUtil()
{
}

void CGLUtil::Init()
{
	// Start by storing the current HDC and HGLRC
	m_hDC = wglGetCurrentDC();
	m_hGLRC = wglGetCurrentContext();

	m_bATI = strstr((const char *)glGetString(GL_VENDOR), "ATI") != NULL;
	LogInfo((const char *)glGetString(GL_VENDOR));
	LogInfo((const char *)glGetString(GL_RENDERER));
	LogInfo((const char *)glGetString(GL_VERSION));
	LogInfo((const char *)glGetString(GL_EXTENSIONS));

	// Finally, initialize the default rendering context
	InitRenderContext(m_hDC, m_hGLRC);
#ifdef USE_CG
	CShaderObject::InitContext();
#endif
}

void CGLUtil::Cleanup()
{
#ifdef USE_CG
	CShaderObject::ReleaseContext();
#endif
}

void CGLUtil::InitRenderContext(HDC hDC, HGLRC hGLRC)
{
	wglMakeCurrent(hDC, hGLRC);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, CVector4(0.0f));

	wglMakeCurrent(m_hDC, m_hGLRC);
}

#if defined(GL_ARB_debug_output) && !defined(__linux__)
//==================================================================
static void GLAPIENTRY errorCallbackARB(
                GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *pMessage,
                const void *pUserParam )
{
    const char *pSource = "";
    switch ( source )
    {
    case GL_DEBUG_SOURCE_API            : pSource = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM  : pSource = "Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: pSource = "Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY    : pSource = "3rd Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION    : pSource = "App"; break;
    case GL_DEBUG_SOURCE_OTHER          : pSource = "Other"; break;
    default: pSource = "Unknown"; break;
    }

    const char *pType = "";
    switch ( type )
    {
    case GL_DEBUG_TYPE_ERROR              : pType = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: pType = "Deprecated"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR : pType = "Undefined"; break;
    case GL_DEBUG_TYPE_PORTABILITY        : pType = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE        : pType = "Performance"; break;
    case GL_DEBUG_TYPE_OTHER              : pType = "Other"; break;
    default: pType = "Unknown"; break;
    }

    const char *pSeverity = "";
    switch ( severity )
    {
    case GL_DEBUG_SEVERITY_HIGH  : pSeverity = "H"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: pSeverity = "M"; break;
    case GL_DEBUG_SEVERITY_LOW   : pSeverity = "L"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: pSeverity = "Not"; break;
    default: pSeverity = "Unk"; break;
    }

    (void)pSource;
    (void)id;
    printf( "* GLERR: %s(%s) -- %s\n", pType, pSeverity, pMessage );
}
#endif

#if defined(GL_ARB_debug_output) && !defined(__linux__)
static constexpr bool INTERCEPT_LOW = false;
static constexpr bool INTERCEPT_MED = false;
static constexpr bool INTERCEPT_NOT = false;
#endif

//==================================================================
void GLUTIL_SetupErrorIntercept()
{
#if defined(GL_ARB_debug_output) && !defined(__linux__)
    if ( glDebugMessageCallback )
    {
        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
        glDebugMessageCallback( errorCallbackARB, NULL );

        auto enableServerity = []( auto sev, bool onOff )
        {
            glDebugMessageControl(
                GL_DONT_CARE,
                GL_DONT_CARE,
                sev,
                0,
                nullptr,
                onOff ? GL_TRUE : GL_FALSE );
        };

        enableServerity( GL_DONT_CARE, true );
        enableServerity( GL_DEBUG_SEVERITY_LOW, INTERCEPT_LOW );
        enableServerity( GL_DEBUG_SEVERITY_MEDIUM, INTERCEPT_MED );
        enableServerity( GL_DEBUG_SEVERITY_NOTIFICATION, INTERCEPT_NOT );
    }
#endif
}

//==================================================================

//==================================================================
/// ONAS_Common.h
///
/// Created by Davide Pasca - 2017/3/26
///
/// Based on the GPU Gems 2 demo by Sean O'Neil (2004)
//==================================================================

uniform vec3  u_CameraPos;      // The camera's current position
uniform vec3  u_LightDir;       // The direction vector to the light source

uniform vec3  u_InvWavelength;  // 1 / pow(wavelength, 4) for the red, green, and blue channels
uniform float u_OuterRadius;    // The outer (atmosphere) radius
uniform float u_InnerRadius;    // The inner (planetary) radius
uniform float u_KrESun;         // Kr * ESun
uniform float u_KmESun;         // Km * ESun
uniform float u_Kr4PI;          // Kr * 4 * PI
uniform float u_Km4PI;          // Km * 4 * PI
uniform float u_Scale;          // 1 / (u_OuterRadius - u_InnerRadius)
uniform float u_ScaleDepth;     // The scale depth (i.e. the altitude at which the atmosphere's average density is found)
uniform float u_ScaleOverScaleDepth; // u_Scale / u_ScaleDepth

uniform float u_g;

const int   SAMPLES_N = 2;
const float SAMPLES_F = 2.0;

//==================================================================
float ONAS_CalcCamDistanceFromPlanetOrigin()
{
    return length( u_CameraPos );
}

//==================================================================
void ONAS_CalcRayFromCamera(
            vec3 pos,
            out vec3 out_raySta,
            out vec3 out_rayDir )
{
    out_raySta = u_CameraPos;
    out_rayDir = normalize( pos - out_raySta );
}

//==================================================================
void ONAS_CalcRayFromCameraLen(
            vec3 pos,
            out vec3  out_raySta,
            out vec3  out_rayDir,
            out float out_rayLen )
{
    out_raySta = u_CameraPos;

    vec3 raySta_to_pos = pos - out_raySta;

    out_rayLen = length( raySta_to_pos );
    out_rayDir = raySta_to_pos / out_rayLen;
}

//==================================================================
float ONAS_Scale( float cosA )
{
    float x = 1.0 - cosA;

    return
        u_ScaleDepth *
            exp( -0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))) );
}

//==================================================================
// http://http.developer.nvidia.com/GPUGems2/elementLinks/0256equ01.jpg
float ONAS_CalcMiePhase( float cosA, float g )
{
    float g2 = g * g;

    float a = 1.0 - g2;
    float b = 2.0 + g2;

    float c = 1.0 + cosA * cosA;
    float d = pow( 1.0 + g2 - 2.0 * g * cosA, 1.5 );

    return (3.0 / 2.0) * a / b * c / d;
}

//==================================================================
// NOTE: assumes that rayDir is normalized
// NOTE: using radius squared
float ONAS_CalcRaySphereClosestInters(
                vec3 raySta,
                vec3 rayDir,
                vec3 sphereC,
                float sphereRSqr )
{
    float B = 2.0 * dot( raySta, rayDir ) - dot( rayDir, sphereC );

    vec3 raySta_to_sphereC = sphereC - raySta;

    float C = dot( raySta_to_sphereC, raySta_to_sphereC ) - sphereRSqr;

    float det = max( 0.0, B*B - 4.0 * C );

    return 0.5 * ( -B - sqrt( det ) );
}

//==================================================================
vec3 ONAS_RaytraceScatterSky(
            vec3 raySta,
            vec3 rayDir,
            float rayLen,
            float useOuterRadius,
            float near,
            float startDepth )
{
    // Calculate the ray's starting position,
    //   then calculate its scattering offset

    vec3 samplePointStart = raySta + rayDir * near;
    float segmentLength = rayLen - near;

    float startAngle = dot(rayDir, samplePointStart) / useOuterRadius;
    float startOffset = startDepth * ONAS_Scale( startAngle );

    // Initialize the scattering loop variables
    float sampleLength = segmentLength / SAMPLES_F;
    float scaledLength = sampleLength * u_Scale;
    vec3 sampleRay = rayDir * sampleLength;
    vec3 samplePoint = samplePointStart + sampleRay * 0.5;

    vec3 out_col = vec3(0.0, 0.0, 0.0);

    for(int i=0; i < SAMPLES_N; ++i)
    {
        float height = length(samplePoint);

        float depth = exp( u_ScaleOverScaleDepth * (u_InnerRadius - height) );

        float lightAngle = dot( u_LightDir, samplePoint ) / height;

        float cameraAngle = dot( rayDir, samplePoint ) / height;

        float scatter = startOffset +
                            depth * (ONAS_Scale( lightAngle ) -
                                     ONAS_Scale( cameraAngle ));

        vec3 atten = exp( -scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI) );

        out_col += atten * (depth * scaledLength);

        samplePoint += sampleRay;
    }

    return out_col;
}

//==================================================================
void ONAS_RaytraceScatterGround(
            out vec3 out_groundCol,
            out vec3 out_atten,
            vec3 pos,
            vec3 raySta,
            vec3 rayDir,
            float rayLen,
            float useOuterRadius,
            float near )
{
    // Calculate the ray's starting position,
    //   then calculate its scattering offset

    vec3 samplePointStart = raySta + rayDir * near;
    float segmentLength  = rayLen - near;

    // Initialize the scattering loop variables
    float sampleLength = segmentLength / SAMPLES_F;
    float scaledLength = sampleLength * u_Scale;
    vec3 sampleRay = rayDir * sampleLength;
    vec3 samplePoint = samplePointStart + sampleRay * 0.5;

    float cameraOffset;
    float temp;
    {
    float depth = exp( (u_InnerRadius - useOuterRadius) / u_ScaleDepth );

    float posLen = length( pos );
    float cameraAngle = dot(-rayDir, pos) / posLen;
    float lightAngle = dot(u_LightDir, pos) / posLen;
    float cameraScale = ONAS_Scale( cameraAngle );
    float lightScale = ONAS_Scale( lightAngle );

    cameraOffset = depth * cameraScale;
    temp = lightScale + cameraScale;
    }

    // Now loop through the sample rays
    vec3 attenIntegr = vec3(0.0, 0.0, 0.0);
    for(int i=0; i < SAMPLES_N; ++i)
    {
        float height = length(samplePoint);

        float depth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - height));

        float scatter = depth * temp - cameraOffset;

        vec3 atten = exp( -scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI) );

        // last attentuation goes to the output
        out_atten = atten;

        attenIntegr += atten * (depth * scaledLength);

        samplePoint += sampleRay;
    }

    out_groundCol = attenIntegr * (u_InvWavelength * u_KrESun + u_KmESun);
}

//==================================================================
void ONAS_CalcMieAndRayleighForSky(
            out vec3 out_mieCol,
            out vec3 out_rayleighCol,
            vec3 raySta,
            vec3 rayDir,
            float rayLen,
            float useOuterRadius,
            float near,
            float startDepth )
{
    vec3 baseCol = ONAS_RaytraceScatterSky(
                        raySta,
                        rayDir,
                        rayLen,
                        useOuterRadius,
                        near,
                        startDepth );

    out_mieCol      = baseCol *  u_KmESun;
    out_rayleighCol = baseCol * (u_KrESun * u_InvWavelength);
}

//==================================================================
// calculate the colors from inside the atmosphere
void ONAS_CalcMieAndRayleighForSkyInside(
            out vec3 out_mieCol,
            out vec3 out_rayleighCol,
            out vec3 out_posToCam,
            vec3 pos )
{
    // Get the ray from the camera to the vertex, and its length
    // (which is the far point of the ray passing through the atmosphere)
    vec3  raySta;
    vec3  rayDir;
    float rayLen;
    ONAS_CalcRayFromCameraLen( pos, raySta, rayDir, rayLen );

    float useOuterRadius = ONAS_CalcCamDistanceFromPlanetOrigin();

    float near = 0.0;

    float startDepth =
                exp( u_ScaleOverScaleDepth *
                        (u_InnerRadius - useOuterRadius) );

    ONAS_CalcMieAndRayleighForSky(
                            out_mieCol,
                            out_rayleighCol,
                            raySta,
                            rayDir,
                            rayLen,
                            useOuterRadius,
                            near,
                            startDepth );

    // necessary as a varying for the fragment shader
    out_posToCam = raySta - pos;
}

//==================================================================
// calculate the colors from outside the atmosphere
void ONAS_CalcMieAndRayleighForSkyOutside(
            out vec3 out_mieCol,
            out vec3 out_rayleighCol,
            out vec3 out_posToCam,
            vec3 pos )
{
    // Get the ray from the camera to the vertex, and its length
    // (which is the far point of the ray passing through the atmosphere)
    vec3  raySta;
    vec3  rayDir;
    float rayLen;
    ONAS_CalcRayFromCameraLen( pos, raySta, rayDir, rayLen );

    float useOuterRadius = u_OuterRadius;

    // Calculate the closest intersection of the ray with the outer atmosphere
    // (which is the near point of the ray passing through the atmosphere)
    float near = ONAS_CalcRaySphereClosestInters(
                                raySta,
                                rayDir,
                                vec3(0.0, 0.0, 0.0),
                                useOuterRadius * useOuterRadius );

    float startDepth = exp( -1.0 / u_ScaleDepth );

    ONAS_CalcMieAndRayleighForSky(
                            out_mieCol,
                            out_rayleighCol,
                            raySta,
                            rayDir,
                            rayLen,
                            useOuterRadius,
                            near,
                            startDepth );

    // necessary as a varying for the fragment shader
    out_posToCam = raySta - pos;
}

//==================================================================
// calculate the colors from inside the atmosphere
void ONAS_CalcColorsForGroundInside(
            out vec3 out_groundCol,
            out vec3 out_attenuation,
            vec3 pos )
{
    vec3  raySta;
    vec3  rayDir;
    float rayLen;
    ONAS_CalcRayFromCameraLen( pos, raySta, rayDir, rayLen );

    ONAS_RaytraceScatterGround(
            out_groundCol,
            out_attenuation,
            pos,
            raySta,
            rayDir,
            rayLen,
            ONAS_CalcCamDistanceFromPlanetOrigin(),
            0.0 );
}

//==================================================================
// calculate the colors from outside the atmosphere
void ONAS_CalcColorsForGroundOutside(
            out vec3 out_groundCol,
            out vec3 out_attenuation,
            vec3 pos )
{
    vec3  raySta;
    vec3  rayDir;
    float rayLen;
    ONAS_CalcRayFromCameraLen( pos, raySta, rayDir, rayLen );

    // Calculate the closest intersection of the ray with the outer atmosphere
    // (which is the near point of the ray passing through the atmosphere)
    float near = ONAS_CalcRaySphereClosestInters(
                                raySta,
                                rayDir,
                                vec3(0.0, 0.0, 0.0),
                                u_OuterRadius * u_OuterRadius );

    ONAS_RaytraceScatterGround(
            out_groundCol,
            out_attenuation,
            pos,
            raySta,
            rayDir,
            rayLen,
            u_OuterRadius,
            near );
}


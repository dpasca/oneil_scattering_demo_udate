//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "ONAS_Common.glsl"

void main(void)
{
    // Get the ray from the camera to the vertex and its length
    vec3 pos = gl_Vertex.xyz;

    vec3 raySta;
    vec3 rayDir;
    ONAS_CalcRayFromCamera( pos, raySta, rayDir );

    vec3 start = raySta;

    if ( length( u_CameraPos ) >= u_OuterRadius )
    {
        // Calculate the farther intersection of the ray with the outer atmosphere
        // (which is the far point of the ray passing through the atmosphere)
        float near = ONAS_CalcRaySphereClosestInters(
                            raySta,
                            rayDir,
                            vec3(0.0, 0.0, 0.0),
                            u_OuterRadius * u_OuterRadius );

        start += rayDir * near;
    }

    // Calculate attenuation from the camera to the top of the atmosphere toward the vertex
    float height = length(start);
    float useOuterRadius = ONAS_CalcCamDistanceFromPlanetOrigin();
    float depth = exp( u_ScaleOverScaleDepth * (u_InnerRadius - useOuterRadius) );
    float angle = dot(rayDir, start) / height;
    float scatter = depth * ONAS_Scale( angle );
    gl_FrontSecondaryColor.rgb = exp(-scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0].st = gl_MultiTexCoord0.st;
}

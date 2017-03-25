//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "AS_Common.glsl"

varying vec3 v_PosToCam;

void main(void)
{
	// Get the ray from the camera to the vertex, and its length
    // (which is the far point of the ray passing through the atmosphere)
	vec3 pos = gl_Vertex.xyz;
    vec3 raySta = u_CameraPos;
	vec3 rayDir = pos - raySta;
	float rayLength = length(rayDir);
	rayDir /= rayLength;

    float useOuterRadius = AS_CalcCamDistanceFromPlanetOrigin();

    float near = 0.0;



	float startDepth =
                exp( u_ScaleOverScaleDepth *
                        (u_InnerRadius - useOuterRadius) );

	// Now loop through the sample rays
	vec3 frontColor = AS_RaytraceScatterSky(
                            raySta,
                            rayDir,
                            rayLength,
                            useOuterRadius,
                            near,
                            startDepth );

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	gl_FrontSecondaryColor.rgb = frontColor * u_KmESun;
	gl_FrontColor.rgb = frontColor * (u_InvWavelength * u_KrESun);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	v_PosToCam = raySta - pos;
}


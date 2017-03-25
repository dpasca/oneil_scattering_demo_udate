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

    vec3  raySta;
	vec3  rayDir;
	float rayLen;
    AS_CalcRayFromCameraLen( pos, raySta, rayDir, rayLen );

    float useOuterRadius = u_OuterRadius;

	// Calculate the closest intersection of the ray with the outer atmosphere
    // (which is the near point of the ray passing through the atmosphere)
    float near = AS_CalcRaySphereClosestInters(
                                raySta,
                                rayDir,
                                PLANET_ORIGIN,
                                useOuterRadius * useOuterRadius );

	float startDepth = exp( -1.0 / u_ScaleDepth );

	// Now loop through the sample rays
	vec3 frontColor = AS_RaytraceScatterSky(
                            raySta,
                            rayDir,
                            rayLen,
                            useOuterRadius,
                            near,
                            startDepth );

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	gl_FrontSecondaryColor.rgb = frontColor * u_KmESun;
	gl_FrontColor.rgb = frontColor * (u_InvWavelength * u_KrESun);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	v_PosToCam = raySta - pos;
}


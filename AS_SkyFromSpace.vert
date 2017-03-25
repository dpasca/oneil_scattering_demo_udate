//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "AS_Common.h"

varying vec3 v_PosToCam;

void main(void)
{
	// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
	vec3 pos = gl_Vertex.xyz;
    vec3 raySta = u_CameraPos;
	vec3 rayDir = pos - raySta;
	float rayLength = length(rayDir);
	rayDir /= rayLength;

	// Calculate the closest intersection of the ray with the outer atmosphere
    // (which is the near point of the ray passing through the atmosphere)
    float near = AS_CalcRaySphereClosestInters(
                                raySta,
                                rayDir,
                                vec3(0.0, 0.0, 0.0), // sphere at origin
                                u_OuterRadius * u_OuterRadius );

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 start = raySta + rayDir * near;
    float segmentLength = rayLength - near;
	float startAngle = dot(rayDir, start) / u_OuterRadius;
	float startDepth = exp(-1.0 / u_ScaleDepth);
	float startOffset = startDepth * AS_Scale( startAngle );

	// Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	float sampleLength = segmentLength / SAMPLES_F;
	float scaledLength = sampleLength * u_Scale;
	vec3 sampleRay = rayDir * sampleLength;
	vec3 samplePoint = start + sampleRay * 0.5;

	// Now loop through the sample rays
	vec3 frontColor = AS_RaytraceScatterSky(
                            samplePoint,
                            startOffset,
                            scaledLength,
                            rayDir,
                            sampleRay );

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	gl_FrontSecondaryColor.rgb = frontColor * u_KmESun;
	gl_FrontColor.rgb          = frontColor * (u_InvWavelength * u_KrESun);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	v_PosToCam = raySta - pos;
}


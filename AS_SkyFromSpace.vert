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
	vec3 ray = pos - u_CameraPos;
	float far = length(ray);
	ray /= far;

	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float B = 2.0 * dot(u_CameraPos, ray);
	float C = (u_CameraHeight * u_CameraHeight) - (u_OuterRadius * u_OuterRadius);
	float det = max(0.0, B*B - 4.0 * C);
	float near = 0.5 * (-B - sqrt(det));

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 start = u_CameraPos + ray * near;
	far -= near;
	float startAngle = dot(ray, start) / u_OuterRadius;
	float startDepth = exp(-1.0 / u_ScaleDepth);
	float startOffset = startDepth * AS_Scale( startAngle );

	// Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	float sampleLength = far / SAMPLES_F;
	float scaledLength = sampleLength * u_Scale;
	vec3 sampleRay = ray * sampleLength;
	vec3 samplePoint = start + sampleRay * 0.5;

	// Now loop through the sample rays
	vec3 frontColor = vec3(0.0, 0.0, 0.0);
	for(int i=0; i < SAMPLES_N; ++i)
	{
		float height      = length(samplePoint);
		float depth       = exp(u_ScaleOverScaleDepth * (u_InnerRadius - height));
		float lightAngle  = dot(u_LightDir, samplePoint) / height;
		float cameraAngle = dot(ray, samplePoint) / height;

		float scatter     = startOffset +
                                    depth * (AS_Scale( lightAngle ) -
                                             AS_Scale( cameraAngle ));

		vec3 attenuate    = exp(-scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));

		frontColor += attenuate * (depth * scaledLength);
		samplePoint += sampleRay;
	}

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	gl_FrontSecondaryColor.rgb = frontColor * u_KmESun;
	gl_FrontColor.rgb          = frontColor * (u_InvWavelength * u_KrESun);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	v_PosToCam = u_CameraPos - pos;
}


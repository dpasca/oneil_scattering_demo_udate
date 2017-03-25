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
	// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 pos = gl_Vertex.xyz;
	vec3 ray = pos - u_CameraPos;
	float far = length(ray);
	ray /= far;

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 start = u_CameraPos;
	float height = length(start);
	float depth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - u_CameraHeight));
	float startAngle = dot(ray, start) / height;
	float startOffset = depth * AS_Scale( startAngle );

	// Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	float sampleLength = far / SAMPLES_F;
	float scaledLength = sampleLength * u_Scale;
	vec3 sampleRay = ray * sampleLength;
	vec3 samplePoint = start + sampleRay * 0.5;

	// Now loop through the sample rays
	vec3 frontColor = AS_RaytraceScatterSky(
                            samplePoint,
                            startOffset,
                            scaledLength,
                            ray,
                            sampleRay );

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	gl_FrontSecondaryColor.rgb = frontColor * u_KmESun;
	gl_FrontColor.rgb = frontColor * (u_InvWavelength * u_KrESun);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	v_PosToCam = u_CameraPos - pos;
}

//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "AS_Common.glsl"

void main(void)
{
	// Get the ray from the camera to the vertex and its length
	vec3 pos = gl_Vertex.xyz;
	vec3 ray = pos - u_CameraPos;
	float far = length(ray);
	ray /= far;

	// Calculate the farther intersection of the ray with the outer atmosphere (which is the far point of the ray passing through the atmosphere)
	float B = 2.0 * dot(u_CameraPos, ray);
	float C = (u_CameraHeight * u_CameraHeight) - (u_OuterRadius * u_OuterRadius);
	float det = max(0.0, B*B - 4.0 * C);
	far = 0.5 * (-B + sqrt(det));

	// Calculate attenuation from the camera to the top of the atmosphere toward the vertex
	vec3 start = u_CameraPos;
	float height = length(start);
	float depth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - u_CameraHeight));
	float angle = dot(ray, start) / height;
	float scatter = depth * AS_Scale( angle );
	gl_FrontSecondaryColor.rgb = exp(-scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0].st = gl_MultiTexCoord0.st;
}

//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "AS_Common.glsl"

float scale(float fCos)
{
	float x = 1.0 - fCos;
	return u_ScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main(void)
{
	// Get the ray from the camera to the vertex and its length
	vec3 v3Pos = gl_Vertex.xyz;
	vec3 v3Ray = v3Pos - u_CameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the farther intersection of the ray with the outer atmosphere (which is the far point of the ray passing through the atmosphere)
	float B = 2.0 * dot(u_CameraPos, v3Ray);
	float C = u_CameraHeight2 - u_OuterRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	fFar = 0.5 * (-B + sqrt(fDet));

	// Calculate attenuation from the camera to the top of the atmosphere toward the vertex
	vec3 v3Start = u_CameraPos;
	float fHeight = length(v3Start);
	float fDepth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - u_CameraHeight));
	float fAngle = dot(v3Ray, v3Start) / fHeight;
	float fScatter = fDepth*scale(fAngle);
	gl_FrontSecondaryColor.rgb = exp(-fScatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0].st = gl_MultiTexCoord0.st;
}

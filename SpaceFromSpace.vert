//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

uniform vec3  u_CameraPos;      // The camera's current position
uniform vec3  u_LightPos;       // The direction vector to the light source
uniform vec3  u_InvWavelength;  // 1 / pow(wavelength, 4) for the red, green, and blue channels
uniform float u_CameraHeight;   // The camera's current height
uniform float u_CameraHeight2;  // u_CameraHeight^2
uniform float u_OuterRadius;    // The outer (atmosphere) radius
uniform float u_OuterRadius2;   // u_OuterRadius^2
uniform float u_InnerRadius;    // The inner (planetary) radius
uniform float u_InnerRadius2;   // u_InnerRadius^2
uniform float u_KrESun;         // Kr * ESun
uniform float u_KmESun;         // Km * ESun
uniform float u_Kr4PI;          // Kr * 4 * PI
uniform float u_Km4PI;          // Km * 4 * PI
uniform float u_Scale;          // 1 / (u_OuterRadius - u_InnerRadius)
uniform float u_ScaleDepth;     // The scale depth (i.e. the altitude at which the atmosphere's average density is found)
uniform float u_ScaleOverScaleDepth; // u_Scale / u_ScaleDepth


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
	float fNear = 0.5 * (-B - sqrt(fDet));

	vec3 v3Start = u_CameraPos + v3Ray*fNear;
	fFar -= fNear;

	// Calculate attenuation from the camera to the top of the atmosphere toward the vertex
	float fHeight = length(v3Start);
	float fDepth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - u_CameraHeight));
	float fAngle = dot(v3Ray, v3Start) / fHeight;
	float fScatter = fDepth*scale(fAngle);
	gl_FrontSecondaryColor.rgb = exp(-fScatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0].st = gl_MultiTexCoord0.st;
}

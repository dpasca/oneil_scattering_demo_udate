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

const int nSamples = 2;
const float fSamples = 2.0;


float scale(float fCos)
{
	float x = 1.0 - fCos;
	return u_ScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main(void)
{
	// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
	vec3 v3Pos = gl_Vertex.xyz;
	vec3 v3Ray = v3Pos - u_CameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float B = 2.0 * dot(u_CameraPos, v3Ray);
	float C = u_CameraHeight2 - u_OuterRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	float fNear = 0.5 * (-B - sqrt(fDet));

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = u_CameraPos + v3Ray * fNear;
	fFar -= fNear;
	float fDepth = exp((u_InnerRadius - u_OuterRadius) / u_ScaleDepth);
	float fCameraAngle = dot(-v3Ray, v3Pos) / length(v3Pos);
	float fLightAngle = dot(u_LightPos, v3Pos) / length(v3Pos);
	float fCameraScale = scale(fCameraAngle);
	float fLightScale = scale(fLightAngle);
	float fCameraOffset = fDepth*fCameraScale;
	float fTemp = (fLightScale + fCameraScale);

	// Initialize the scattering loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * u_Scale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate;
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - fHeight));
		float fScatter = fDepth*fTemp - fCameraOffset;
		v3Attenuate = exp(-fScatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	gl_FrontColor.rgb = v3FrontColor * (u_InvWavelength * u_KrESun + u_KmESun);

	// Calculate the attenuation factor for the ground
	gl_FrontSecondaryColor.rgb = v3Attenuate;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
}

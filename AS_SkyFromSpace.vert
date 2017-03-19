//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "AS_Uniforms.h"

const int nSamples = 2;
const float fSamples = 2.0;

varying vec3 v3Direction;


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
	float fStartAngle = dot(v3Ray, v3Start) / u_OuterRadius;
	float fStartDepth = exp(-1.0 / u_ScaleDepth);
	float fStartOffset = fStartDepth*scale(fStartAngle);

	// Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * u_Scale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	for(int i=0; i<nSamples; i++)
	{
		float fHeight       = length(v3SamplePoint);
		float fDepth        = exp(u_ScaleOverScaleDepth * (u_InnerRadius - fHeight));
		float fLightAngle   = dot(u_LightPos, v3SamplePoint) / fHeight;
		float fCameraAngle  = dot(v3Ray, v3SamplePoint) / fHeight;

		float fScatter      = (fStartOffset +
                                    fDepth * (scale(fLightAngle) -
                                              scale(fCameraAngle)));

		vec3 v3Attenuate    = exp(-fScatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));

		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	gl_FrontSecondaryColor.rgb = v3FrontColor * u_KmESun;
	gl_FrontColor.rgb = v3FrontColor * (u_InvWavelength * u_KrESun);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	v3Direction = u_CameraPos - v3Pos;
}

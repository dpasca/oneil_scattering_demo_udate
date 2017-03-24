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
	// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 v3Pos = gl_Vertex.xyz;
	vec3 v3Ray = v3Pos - u_CameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = u_CameraPos;
	float fDepth = exp((u_InnerRadius - u_CameraHeight) / u_ScaleDepth);
	float fCameraAngle = dot(-v3Ray, v3Pos) / length(v3Pos);
	float fLightAngle = dot(u_LightDir, v3Pos) / length(v3Pos);
	float fCameraScale = AS_Scale( fCameraAngle );
	float fLightScale = AS_Scale( fLightAngle );
	float fCameraOffset = fDepth*fCameraScale;
	float fTemp = (fLightScale + fCameraScale);

	// Initialize the scattering loop variables
	float fSampleLength = fFar / SAMPLES_F;
	float fScaledLength = fSampleLength * u_Scale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate;
	for(int i=0; i < SAMPLES_N; ++i)
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

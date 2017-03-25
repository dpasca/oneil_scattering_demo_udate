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
	float depth = exp((u_InnerRadius - u_OuterRadius) / u_ScaleDepth);
	float cameraAngle = dot(-ray, pos) / length(pos);
	float lightAngle = dot(u_LightDir, pos) / length(pos);
	float cameraScale = AS_Scale( cameraAngle );
	float lightScale = AS_Scale( lightAngle );
	float cameraOffset = depth*cameraScale;
	float temp = (lightScale + cameraScale);

	// Initialize the scattering loop variables
	float sampleLength = far / SAMPLES_F;
	float scaledLength = sampleLength * u_Scale;
	vec3 sampleRay = ray * sampleLength;
	vec3 samplePoint = start + sampleRay * 0.5;

	// Now loop through the sample rays
	vec3 frontColor = vec3(0.0, 0.0, 0.0);
	vec3 attenuate;
	for(int i=0; i < SAMPLES_N; ++i)
	{
		float height = length(samplePoint);
		float depth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - height));
		float scatter = depth*temp - cameraOffset;
		attenuate = exp(-scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));
		frontColor += attenuate * (depth * scaledLength);
		samplePoint += sampleRay;
	}

	gl_FrontColor.rgb = frontColor * (u_InvWavelength * u_KrESun + u_KmESun);

	// Calculate the attenuation factor for the ground
	gl_FrontSecondaryColor.rgb = attenuate;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
}

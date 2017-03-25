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
    //  (which is the far point of the ray passing through the atmosphere)
	vec3 pos = gl_Vertex.xyz;
	vec3 ray = pos - u_CameraPos;
	float rayLength = length(ray);
	ray /= rayLength;

    vec3 attenuation = vec3(0.0 ,0.0, 0.0);
    vec3 frontColor = AS_RaytraceScatterGround(
                            pos,
                            ray,
                            u_CameraHeight,
                            0.0,
                            rayLength,
                            attenuation );

	gl_FrontColor.rgb = frontColor * (u_InvWavelength * u_KrESun + u_KmESun);

	// Calculate the attenuation factor for the ground
	gl_FrontSecondaryColor.rgb = attenuation;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
}

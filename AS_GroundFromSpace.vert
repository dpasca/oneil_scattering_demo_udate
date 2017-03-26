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

    vec3 groundCol;
    vec3 attenuation;
    AS_CalcColorsForGroundOutside( groundCol, attenuation, pos );

	gl_FrontColor.rgb = groundCol;
	gl_FrontSecondaryColor.rgb = attenuation;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
}

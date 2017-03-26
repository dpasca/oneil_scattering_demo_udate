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
	vec3 pos = gl_Vertex.xyz;

    vec3 mieCol;
    vec3 rayleighCol;
    vec3 posToCam;
    AS_CalcMieAndRayleighForSkyInside( mieCol, rayleighCol, posToCam, pos );

    gl_FrontSecondaryColor.rgb = mieCol;
    gl_FrontColor.rgb = rayleighCol;

    v_PosToCam = posToCam;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}


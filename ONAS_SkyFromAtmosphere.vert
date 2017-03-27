//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "ONAS_Common.glsl"

varying vec3 v_mieCol;
varying vec3 v_rayleighCol;
varying vec3 v_posToCam;

void main(void)
{
    vec3 pos = gl_Vertex.xyz;

    ONAS_CalcMieAndRayleighForSkyInside( v_mieCol, v_rayleighCol, v_posToCam, pos );

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}


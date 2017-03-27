//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "ONAS_Common.glsl"

varying vec3 v_groundCol;
varying vec3 v_attenuation;

void main(void)
{
    // Get the ray from the camera to the vertex and its length
    //  (which is the far point of the ray passing through the atmosphere)
    vec3 pos = gl_Vertex.xyz;

    ONAS_CalcColorsForGroundOutside( v_groundCol, v_attenuation, pos );

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
}

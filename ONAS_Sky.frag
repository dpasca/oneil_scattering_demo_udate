//
// Atmospheric scattering fragment shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "ONAS_Common.h"

varying vec3 v_mieCol;
varying vec3 v_rayleighCol;
varying vec3 v_posToCam;

void main()
{
    float cosA = dot(u_LightDir, v_posToCam) / length(v_posToCam);

    float miePhase = ONAS_CalcMiePhase( cosA, u_g );

    gl_FragColor.rgb = v_rayleighCol + miePhase * v_mieCol;
    gl_FragColor.a = gl_FragColor.b;
}



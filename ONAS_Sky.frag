//
// Atmospheric scattering fragment shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "ONAS_Common.h"

varying vec3 v_PosToCam;

void main()
{
    float cosA = dot(u_LightDir, v_PosToCam) / length(v_PosToCam);

    float miePhase = ONAS_CalcMiePhase( cosA, u_g );

    gl_FragColor = gl_Color + miePhase * gl_SecondaryColor;
    gl_FragColor.a = gl_FragColor.b;
}



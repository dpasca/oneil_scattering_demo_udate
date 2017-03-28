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

#define OVERWRITE_NAN_WITH_DEBUG_COLOR

void main()
{
    float cosA = dot(u_LightDir, v_posToCam) / length(v_posToCam);

    float miePhase = ONAS_CalcMiePhase( cosA, u_g );

    vec3 outCol = v_rayleighCol + miePhase * v_mieCol;

#ifdef OVERWRITE_NAN_WITH_DEBUG_COLOR
    if ( outCol != outCol )
        outCol = vec4( 1.0, 0.0, 1.0, 1.0 );
#endif

    gl_FragColor = vec4( outCol, 1.0 );
}


//
// Atmospheric scattering fragment shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "AS_Uniforms.h"

varying vec3 v_PosToCam;


void main (void)
{
	float fCos = dot(u_LightPos, v_PosToCam) / length(v_PosToCam);

	float fMiePhase =
                1.5 * ((1.0 - u_g2) / (2.0 + u_g2)) *
                    (1.0 + fCos*fCos) /
                        pow(1.0 + u_g2 - 2.0*u_g*fCos, 1.5);

    gl_FragColor = gl_Color + fMiePhase * gl_SecondaryColor;
	gl_FragColor.a = gl_FragColor.b;
}

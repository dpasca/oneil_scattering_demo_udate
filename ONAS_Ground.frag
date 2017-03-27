//
// Atmospheric scattering fragment shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#define ENABLE_TEXTURE

#ifdef ENABLE_TEXTURE
uniform sampler2D s2Tex1;
//uniform sampler2D s2Tex2;
#endif

varying vec3 v_groundCol;
varying vec3 v_attenuation;

void main (void)
{
#ifdef ENABLE_TEXTURE
    gl_FragColor.rgb = v_groundCol +
                       texture2D(s2Tex1, gl_TexCoord[0].st) *
                       //texture2D(s2Tex2, gl_TexCoord[1].st) *
                       v_attenuation;
#else
    gl_FragColor.rgb = v_groundCol + 0.25 * v_attenuation;
#endif

    gl_FragColor.a = 1.0;
}

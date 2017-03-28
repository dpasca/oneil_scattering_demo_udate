//
// Atmospheric scattering fragment shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#define ENABLE_TEXTURE
#define OVERWRITE_NAN_WITH_DEBUG_COLOR

#ifdef ENABLE_TEXTURE
uniform sampler2D s2Tex1;
//uniform sampler2D s2Tex2;
#endif

varying vec3 v_groundCol;
varying vec3 v_attenuation;

void main (void)
{
#ifdef ENABLE_TEXTURE
    vec3 outCol = v_groundCol +
                       texture2D(s2Tex1, gl_TexCoord[0].st) *
                       //texture2D(s2Tex2, gl_TexCoord[1].st) *
                       v_attenuation;
#else
    vec3 outCol = v_groundCol + 0.25 * v_attenuation;
#endif

#ifdef OVERWRITE_NAN_WITH_DEBUG_COLOR
    if ( outCol != outCol )
        outCol = vec4( 1.0, 0.0, 1.0, 1.0 );
#endif

    gl_FragColor = vec4( outCol, 1.0 );
}


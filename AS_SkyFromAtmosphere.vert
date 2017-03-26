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
	// Get the ray from the camera to the vertex, and its length
    // (which is the far point of the ray passing through the atmosphere)
	vec3 pos = gl_Vertex.xyz;

    vec3  raySta;
	vec3  rayDir;
	float rayLen;
    AS_CalcRayFromCameraLen( pos, raySta, rayDir, rayLen );

    float useOuterRadius = AS_CalcCamDistanceFromPlanetOrigin();

    float near = 0.0;



	float startDepth =
                exp( u_ScaleOverScaleDepth *
                        (u_InnerRadius - useOuterRadius) );

    vec3 mieCol;
    vec3 rayleighCol;
    AS_CalcMieAndRayleighForSky(
                            mieCol,
                            rayleighCol,
                            raySta,
                            rayDir,
                            rayLen,
                            useOuterRadius,
                            near,
                            startDepth );

    gl_FrontSecondaryColor.rgb = mieCol;
    gl_FrontColor.rgb = rayleighCol;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	v_PosToCam = raySta - pos;
}


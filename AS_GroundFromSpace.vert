//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

//#include "AS_Common.glsl"

void main(void)
{
	// Get the ray from the camera to the vertex and its length
    //  (which is the far point of the ray passing through the atmosphere)
	vec3 pos = gl_Vertex.xyz;

    vec3  raySta;
	vec3  rayDir;
	float rayLen;
    AS_CalcRayFromCameraLen( pos, raySta, rayDir, rayLen );

	// Calculate the closest intersection of the ray with the outer atmosphere
    // (which is the near point of the ray passing through the atmosphere)
    float near = AS_CalcRaySphereClosestInters(
                                raySta,
                                rayDir,
                                u_PlanetPos,
                                u_OuterRadius * u_OuterRadius );

    vec3 attenuation = vec3(0.0 ,0.0, 0.0);
    vec3 groundCol = AS_RaytraceScatterGround(
                            pos,
                            raySta,
                            rayDir,
                            rayLen,
                            u_OuterRadius,
                            near,
                            attenuation );

	gl_FrontColor.rgb = groundCol;
	gl_FrontSecondaryColor.rgb = attenuation;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
}

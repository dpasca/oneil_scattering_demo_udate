//
// Atmospheric scattering vertex shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//
// Modified by Davide Pasca (2017)

uniform vec3  u_CameraPos;      // The camera's current position
uniform vec3  u_LightDir;       // The direction vector to the light source
uniform vec3  u_InvWavelength;  // 1 / pow(wavelength, 4) for the red, green, and blue channels
uniform float u_CameraHeight;   // The camera's current height
uniform float u_OuterRadius;    // The outer (atmosphere) radius
uniform float u_InnerRadius;    // The inner (planetary) radius
uniform float u_KrESun;         // Kr * ESun
uniform float u_KmESun;         // Km * ESun
uniform float u_Kr4PI;          // Kr * 4 * PI
uniform float u_Km4PI;          // Km * 4 * PI
uniform float u_Scale;          // 1 / (u_OuterRadius - u_InnerRadius)
uniform float u_ScaleDepth;     // The scale depth (i.e. the altitude at which the atmosphere's average density is found)
uniform float u_ScaleOverScaleDepth; // u_Scale / u_ScaleDepth

uniform float u_g;

const int   SAMPLES_N = 2;
const float SAMPLES_F = 2.0;

//==================================================================
float AS_Scale( float cosA )
{
	float x = 1.0 - cosA;

	return
        u_ScaleDepth *
            exp( -0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))) );
}

//==================================================================
float AS_CalcMiePhase( float cosA, float g )
{
    float g2 = g * g;

    return 1.5 *
            ((1.0 - g2) / (2.0 + g2)) *
                (1.0 + cosA * cosA) /
                    pow(1.0 + g2 - 2.0 * g * cosA, 1.5);
}

//==================================================================
vec3 AS_RaytraceScatterSky(
            vec3 samplePoint,
            float startOffset,
            float scaledLength,
            vec3 ray,
            vec3 sampleRay )
{
	vec3 out_col = vec3(0.0, 0.0, 0.0);

	for(int i=0; i < SAMPLES_N; ++i)
	{
		float height = length(samplePoint);

		float depth = exp( u_ScaleOverScaleDepth * (u_InnerRadius - height) );

		float lightAngle = dot( u_LightDir, samplePoint ) / height;

		float cameraAngle = dot( ray, samplePoint ) / height;

		float scatter = startOffset +
                            depth * (AS_Scale( lightAngle ) -
                                     AS_Scale( cameraAngle ));

		vec3 atten = exp( -scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI) );

		out_col += atten * (depth * scaledLength);

		samplePoint += sampleRay;
	}

    return out_col;
}

//==================================================================
vec3 AS_RaytraceScatterGround(
            vec3 pos,
            vec3 ray,
            float useOuterRadius,
            float near,
            float rayLength,
            out vec3 out_atten )
{
	// Calculate the ray's starting position,
    //   then calculate its scattering offset

    vec3 samplePointStart = u_CameraPos + (ray * near);
    float segmentLength  = rayLength - near;

	float depth = exp((u_InnerRadius - useOuterRadius) / u_ScaleDepth);

    float posLen = length( pos );
	float cameraAngle = dot(-ray, pos) / posLen;
	float lightAngle = dot(u_LightDir, pos) / posLen;
	float cameraScale = AS_Scale( cameraAngle );
	float lightScale = AS_Scale( lightAngle );
	float cameraOffset = depth * cameraScale;
	float temp = (lightScale + cameraScale);

	// Initialize the scattering loop variables
	float sampleLength = segmentLength / SAMPLES_F;
	float scaledLength = sampleLength * u_Scale;
	vec3 sampleRay = ray * sampleLength;
	vec3 samplePoint = samplePointStart + sampleRay * 0.5;

	// Now loop through the sample rays
	vec3 out_col = vec3(0.0, 0.0, 0.0);
	for(int i=0; i < SAMPLES_N; ++i)
	{
		float height = length(samplePoint);

		float depth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - height));

		float scatter = depth * temp - cameraOffset;

		vec3 atten = exp( -scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI) );

        // last attentuation goes to the output
        out_atten = atten;

		out_col += atten * (depth * scaledLength);

		samplePoint += sampleRay;
	}

    return out_col;
}


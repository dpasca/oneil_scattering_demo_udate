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
uniform float u_CameraHeight2;  // u_CameraHeight^2
uniform float u_OuterRadius;    // The outer (atmosphere) radius
uniform float u_OuterRadius2;   // u_OuterRadius^2
uniform float u_InnerRadius;    // The inner (planetary) radius
uniform float u_InnerRadius2;   // u_InnerRadius^2
uniform float u_KrESun;         // Kr * ESun
uniform float u_KmESun;         // Km * ESun
uniform float u_Kr4PI;          // Kr * 4 * PI
uniform float u_Km4PI;          // Km * 4 * PI
uniform float u_Scale;          // 1 / (u_OuterRadius - u_InnerRadius)
uniform float u_ScaleDepth;     // The scale depth (i.e. the altitude at which the atmosphere's average density is found)
uniform float u_ScaleOverScaleDepth; // u_Scale / u_ScaleDepth

uniform float u_g;
uniform float u_g2;


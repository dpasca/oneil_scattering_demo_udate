//==================================================================
/// AS_State.h
///
/// Created by Davide Pasca - 2017/3/26
///
/// Based on the GPU Gems 2 demo by Sean O'Neil
//==================================================================

#ifndef AS_STATE_H
#define AS_STATE_H

#include <functional>

//==================================================================
class AS_State
{
public:
    using SetUniform1F_FnT = std::function<void (const char *, float)>;
    using SetUniform3F_FnT = std::function<void (const char *, const float *)>;

	float m_Kr      =  0.0025f; // Rayleigh scattering constant
	float m_Km      =  0.0010f; // Mie scattering constant
	float m_ESun    =  20.0f;   // Sun brightness constant
	float m_g       = -0.990f;  // The Mie phase asymmetry factor

	float m_InnerRadius        = 10.0f;
	float m_OuterRadius        = 10.25f;
	float m_Wavelength[3]      = {0,0,0};
	float m_RayleighScaleDepth = 0;
	float m_MieScaleDepth      = 0;

public:
    AS_State()
    {
        m_Wavelength[0] = 0.650f;		// 650 nm for red
        m_Wavelength[1] = 0.570f;		// 570 nm for green
        m_Wavelength[2] = 0.475f;		// 475 nm for blue

        m_RayleighScaleDepth = 0.25f;
        m_MieScaleDepth = 0.1f;
    }

    void SetPlanetRadius( float innerR )
    {
        m_InnerRadius = innerR;
        m_OuterRadius = innerR * 1.025f;
    }

    //
    void UpdateShaderUniforms(
            const float camPos[3],
            const float planetPos[3],
            const float lightDir[3],
            const SetUniform1F_FnT &setUniform1f,
            const SetUniform3F_FnT &setUniform3f ) const
    {
        static const double pi = 3.14159265358979323846;

        setUniform3f( "u_CameraPos", camPos );
        setUniform3f( "u_LightDir", lightDir );

        float ooWaveLen4[3] =
        {
            1 / powf( m_Wavelength[0], 4.0f ),
            1 / powf( m_Wavelength[1], 4.0f ),
            1 / powf( m_Wavelength[2], 4.0f )
        };
        setUniform3f( "u_InvWavelength", ooWaveLen4 );

        setUniform1f( "u_InnerRadius", m_InnerRadius );
        setUniform1f( "u_OuterRadius", m_OuterRadius );
        setUniform1f( "u_KrESun", m_Kr * m_ESun );
        setUniform1f( "u_KmESun", m_Km * m_ESun );
        setUniform1f( "u_Kr4PI", m_Kr * (float)(4.0 * pi) );
        setUniform1f( "u_Km4PI", m_Km * (float)(4.0 * pi) );
        setUniform1f( "u_Scale", 1.0f / (m_OuterRadius - m_InnerRadius) );
        setUniform1f( "u_ScaleDepth", m_RayleighScaleDepth );

        setUniform1f( "u_ScaleOverScaleDepth",
                1.0f / ((m_OuterRadius - m_InnerRadius) * m_RayleighScaleDepth) );

        setUniform1f( "u_g", m_g );
    }
};

#endif


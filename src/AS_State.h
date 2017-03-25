//==================================================================
/// AS_State.h
///
/// Created by Davide Pasca - 2017/3/26
///
/// Based on the GPU Gems 2 demo by Sean O'Neil
//==================================================================

#ifndef AS_STATE_H
#define AS_STATE_H

//==================================================================
class AS_State
{
public:
	float m_Kr      =  0.0025f; // Rayleigh scattering constant
	float m_Km      =  0.0010f; // Mie scattering constant
	float m_ESun    =  20.0f;   // Sun brightness constant
	float m_g       = -0.990f;  // The Mie phase asymmetry factor

	float m_fInnerRadius        = 10.0f;
	float m_fOuterRadius        = 10.25f;
	float m_fWavelength[3]      = {0,0,0};
	float m_fWavelength4[3]     = {0,0,0};
	float m_fRayleighScaleDepth = 0;
	float m_fMieScaleDepth      = 0;

    AS_State()
    {
        m_fWavelength[0] = 0.650f;		// 650 nm for red
        m_fWavelength[1] = 0.570f;		// 570 nm for green
        m_fWavelength[2] = 0.475f;		// 475 nm for blue
        m_fWavelength4[0] = powf(m_fWavelength[0], 4.0f);
        m_fWavelength4[1] = powf(m_fWavelength[1], 4.0f);
        m_fWavelength4[2] = powf(m_fWavelength[2], 4.0f);

        m_fRayleighScaleDepth = 0.25f;
        m_fMieScaleDepth = 0.1f;
    }
};

#endif


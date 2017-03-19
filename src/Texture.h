/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __Texture_h__
#define __Texture_h__

#include "PixelBuffer.h"
#include "GLUtil.h"

/*******************************************************************************
* Class: CTexture
********************************************************************************
* This class encapsulates OpenGL texture objects. You initialize it with a
* CPixelBuffer instance and a flag indicating whether you want mipmaps to be
* generated.
*******************************************************************************/
class CTexture
{
protected:
	int m_nType;					// GL_TEXTURE_1D, GL_TEXTURE_2D, or GL_TEXTURE_3D
	unsigned int m_nID;				// OpenGL-generated texture ID

	static CTexture m_tCloudCell;		// Shared cloud cell texture
	static CTexture m_t1DGlow;

public:

	CTexture()		{ m_nID = -1; }
	CTexture(CPixelBuffer *pBuffer, bool bMipmap=true)
	{
		m_nID = -1;
		Init(pBuffer, bMipmap);
	}
	~CTexture()		{ Cleanup(); }
	void Cleanup()
	{
		if(m_nID != -1)
		{
			glDeleteTextures(1, &m_nID);
			m_nID = -1;
		}
	}

	static void InitStaticMembers(int nSeed, int nSize);
	static CTexture &GetCloudCell()			{ return m_tCloudCell; }
	static CTexture &Get1DGlow()			{ return m_t1DGlow; }
	static CTexture &Get3DNoise()			{ return m_t1DGlow; }
	static void Enable(int nType)			{ glEnable(nType); }
	static void Disable(int nType)			{ glDisable(nType); }
	
	DWORD GetID()						{ return m_nID; }
	int GetType()						{ return m_nType; }
	void BindTexture()					{ assert( m_nID != -1 ); glBindTexture(m_nType, m_nID); }
	void EnableTexture()				{ BindTexture(); glEnable(m_nType); }
	void DisableTexture()				{ assert( m_nID != -1 ); glDisable(m_nType); }

	void Init(CPixelBuffer *pBuffer, bool bClamp=true, bool bMipmap=true);
	void Update(CPixelBuffer *pBuffer, int nLevel=0);

	// Use when rendering to texture (either in the back buffer or a CPBuffer)
	void InitCopy(int x, int y, int nWidth, int nHeight, bool bClamp=true);
};

#endif // __Texture_h__


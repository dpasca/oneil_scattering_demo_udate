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

#include "Master.h"
#include "Texture.h"

CTexture CTexture::m_tCloudCell;
CTexture CTexture::m_t1DGlow;

void CTexture::InitStaticMembers(int nSeed, int nSize)
{
	// Initialize the shared cloud cell texture
    {
	auto pb = CPixelBuffer(16, 16, 1, 2, GL_LUMINANCE_ALPHA);
	pb.MakeCloudCell(2, 0);
	m_tCloudCell.Init(&pb);
    }

    {
	auto pb = CPixelBuffer(64, 1, 1, 2, GL_LUMINANCE_ALPHA);
	pb.MakeGlow1D();
	m_t1DGlow.Init(&pb);
    }
}

void CTexture::Init(CPixelBuffer *pBuffer, bool bClamp, bool bMipmap)
{
	Cleanup();
	m_nType = pBuffer->GetHeight() == 1 ? GL_TEXTURE_1D : GL_TEXTURE_2D;

	glGenTextures(1, &m_nID);

	auto bindScope = BindTexture();

	//glTexParameteri(m_nType, GL_TEXTURE_WRAP_R, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_WRAP_S, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_WRAP_T, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_nType, GL_TEXTURE_MIN_FILTER, bMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	switch(m_nType)
	{
    case GL_TEXTURE_1D:
        glTexImage1D(
                m_nType,
                0,
                pBuffer->GetChannels(),
                pBuffer->GetWidth(),
                0,
                pBuffer->GetFormat(),
                pBuffer->GetDataType(),
                pBuffer->GetBuffer());

        if ( bMipmap )
            glGenerateMipmap( GL_TEXTURE_1D );
        break;

    case GL_TEXTURE_2D:
        glTexImage2D(
                m_nType,
                0,
                pBuffer->GetChannels(),
                pBuffer->GetWidth(),
                pBuffer->GetHeight(),
                0,
                pBuffer->GetFormat(),
                pBuffer->GetDataType(),
                pBuffer->GetBuffer());

        if ( bMipmap )
            glGenerateMipmap( GL_TEXTURE_2D );
        break;

    default: assert(0); break;
	}
}

void CTexture::Update(CPixelBuffer *pBuffer, int nLevel)
{
	auto bindScope = BindTexture();

	switch(m_nType)
	{
	case GL_TEXTURE_1D:
		glTexSubImage1D(
                m_nType,
                nLevel,
                0,
                pBuffer->GetWidth(),
                pBuffer->GetFormat(),
                pBuffer->GetDataType(),
                pBuffer->GetBuffer() );
		break;

	case GL_TEXTURE_2D:
		glTexSubImage2D(
                m_nType,
                nLevel,
                0,
                0,
                pBuffer->GetWidth(),
                pBuffer->GetHeight(),
                pBuffer->GetFormat(),
                pBuffer->GetDataType(),
                pBuffer->GetBuffer() );
		break;

    default: assert(0); break;
	}
}

void CTexture::InitCopy(int x, int y, int nWidth, int nHeight, bool bClamp)
{
	Cleanup();
	m_nType = nHeight == 1 ? GL_TEXTURE_1D : GL_TEXTURE_2D;
	glGenTextures(1, &m_nID);

	auto bindScope = BindTexture();

	glTexParameteri(m_nType, GL_TEXTURE_WRAP_S, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_WRAP_T, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_nType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	switch(m_nType)
	{
	case GL_TEXTURE_1D:
		glCopyTexImage1D(m_nType, 0, GL_RGBA, x, y, nWidth, 0);
		break;
	case GL_TEXTURE_2D:
		glCopyTexImage2D(m_nType, 0, GL_RGBA, x, y, nWidth, nHeight, 0);
		break;

    default: assert(0); break;
	}
}


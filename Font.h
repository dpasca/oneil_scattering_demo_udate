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

#ifndef __Font_h__
#define __Font_h__

//#define DISABLE_CFONT // DAVIDE - necessary to use NVIDIA Nsight

/*******************************************************************************
* Class: CFont
********************************************************************************
* This is just a quick and dirty class for using wglUseFontBitmaps. I don't like
* using it because it's slow, but for now that's all I have to use for fonts.
* It would be nice to test drawing to a normal Windows bitmap using GDI commands,
* then blitting the text to OpenGL somehow.
*******************************************************************************/
class CFont
{
protected:
	int m_nListBase;
	float m_fXPos;
	float m_fYPos;

public:
	CFont(HDC hDC=NULL)
	{
#ifndef DISABLE_CFONT
		m_nListBase = -1;
		m_fXPos = 0;
		m_fYPos = 0;
		if(hDC)
		{
			m_nListBase = glGenLists(256);
			wglUseFontBitmaps(hDC, 0, 255, m_nListBase);
		}
#endif
	}
	~CFont()	{ Cleanup(); }
	void Init(HDC hDC)
	{
#ifndef DISABLE_CFONT
		Cleanup();
		m_nListBase = glGenLists(256);
		wglUseFontBitmaps(hDC, 0, 255, m_nListBase);
#endif
	}
	void Cleanup()
	{
#ifndef DISABLE_CFONT
		if(m_nListBase != -1)
		{
			glDeleteLists(m_nListBase, 256);
			m_nListBase = -1;
		}
#endif
	}
	void SetPosition(int x, int y)
	{
		m_fXPos = (float)x;
		m_fYPos = (float)y;
	}
	void Begin()
	{
#ifndef DISABLE_CFONT
		glDisable(GL_LIGHTING);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, GetGameApp()->GetWidth(), GetGameApp()->GetHeight(), 0, -1, 1);
#endif
	}
	void Print(const char *pszMessage)
	{
#ifndef DISABLE_CFONT
		glRasterPos2f(m_fXPos, m_fYPos+11);
		glListBase(m_nListBase);
		glCallLists(strlen(pszMessage), GL_UNSIGNED_BYTE, pszMessage);
#endif
	}
	void End()
	{
#ifndef DISABLE_CFONT
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glEnable(GL_LIGHTING);
#endif
	}
};

#endif // __Font_h__


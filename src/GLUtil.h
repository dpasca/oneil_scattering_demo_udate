// GLUtil.h
//

#ifndef __GLUtil_h__
#define __GLUtil_h__

#include "Texture.h"


class CGLUtil
{
// Attributes
protected:
	// Standard OpenGL members
	HDC     m_hDC = nullptr;
	HGLRC   m_hGLRC = nullptr;
	bool    m_bATI = false;

public:
	static CGLUtil *m_pMain;

// Operations
public:
	void Init();
	void InitRenderContext(HDC hDC=NULL, HGLRC hGLRC=NULL);

	HDC GetHDC()					{ return m_hDC; }
	HGLRC GetHGLRC()				{ return m_hGLRC; }
	void MakeCurrent()				{ wglMakeCurrent(m_hDC, m_hGLRC); }
	bool IsATI()					{ return m_bATI; }
};

inline CGLUtil *GLUtil()			{ return CGLUtil::m_pMain; }

//
void GLUTIL_SetupErrorIntercept();

#endif // __GLUtil_h__



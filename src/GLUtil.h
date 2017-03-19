// GLUtil.h
//

#ifndef __GLUtil_h__
#define __GLUtil_h__

#include <assert.h>

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

//==================================================================
class TextureBindScope
{
    GLenum mTarget = 0;

public:
    TextureBindScope() {}

    TextureBindScope( GLenum target, GLuint texID )
        : mTarget(target)
    {
        assert( texID != (GLuint)-1 && texID != 0 );

        glBindTexture( mTarget, texID );
    }

    ~TextureBindScope()
    {
        if ( mTarget )
            glBindTexture( mTarget, 0 );
    }

    // move constructor
    TextureBindScope( TextureBindScope &&other )
    {
        mTarget = std::move( other.mTarget );
        other.mTarget = 0;
    }

    // move assignment operator
    TextureBindScope &operator=( TextureBindScope &&other )
    {
        mTarget = std::move( other.mTarget );
        other.mTarget = 0;
    }
};

#endif // __GLUtil_h__



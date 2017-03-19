//==================================================================
// ShaderObject.h
//==================================================================

#ifndef __ShaderObject_h__
#define __ShaderObject_h__

#include <map>
#include <string>

//==================================================================
class CShaderObject
{
protected:
	GLhandleARB m_hProgram = 0;
	GLhandleARB m_hVertexShader = 0;
	GLhandleARB m_hFragmentShader = 0;
	std::map<std::string, GLint> m_mapParameters;

	void LogGLErrors()
	{
		GLenum glErr;
		while((glErr = glGetError()) != GL_NO_ERROR)
			LogError((const char *)gluErrorString(glErr));
	}
	void LogGLInfoLog(GLhandleARB hObj)
	{
		int nBytes;
		glGetObjectParameterivARB(hObj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &nBytes);
		if(nBytes)
		{
			char *pInfo = new char[nBytes];
			glGetInfoLogARB(hObj, nBytes, &nBytes, pInfo);
			LogInfo(pInfo);
			delete pInfo;
		}
	}

public:
	CShaderObject();
	~CShaderObject();

	bool LoadFromFile(const char *pszPath, const char *pszPath2=NULL);

	void Enable();
	void Disable();

	GLint GetUniformParameterID(const char *pszParameter);

	void SetUniformParameter1i(const char *pszParameter, int n1);
	void SetUniformParameter1f(const char *pszParameter, float p1);
	void SetUniformParameter3f(const char *pszParameter, float p1, float p2, float p3);
};

#endif // __ShaderObject_h__



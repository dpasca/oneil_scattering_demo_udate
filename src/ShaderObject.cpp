//==================================================================
// ShaderObject.cpp
//==================================================================

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <fstream>
#include <GL\glew.h>
#include <GL\wglew.h>
#include <GL\gl.h>
#include <GL\glu.h>
#include "Log.h"
#include "ShaderObject.h"

//==================================================================
CShaderObject::CShaderObject()
{
    m_hProgram = glCreateProgramObjectARB();
    m_hVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    m_hFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
}

CShaderObject::~CShaderObject()
{
    glDeleteObjectARB(m_hFragmentShader);
    glDeleteObjectARB(m_hVertexShader);
    glDeleteObjectARB(m_hProgram);
}

bool CShaderObject::LoadFromFile(const char *pszPath, const char *pszPath2)
{
    char szPath[_MAX_PATH], *psz;
    int nBytes, bSuccess;

    sprintf(szPath, "%s.vert", pszPath);
    LogInfo("Compiling GLSL shader %s", szPath);
    std::ifstream ifVertexShader(szPath, std::ios::binary);
    ifVertexShader.seekg(0, std::ios::end);
    nBytes = (int)ifVertexShader.tellg();
    ifVertexShader.seekg(0, std::ios::beg);
    psz = new char[nBytes+1];
    ifVertexShader.read(psz, nBytes);
    psz[nBytes] = 0;
    ifVertexShader.close();
    glShaderSourceARB(m_hVertexShader, 1, (const char **)&psz, &nBytes);
    glCompileShaderARB(m_hVertexShader);
    glGetObjectParameterivARB(m_hVertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &bSuccess);
    delete psz;
    if(!bSuccess)
    {
        LogError("Failed to compile vertex shader %s", szPath);
        LogGLErrors();
        LogGLInfoLog(m_hVertexShader);
        return false;
    }

    sprintf(szPath, "%s.frag", pszPath2 ? pszPath2 : pszPath);
    LogInfo("Compiling GLSL shader %s", szPath);
    std::ifstream ifFragmentShader(szPath, std::ios::binary);
    ifFragmentShader.seekg(0, std::ios::end);
    nBytes = (int)ifFragmentShader.tellg();
    ifFragmentShader.seekg(0, std::ios::beg);
    psz = new char[nBytes];
    ifFragmentShader.read(psz, nBytes);
    ifFragmentShader.close();
    glShaderSourceARB(m_hFragmentShader, 1, (const char **)&psz, &nBytes);
    glCompileShaderARB(m_hFragmentShader);
    glGetObjectParameterivARB(m_hFragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &bSuccess);
    delete psz;
    if(!bSuccess)
    {
        LogError("Failed to compile fragment shader %s", szPath);
        LogGLErrors();
        LogGLInfoLog(m_hFragmentShader);
        return false;
    }

    glAttachObjectARB(m_hProgram, m_hVertexShader);
    glAttachObjectARB(m_hProgram, m_hFragmentShader);
    glLinkProgramARB(m_hProgram);

    glGetObjectParameterivARB(m_hProgram, GL_OBJECT_LINK_STATUS_ARB, &bSuccess);
    if(!bSuccess)
    {
        LogError("Failed to link shader %s", szPath);
        LogGLErrors();
        LogGLInfoLog(m_hProgram);
        return false;
    }

    LogGLInfoLog(m_hProgram);
    return true;
}

void CShaderObject::Enable()
{
    glUseProgramObjectARB(m_hProgram);
}

void CShaderObject::Disable()
{
    glUseProgramObjectARB(NULL);
}

GLint CShaderObject::GetUniformParameterID(const char *pszParameter)
{
    std::map<std::string, GLint>::iterator it = m_mapParameters.find(pszParameter);
    if(it == m_mapParameters.end())
    {
        GLint nLoc = glGetUniformLocationARB(m_hProgram, pszParameter);
        it = m_mapParameters.insert(std::pair<std::string, GLint>(pszParameter, nLoc)).first;
    }
    return it->second;
}

void CShaderObject::SetUniformParameter1i(const char *pszParameter, int n1)
{
    glUniform1iARB(GetUniformParameterID(pszParameter), n1);
}

void CShaderObject::SetUniformParameter1f(const char *pszParameter, float p1)
{
    glUniform1fARB(GetUniformParameterID(pszParameter), p1);
}

void CShaderObject::SetUniformParameter3f(const char *pszParameter, float p1, float p2, float p3)
{
    glUniform3fARB(GetUniformParameterID(pszParameter), p1, p2, p3);
}

//==================================================================
// ShaderObject.cpp
//==================================================================

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <fstream>
#include <streambuf>
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


//==================================================================
static std::string makeStringFromFile( const std::string &fname )
{
    std::ifstream file( fname );

    if ( file.fail() )
    {
        std::runtime_error( std::string("Could not load file: ") + fname );
    }

    return { (std::istreambuf_iterator<char>(file)),
              std::istreambuf_iterator<char>() };
}

//==================================================================
bool CShaderObject::compileShader(
        const std::string &headerFName,
        const std::string &sourceFName,
        GLhandleARB handle )
{
    LogInfo( "Compiling GLSL shader %s", sourceFName.c_str() );

    // load the header, if any
    std::string header;
    if ( !headerFName.empty() )
        header = makeStringFromFile( headerFName );

    // load the source, and places a header as aprefix
    auto src = header + makeStringFromFile( sourceFName );

    const char *ppSrc[] = { src.c_str() };
    int nBytes = (int)src.size();
    int bSuccess = 0;
    glShaderSourceARB( handle, 1, ppSrc, &nBytes );
    glCompileShaderARB( handle );
    glGetObjectParameterivARB( handle, GL_OBJECT_COMPILE_STATUS_ARB, &bSuccess );

    if (!bSuccess)
    {
        LogError( "Failed to compile shader %s", sourceFName.c_str() );
        LogGLErrors();
        LogGLInfoLog( handle );
        return false;
    }

    return true;
}

//==================================================================
bool CShaderObject::LoadFromFile(
            const std::string &headerFName,
            const std::string &vertFName,
            const std::string &fragFName )
{
    if ( !compileShader( headerFName, vertFName, m_hVertexShader   ) )
        return false;

    if ( !compileShader( headerFName, fragFName, m_hFragmentShader ) )
        return false;

    //
    glAttachObjectARB(m_hProgram, m_hVertexShader);
    glAttachObjectARB(m_hProgram, m_hFragmentShader);
    glLinkProgramARB(m_hProgram);

    int bSuccess = 0;
    glGetObjectParameterivARB(m_hProgram, GL_OBJECT_LINK_STATUS_ARB, &bSuccess);
    if (!bSuccess)
    {
        LogError( "Failed to link shader %s", vertFName.c_str() );
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

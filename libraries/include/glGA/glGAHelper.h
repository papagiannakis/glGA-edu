/**
*
* \file glGAHelper.h
* A general helper global functions file.
* Contains function for loading/compiling shaders, manipulating file paths and Geometry Buffers.
*
*  Created by George Papagiannakis.
*  Copyright (c) 2012 UoC & FORTH. All rights reserved.
*/

#ifndef glGACharacterApp_glGAHelper_h
#define glGACharacterApp_glGAHelper_h

#include "glGA/PlatformWrapper.h"


#if defined _WIN32 || defined _WIN64
	#define _WINDOWS_TIME_
#endif


//BOOST 1.5.5
//#define USE_BOOST
#ifdef USE_BOOST
#include <boost/filesystem.hpp>
using namespace boost::filesystem;
#endif

// to use USE_SOIL2, uncomment the #define below
// https://bitbucket.org/SpartanJ/soil2
#define USE_SOIL2
#ifdef USE_SOIL2
#include <SOIL2/SOIL2.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#ifndef __APPLE__
# include <GL/glew.h>
#endif  // __APPLE__

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define ZERO_MEM(a) memset(a, 0, sizeof(a))

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define INVALID_OGL_VALUE 0xFFFFFFFF

#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

#define GLExitIfError()                                                          \
{                                                                               \
GLenum Error = glGetError();                                                \
\
if (Error != GL_NO_ERROR) {                                                 \
printf("OpenGL error in %s:%d: 0x%x\n", __FILE__, __LINE__, Error);     \
}                                                                           \
}

#define GLCheckError() (glGetError() == GL_NO_ERROR)

// --------------------- SHADER helper methods ------------------/
//dual method in C
static char* readShaderSource(const char* shaderFile);
GLuint InitShader(const char* vShaderFile, const char* fShaderFile);
// alternative single C++ method
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
// New version of the function LoadShaders. This function can add 3 extra type of shaders along with the vertex and fragment shaders. If the vertex and fragment shaders are the only shaders that will be used the rest of the arguments should be NULL.
GLuint AddShaders(const char* vertexShader_path, const char* fragmentShader_path, const char* geometryShader_path, const char* computeShader_path, const char* tess_ctrl_path, const char* tess_eval_path, GLuint *compProg);
//A function used to compile the given shader and link it to the given program.
GLuint ShaderCompileLink(const char* shader_path, GLenum type, GLuint program);

#ifdef USE_BOOST
//------------------------- PATH resolution methods -------------/
std::string resolvePath(std::string& inPathStr, std::string& originalPathStr);
bool        restorePath(std::string& originalPathStr);
#endif

// ------------ TEXTURE helper class (specified in .cpp)--------/
class Texture
{
public:
    Texture(GLenum textureTarget, const std::string& fileName);
    
    bool loadTexture();
    
    void bindTexture(GLenum textureUnit);
    
    std::string getTextureFileName(){return m_fileName;};
    
private:
    std::string     m_fileName;
    GLenum          m_textureTarget;
    GLuint          m_textureObj;
    int             width, height;
    unsigned        char* image;
};



#ifdef _WINDOWS_TIME_
#include < time.h >
#include <windows.h> //I've ommited this line.

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);
/*
 Usage
 Code Block
 gettimeofday(&now, NULL);
 gettimeofday(&now, &tzone);
 */
#endif //_WINDOWS_TIME_

#if defined __linux__ || defined __APPLE__
#include <sys/time.h>
#endif


#endif

#ifndef GBUFFER_H
#define	GBUFFER_H

#include <stdio.h>
#include <GL/glew.h>

#define GBUFFER_POSITION_TEXTURE_UNIT 0
#define GBUFFER_DIFFUSE_TEXTURE_UNIT  1
#define GBUFFER_NORMAL_TEXTURE_UNIT   2
#define GBUFFER_TEXCOORD_TEXTURE_UNIT 3

class GBuffer
{
public:
    
    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_TEXCOORD,
        GBUFFER_NUM_TEXTURES
    };
    
    GBuffer();
    
    ~GBuffer();
    
    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
    
    void BindForWriting();
    void BindForReading();
    void SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType);
    void StartFrame();
    void BindForGeomPass();
    void BindForStencilPass();
    void BindForLightPass();
    void BindForFinalPass();
    
private:
    
    GLuint m_fbo;
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
    GLuint m_depthTexture;
    GLuint m_finalTexture;
};

#endif /* glGAHelper_h */

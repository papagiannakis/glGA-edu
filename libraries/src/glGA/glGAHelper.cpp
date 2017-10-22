//
//  glGAHelper.cpp
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <SOIL2/SOIL2.h>
#include "glGA/glGAHelper.h"
#include "glGA/PlatformWrapper.h"

#if defined(__APPLE__) && !defined(__IOS__)
#include <OpenGL/gl3.h>
#endif



//*********************************** SHADER Helper functions ***********************/

// Create a NULL-terminated string by reading the provided file (in C)
static char* readShaderSource(const char* shaderFile)
{
	FILE* fp = fopen(shaderFile, "r");

	if ( fp == NULL ) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);

	buf[size] = '\0';
	fclose(fp);

	return buf;
}

// Create a GLSL program object from vertex and fragment shader files (in C)
GLuint InitShader(const char* vShaderFile, const char* fShaderFile)
{
	struct Shader {
		const char*  filename;
		GLenum       type;
		GLchar*      source;
	}  shaders[2] = {
		{ vShaderFile, GL_VERTEX_SHADER, NULL },
		{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
	};

	GLuint program = glCreateProgram();

	for ( int i = 0; i < 2; ++i ) {
		Shader& s = shaders[i];
		s.source = readShaderSource( s.filename );
		if ( shaders[i].source == NULL ) {
			std::cerr << "Failed to read " << s.filename << std::endl;
			exit( EXIT_FAILURE );
		}

		GLuint shader = glCreateShader( s.type );
		glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
		glCompileShader( shader );

		GLint  compiled;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled ) {
			std::cerr << s.filename << " failed to compile:" << std::endl;
			GLint  logSize;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
			char* logMsg = new char[logSize];
			glGetShaderInfoLog( shader, logSize, NULL, logMsg );
			std::cerr << logMsg << std::endl;
			delete [] logMsg;

			exit( EXIT_FAILURE );
		}

		delete [] s.source;

		glAttachShader( program, shader );
	}

	/* link  and error check */
	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv( program, GL_LINK_STATUS, &linked );
	if ( !linked ) {
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog( program, logSize, NULL, logMsg );
		std::cerr << logMsg << std::endl;
		delete [] logMsg;

		exit( EXIT_FAILURE );
	}

	/* use program object */
	//   glUseProgram(program);

	return program;
}

// Alternative single C++ method to Load/Compile/Link a pair for vertex and fragment shaders
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{


	string str_vertex_filepath = getGLSLResourcePath(vertex_file_path);
	//cout << str_vertex_filepath << endl;
	string str_fragment_filepath = getGLSLResourcePath(fragment_file_path);
	//cout << str_fragment_filepath << endl;


	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(str_vertex_filepath.c_str(), std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else
	{
		std::cout<<"shader: "<<str_vertex_filepath<<" not found!"<<std::endl;
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(str_fragment_filepath.c_str(), std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else
	{
		std::cout<<"shader: "<<str_fragment_filepath<<" not found!"<<std::endl;
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength=10;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n\n", str_vertex_filepath.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	if (!Result) 
	{
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}
	// Compile Fragment Shader
	printf("Compiling shader : %s\n\n", str_fragment_filepath.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	if (!Result) 
	{
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}
	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);


	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

// New version of the function LoadShaders. This function can add 3 extra type of shaders along with the vertex and fragment shaders. If the vertex and fragment shaders are the only shaders that will be used the rest of the arguments should be NULL.
GLuint AddShaders(const char* vertexShader_path, const char* fragmentShader_path, const char* geometryShader_path, const char* computeShader_path, const char* tess_ctrl_path, const char* tess_eval_path, GLuint *compProg){
    
    //Create the program.
    GLuint programID = glCreateProgram();
    
    //Check each shader path and if they are valid, the shader is compiled and the program linked.
    
    if (vertexShader_path != NULL){
        //Vertex shader
        programID = ShaderCompileLink(vertexShader_path, GL_VERTEX_SHADER, programID);
        
    }
    else{
        
        std::printf("Warning: A vertex shader could not be found.\n");
        
    }
    if (fragmentShader_path != NULL){
        //Fragment shader
        programID = ShaderCompileLink(fragmentShader_path, GL_FRAGMENT_SHADER, programID);
        
    }
    else{
        
        std::printf("Warning: A fragment shader could not be found.\n");
        
    }
    if (geometryShader_path != NULL){
        //Geometry shader
        programID = ShaderCompileLink(geometryShader_path, GL_GEOMETRY_SHADER, programID);
        
    }
    if (computeShader_path != NULL){
        //Compute shader
        *compProg = ShaderCompileLink(computeShader_path, GL_COMPUTE_SHADER, (GLuint)*compProg);
        
    }
    if (tess_ctrl_path != NULL){
        //Tessellation control shader
        programID = ShaderCompileLink(tess_ctrl_path, GL_TESS_CONTROL_SHADER, programID);
        
    }
    if (tess_eval_path != NULL){
        //Tessellation evaluation shader
        programID = ShaderCompileLink(tess_eval_path, GL_TESS_EVALUATION_SHADER, programID);
        
    }
    
    //Link the shaders to the program
    fprintf(stdout, "Linking the shaders to the program...\n");
    glLinkProgram(programID);
    
    GLint res = GL_FALSE;
    
    glGetProgramiv(programID, GL_LINK_STATUS, &res);
    if (GL_TRUE != res) {
        int max_length = 4096;
        int actual_length = 0;
        char log[4096];
        
        fprintf(stderr, "Error: The program %u failed to link.\n", programID);
        glGetProgramInfoLog(programID, max_length, &actual_length, log);
        fprintf(stderr, "program info log for GL index %u\n%s\n", programID, log);
    }
    else{
        std::cout << "Done!" << endl << endl;
    }
    
    
    
    return programID;
}

//A function used to compile the given shader and link it to the given program.
GLuint ShaderCompileLink(const char * shader_path, GLenum type, GLuint program){
    
    int max_length = 4096;
    int actual_length = 0;
    char log1[4096];
    char log2[4096];
    int error = 0;
    int tessellation = 0;
    
    string Shaderp = getGLSLResourcePath(shader_path);
    
    //Create the shader.
    GLuint ShaderID = glCreateShader(type);
    
    //Read the shader source code from the path.
    std::string buffer;
    std::ifstream shaderSrc(Shaderp.c_str(), std::ios::in);
    if (shaderSrc.is_open()){
        std::string Line = "";
        while (getline(shaderSrc, Line)){
            buffer += "\n" + Line;
        }
        shaderSrc.close();
    }
    else{
        std::printf("Error: The path %s was not found. \n", Shaderp.c_str());
    }
    if (type == GL_VERTEX_SHADER){
        std::printf("Compiling the vertex shader : %s ... \n", Shaderp.c_str());
    }
    else if (type == GL_FRAGMENT_SHADER){
        std::printf("Compiling the fragment shader : %s ... \n", Shaderp.c_str());
    }
    else if (type == GL_GEOMETRY_SHADER){
        std::printf("Compiling the geometry shader : %s ... \n", Shaderp.c_str());
    }
    else if (type == GL_COMPUTE_SHADER){
        std::printf("Compiling the compute shader : %s ... \n", Shaderp.c_str());
        program = glCreateProgram();
    }
    else if (type == GL_TESS_CONTROL_SHADER){
        std::printf("Compiling the tessellation control shader : %s ... \n", Shaderp.c_str());
    }
    else if (type == GL_TESS_EVALUATION_SHADER){
        std::printf("Compiling the tessellation evaluation shader : %s ... \n", Shaderp.c_str());
    }
    else{
        assert(0); //Error: Execution should never reach here.
    }
    
    //Compile shader
    char const * SourcePointer = buffer.c_str();
    glShaderSource(ShaderID, 1, &SourcePointer, NULL);
    glCompileShader(ShaderID);
    
    //Check for compilation errors
    GLint res = GL_FALSE;
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &res);
    if (GL_TRUE != res) {
        fprintf(stderr, "Error: The shader %s failed to compile.\n", Shaderp.c_str());
        glGetShaderInfoLog(ShaderID, max_length, &actual_length, log1);
        fprintf(stderr, "Shader info log for GL index %u\n%s\n", ShaderID, log1);
        error = 1;
    }
    else{
        std::cout << "Done!" << endl << endl;
    }
    
    if (error == 0){
        
        glAttachShader(program, ShaderID);
        
    }
    
    if (type == GL_COMPUTE_SHADER){
        //Link the compute shader to the program
        fprintf(stdout, "Linking the compute shader to the program...\n");
        glLinkProgram(program);
        
        res = GL_FALSE;
        
        glGetProgramiv(program, GL_LINK_STATUS, &res);
        if (GL_TRUE != res) {
            fprintf(stderr, "Error: The program %u failed to link.\n", program);
            glGetProgramInfoLog(program, max_length, &actual_length, log2);
            fprintf(stderr, "program info log for GL index %u\n%s\n", program, log2);
        }
        else{
            std::cout << "Done!" << endl << endl;
        }
        
    }
    
    glDeleteShader(ShaderID);
    
    return program;
    
}

#ifdef USE_BOOST
//*********************************** SHADER Helper functions ***********************/

/**
 * @par 
 * resolves the path of a filename, and changes current path to the one pointed by the
 * dedicated input path string, so that subsequent calls can assume the new current path.
 * - inPathStr: the input path
 * - outFileStr: the filename
 * -  outParentPathStr: the newly changed path that the outFileStr resides in
 */
std::string resolvePath(std::string& inPathStr, std::string& originalPathStr)
{
    path inPath(inPathStr);
    path originalPath;
    std::string outParentPathStr = "";
    std::string outFileStr("");
    
    if (exists(inPath))
    {
        if (is_regular_file(inPath))
        {
        //save current path
        originalPath = current_path();
        originalPathStr = originalPath.string();
        //chdir path to the parent path pointed by inPath
        current_path(inPath.parent_path());
        // alternative
        //chdir(inPath.parent_path().c_str());
        // verify that path was changed correctly!
        std::cout<<"changed current path: "<<current_path().string()<<", "<<inPath.parent_path().string()<<std::endl;
            
        outFileStr = inPath.filename().string();
        outParentPathStr = inPath.parent_path().string();

        std::cout<<"founded file: "<<outFileStr<<" with path: "<<outParentPathStr<<std::endl;
        }
    }
    
    return outFileStr;
}

bool restorePath(std::string& originalPathStr)
{
    path previousPath(originalPathStr);
    if (exists(previousPath))
    {
        current_path(previousPath);
        std::cout<<"changed to previous path: "<<current_path().string()<<std::endl;
        return true;
    }
    else
        return false;
}
#endif
//*********************************** TEXTURE Helper functions ***********************/


Texture::Texture(GLenum textureTarget, const std::string& fileName)
{
	m_textureTarget = textureTarget;
	m_fileName      = fileName;

	std::cout<< "Constructing texture is done with success! :"<<m_fileName<<std::endl;
}

bool Texture::loadTexture()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int channels = 4;
	image = SOIL_load_image(m_fileName.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
	if (image == NULL)
	{
		std::cout << "loading texture failed :" << m_fileName << std::endl;
	}
    
	//then if texture is loaded ok by USE_SOIL2
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);

	glTexImage2D(m_textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::cout << "loading texture is done with success! :" << m_fileName << std::endl;

	return true;
}

void Texture::bindTexture(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}




#ifdef _WINDOWS_TIME_
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
#endif


//*************************** THE GBUFFER CLASS ****************************//

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))


GBuffer::GBuffer()
{
    m_fbo = 0;
    m_depthTexture = 0;
    m_finalTexture = 0;
    ZERO_MEM(m_textures);
    
    printf("The creation of the GBuffer was successful!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

GBuffer::~GBuffer()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }
    
    if (m_textures[0] != 0) {
        glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
    }
    
    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
    }
    
    printf("The destruction of the GBuffer was successful!\n");
}


bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    
    printf("Starting initialization of the GBuffer...\n");
    
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    // Create the gbuffer textures
    glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
    glGenTextures(1, &m_depthTexture);
    glGenTextures(1, &m_finalTexture);
    
    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    }
    
    // depth
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, WindowWidth, WindowHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    
    glBindTexture(GL_TEXTURE_2D, m_finalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_finalTexture, 0);
    
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
    
    printf("The GBuffer was initialized successfully!\n");
    
    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    return true;
}

void GBuffer::StartFrame()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glClear(GL_COLOR_BUFFER_BIT);
}


void GBuffer::BindForGeomPass()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2 };
    
    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
}


void GBuffer::BindForStencilPass()
{
    // must disable the draw buffers
    glDrawBuffer(GL_NONE);
}



void GBuffer::BindForLightPass()
{
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    
    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
    }
}


void GBuffer::BindForFinalPass()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT4);
}

void GBuffer::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}


void GBuffer::BindForReading()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
    }
}

void GBuffer::SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}





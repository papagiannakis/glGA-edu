//
//  PlatformWrapper.h
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//

/**
 * A platformWrapper for the iOS platform.
 * Contains all necessary extensions for glGA to be available on the iOS platform.
 */

#ifdef TARGET_OS_IPHONE
    #define __IOS__
#else
    #define __WINDOWS__MAC__LINUX__
#endif

#include <string>
using namespace std;

#if defined __linux__
#define GLEW_NO_GLUE
#endif


#ifdef __WINDOWS__MAC__LINUX__ 

// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>


#define PLATFORM_glGenVertexArrays(x,y) glGenVertexArrays(x,y)
#define PLATFORM_glBindVertexArray(x) glBindVertexArray(x)


#elif defined(__IOS__)

//#include <OpenGLES/ES2/gl.h>
//#include <OpenGLES/ES2/glext.h>

#define PLATFORM_glGenVertexArrays(x,y) glGenVertexArraysOES(x,y)
#define PLATFORM_glBindVertexArray(x) glBindVertexArrayOES(x)

string getFilenameWithoutExtension(const char* filename);
string getExtension(const char* filename);
string getExtensionWithDot(const char* filename);
void getRidOfString(string& full_string, string part_string);
void replaceURLSpaces(string& str);
string getBundlePath(const char* filename);

#endif

/**
 * Returns the texture path as a string.
 * @param filename the file name as a char *
 * @ return the char * filename as an STL string
 */
string getTextureResourcePath(const char* filename);

/**
 * Returns the GLSL resource path as a string.
 * @param filename the file name as a char *
 * @ return the char * filename as an STL string
 */
string getGLSLResourcePath(const char* filename);

/**
 * Returns the 3D model path as a string.
 * @param filename the file name as a char *
 * @ return the char * filename as an STL string
 */
string getModelResourcePath(const char* filename);

//
//  PlatformWrapper.h
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//


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


string getTextureResourcePath(const char* filename);
string getGLSLResourcePath(const char* filename);
string getModelResourcePath(const char* filename);



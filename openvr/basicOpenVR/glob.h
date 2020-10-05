#pragma once
/*
	Simple Header
	description : main.cpp
*/

// Basic Includes
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cassert>
#include <vector>

// OpenGL extension Library
#include <GL/glew.h>

// Cross-platform window system
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// basic math and our own glGAHelper
#include "matrix.h"
#include "glGA/glGAHelper.h"

// Uncomment to add VR support
#define _VR

// To switch the box to a teapot, uncomment the following two lines
//#include "teapot.h"
//#define Shape Teapot

// Window variables defined before others because in case of _VR 
// we need the framebufferWidth and Height
uint32_t framebufferWidth = 1280, framebufferHeight = 720;
const int windowHeight = 720;
const int windowWidth = (framebufferWidth * windowHeight) / framebufferHeight;

#ifdef _VR
#include "minimalOpenVR.h"
vr::IVRSystem* hmd = initOpenVR(framebufferWidth, framebufferHeight);
vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
const int numEyes = 2;
#else
const int numEyes = 1;
#endif

#ifndef Shape
#   define Shape Cube
#endif



namespace Cube {
	const float position[][3] = { -.5f, .5f, -.5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, -.5f, .5f };
	const float normal[][3] = { 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f };
	const float tangent[][4] = { 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f };
	const float texCoord[][2] = { 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f };
	const int   index[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23 };
};

// Global Variables

// Window variables
SDL_Window		*window = NULL;
SDL_GLContext	context;


const int numIndices = sizeof(Shape::index) / sizeof(Shape::index[0]);

const float nearPlaneZ = -0.1f;
const float farPlaneZ = -100.0f;
const float verticalFieldOfView = 45.0f * PI / 180.0f;


// Camera & Mouse speed
const float cameraTurnSpeed = 0.0005f;
const float cameraMoveSpeed = 0.01f;
bool  camera = false;

// OpenGL variables
GLuint shader;
GLuint framebuffer[numEyes];
GLuint colorRenderTarget[numEyes], depthRenderTarget[numEyes];
GLuint positionBuffer	= GL_NONE;
GLuint texCoordBuffer	= GL_NONE;
GLuint normalBuffer		= GL_NONE;
GLuint tangentBuffer	= GL_NONE;
GLuint indexBuffer		= GL_NONE;
GLuint colorTexture		= GL_NONE;
GLuint trilinearSampler = GL_NONE;
GLuint uniformBlock;

GLint positionAttribute;
GLint normalAttribute;
GLint texCoordAttribute;
GLint tangentAttribute;
GLint colorTextureUniform;

const GLchar* uniformName[] = {
	"Uniform.objectToWorldNormalMatrix",
	"Uniform.objectToWorldMatrix",
	"Uniform.modelViewProjectionMatrix",
	"Uniform.light",
	"Uniform.cameraPosition" };


const int numBlockUniforms = sizeof(uniformName) / sizeof(uniformName[0]);
GLuint uniformBlockIndex;
GLuint uniformBindingPoint = 1;
GLuint uniformIndex[numBlockUniforms];
GLint  uniformOffset[numBlockUniforms];
GLint  colorTextureUnit;

// Math variables 
//TODO: Remove Matrix and use GLM instead :)
Vector3 bodyTranslation(0.0f, 1.6f, 5.0f);
Vector3 bodyRotation;
Matrix4x4 temp;
Texture* pTexture;



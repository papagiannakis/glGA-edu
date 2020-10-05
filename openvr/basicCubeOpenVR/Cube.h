#pragma once
/*
*	Marios Kanakis - 28.08.2016
*	Cube.h
*
*	See description at main.cpp.
*/


//basic Input-Output stream
#include <iostream>
#include <stdio.h>

// OpenGL Extension Wrangler Library
// Link : http://glew.sourceforge.net/
#include <GL/glew.h>

// GLM lib
// http://glm.g-truc.net/api/modules.html
#define GLM_SWIZZLE
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

// Simple DirectMedia Layer is a cross-platform development library
// designed to provide low level access to audio, keyboard, mouse, joystick, 
// and graphics hardware via OpenGL
// Link : https://www.libsdl.org/
#include <SDL2/SDL.h>
// SDL Wrapper for OpenGL
#include <SDL2/SDL_opengl.h>

// local
#include "glGA/glGAHelper.h"

// Uncomment this line to add VR support.
#define _VR

#ifdef _VR
#   include "minimalOpenVR.h"
#endif


class Cube
{
public:
	Cube();
	virtual ~Cube();

	bool init();
	bool initSDL();
	bool initGL();

	void initCube();
	void displayCube();

	void colorcube();
	void quad(int a, int b, int c, int d);
	
	bool eventHandler(SDL_Event* event);

	void RunMainLoop();

	void Shutdown();

	// Unnecessary , will be removed
	void printMat(const glm::mat4x4& tmp);

private: // VR support
#ifdef _VR
	vr::IVRSystem* hmd;
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	static const int numEyes = 2;
#else
	static const int numEyes = 1;
#endif

private: // Window 
	uint32_t		framebufferWidth;
	uint32_t		framebufferHeight;
	const int		windowHeight;
	const int		windowWidth;

	const float		nearPlaneZ;
	const float		farPlaneZ;
	const float		verticalFieldOfView;

private: // SDL
	SDL_Window		*window;
	SDL_GLContext	context;

private: // OpenGL 
	GLuint			program;
	GLuint			vao;
	GLuint			buffer;

	GLuint			framebuffer[numEyes];
	GLuint			colorRenderTarget[numEyes];
	GLuint			depthRenderTarget[numEyes];

	GLuint			MVPmat;
	GLuint			TranslateMat;

	Texture			*pTexture;

private: // Camera
	bool		camera;
	const float cameraTurnSpeed;
	const float cameraMoveSpeed;

private: // Modelling
#define						PI (3.1415927f)
	typedef   glm::vec4		color4;
	typedef   glm::vec4		point4;
	int						Index;
	static const int		NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
	point4					points[NumVertices];
	color4					colors[NumVertices];
	glm::vec3				normals[NumVertices];
	glm::vec2				tex_coords[NumVertices];
	glm::mat4				translate;

	glm::mat4				modelViewProjection;
	glm::mat4				temp;
	glm::vec3				bodyTranslation;
	glm::vec3				bodyRotation;
	

	// Vertices of a unit cube centered at origin, sides aligned with axes
	point4 vertices[8];

	// RGBA olors
	color4 vertex_colors[8] ;
};

//---------------------------------------------------------
// Purpose: Constructor
//---------------------------------------------------------
Cube::Cube()
	: framebufferWidth(1280)
	, framebufferHeight(720)
	, windowHeight(720)
	, windowWidth((framebufferWidth * windowHeight) / framebufferHeight)
	, nearPlaneZ(0.1f)
	, farPlaneZ(100.0f)
	, verticalFieldOfView(45.0f)
	, window(NULL)
	, context(NULL)
	, program(0)
	, vao(0)
	, buffer(0)
	, pTexture(NULL)
	, MVPmat(0)
	, TranslateMat(0)
	, Index(0)
	, camera(false)
	, cameraTurnSpeed(0.0005f)
	, cameraMoveSpeed(0.01f)
{
#ifdef _VR
	hmd = initOpenVR(framebufferWidth, framebufferHeight);
	assert(hmd);
	framebuffer[0] = 0;
	framebuffer[1] = 0;
	colorRenderTarget[0] = 0;
	colorRenderTarget[1] = 0;
	depthRenderTarget[0] = 0;
	depthRenderTarget[1] = 0;
#else
	framebuffer[0] = 0;
	colorRenderTarget[0] = 0;
	depthRenderTarget[0] = 0;
#endif

	translate			= glm::mat4x4();
	modelViewProjection = glm::mat4x4();
	temp				= glm::mat4x4();
	bodyRotation		= glm::vec3();
	bodyTranslation		= glm::vec3(0.0f, 1.6f, 5.0f);

	// Cube Vertices
	vertices[0] = point4(-0.5, -0.5,  0.5, 1.0);
	vertices[1] = point4(-0.5,  0.5,  0.5, 1.0);
	vertices[2] = point4( 0.5,  0.5,  0.5, 1.0);
	vertices[3] = point4( 0.5, -0.5,  0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5,  0.5, -0.5, 1.0);
	vertices[6] = point4( 0.5,  0.5, -0.5, 1.0);
	vertices[7] = point4( 0.5, -0.5, -0.5, 1.0);

	// Cube vertex colors
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(0.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan

};


//---------------------------------------------------------
// Purpose: Destructor
//---------------------------------------------------------
Cube::~Cube()
{
	// Shutdown function
	std::cout << "Shutdown" << std::endl;
}


//---------------------------------------------------------
// Purpose: Initialize everything!
//---------------------------------------------------------
bool Cube::init()
{
	bool success = true;

	if (initSDL() == false)
	{
		std::cout << "SDL failed to initialize!!!" << std::endl;
		success = false;
	}

	initCube();

	if (success)
		std::cout << "Everything initialized successfully!" << std::endl;

	return success;
}


//---------------------------------------------------------
// Purpose: Initialize SDL
//---------------------------------------------------------
bool Cube::initSDL()
{
	// Basic Setup
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	// Specify basic OpenGL attributes
	// OpenGL 4.1 version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Window flags
	const char*	title = "basicCubeOpenVR";
	Uint32 windowFlags	= SDL_WINDOW_OPENGL;
	Uint32 positionFlag = SDL_WINDOWPOS_CENTERED;

	// Create Window
	window = SDL_CreateWindow(title, positionFlag, positionFlag, windowWidth, windowHeight, windowFlags);
	if (window == NULL)
	{
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	std::cout << "Window created successfully!" << std::endl;

	// Create OpenGL context
	context = SDL_GL_CreateContext(window);
	if (context == NULL)
	{
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	std::cout << "OpenGL context created successfully!" << std::endl;

	if (initGL() == false)
	{
		return false;
	}

	// Use vSync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		std::cout << "Warning: Unable to set Vsync! SDL Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "vSync set succesfully!" << std::endl;

	return true;
}


//---------------------------------------------------------
// Purpose: Initialize GLEW
//---------------------------------------------------------
bool Cube::initGL()
{
	// Initialize Glew
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
		return false;
	}

	std::cout << "Glew initialized succesfully!" << std::endl;
	fprintf(stderr, "GPU: %s (OpenGL version %s)\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
	return true;
}


//---------------------------------------------------------
// Purpose: Initialize Cube (VAO, VBOS, FrameBuffers, etc.)
//---------------------------------------------------------
void Cube::initCube()
{
	//generate and bind a VAO for the 3D axes
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	pTexture = new Texture(GL_TEXTURE_2D, "texture0.jpg");
	if (!pTexture->loadTexture()) {
		return;
	}

	// Allocate the frame buffer
	glGenFramebuffers(numEyes, framebuffer);

	glGenTextures(numEyes, colorRenderTarget);
	glGenTextures(numEyes, depthRenderTarget);
	for (int eye = 0; eye < numEyes; ++eye) {
		glBindTexture(GL_TEXTURE_2D, colorRenderTarget[eye]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebufferWidth, framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, depthRenderTarget[eye]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, framebufferWidth, framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[eye]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorRenderTarget[eye], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRenderTarget[eye], 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	colorcube();

	//Load Shaders
	program = LoadShaders("vshaderCube.vert", "fshaderCube.frag");

	glUseProgram(program);

	// Create and initialize a buffer object on the server side (GPU)
	//GLuint buffer;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(tex_coords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(tex_coords), tex_coords);


	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	TranslateMat = glGetUniformLocation(program, "translate");
	glUniformMatrix4fv(TranslateMat, 1, GL_FALSE, glm::value_ptr(translate));

	MVPmat = glGetUniformLocation(program, "modelViewProjection");
	glUniformMatrix4fv(MVPmat, 1, GL_FALSE, glm::value_ptr(modelViewProjection));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);


	// only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	glBindVertexArray(0);
}


//---------------------------------------------------------
// Purpose: generate 12 triangles: 36 vertices and 36 
// colors
//---------------------------------------------------------
void Cube::colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}


//---------------------------------------------------------
// Purpose: quad generates two triangles for each face and 
// assigns colors to the vertices
//---------------------------------------------------------
void Cube::quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; tex_coords[Index] = glm::vec2(0, 0); Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; tex_coords[Index] = glm::vec2(0, 1); Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; tex_coords[Index] = glm::vec2(1, 1); Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; tex_coords[Index] = glm::vec2(0, 0); Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; tex_coords[Index] = glm::vec2(1, 1); Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; tex_coords[Index] = glm::vec2(1, 0); Index++;
}


//---------------------------------------------------------
// Purpose: Render our Cube
//---------------------------------------------------------
void Cube::displayCube()
{
	glm::mat4x4 eyeToHead[numEyes], projectionMatrix[numEyes], headToBodyMatrix;

#ifdef _VR
	getEyeTransformations(hmd, trackedDevicePose, nearPlaneZ, farPlaneZ, glm::value_ptr(headToBodyMatrix), glm::value_ptr(eyeToHead[0]), glm::value_ptr(eyeToHead[1]), glm::value_ptr(projectionMatrix[0]), glm::value_ptr(projectionMatrix[1]));
#else
	projectionMatrix[0] = glm::perspective(verticalFieldOfView, (float)framebufferWidth / (float)framebufferHeight, nearPlaneZ, farPlaneZ);
#endif // _VR


	const glm::mat4x4 bodyToWorldMatrix = glm::translate(glm::mat4(), bodyTranslation)*glm::yawPitchRoll(bodyRotation.y, bodyRotation.x, bodyRotation.z);


	const glm::mat4x4 headToWorldMatrix = bodyToWorldMatrix * headToBodyMatrix;
	temp = headToWorldMatrix;

	glUseProgram(program);
	glBindVertexArray(vao);

	for (int eye = 0; eye < numEyes; ++eye)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[eye]);
		glViewport(0, 0, framebufferWidth, framebufferHeight);

		glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		translate = glm::translate(glm::mat4(1.0), glm::vec3(-0.5, 0.0, 0.0));
		glUniformMatrix4fv(TranslateMat, 1, GL_FALSE, glm::value_ptr(translate));

		const glm::mat4x4 objectToWorld = glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.5f, 0.0f)) * glm::yawPitchRoll(PI / 3.0f, 0.0f, 0.0f);
		const glm::mat4x4 cameraToWorld = headToWorldMatrix * eyeToHead[eye];

		modelViewProjection = projectionMatrix[eye] * glm::inverse(cameraToWorld) * objectToWorld;
		glUniformMatrix4fv(MVPmat, 1, GL_FALSE, glm::value_ptr(modelViewProjection));

		pTexture->bindTexture(0);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);

#   ifdef _VR
		{
			const vr::Texture_t tex = { reinterpret_cast<void*>(intptr_t(colorRenderTarget[eye])), vr::API_OpenGL, vr::ColorSpace_Gamma };
			vr::VRCompositor()->Submit(vr::EVREye(eye), &tex);
		}
#   endif
	}

#   ifdef _VR
	// Tell the compositor to begin work immediately instead of waiting for the next WaitGetPoses() call
	vr::VRCompositor()->PostPresentHandoff();
#   endif


	// Mirror to the window
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);
	glBindVertexArray(0);
}


//---------------------------------------------------------
// Purpose: Handle Events
//---------------------------------------------------------
bool Cube::eventHandler(SDL_Event* event)
{
	if (bodyTranslation.y < 0.01f)
	{
		bodyTranslation.y = 0.01f;
	}
	switch (event->type)
	{
	case SDL_KEYDOWN:
	{
		//std::cout << "Buttons pressed\n";
		if (event->key.keysym.sym == SDLK_w)
		{
			bodyTranslation += glm::vec3(temp * glm::vec4(0, 0, -cameraMoveSpeed, 0));
			return true;
		}

		if (event->key.keysym.sym == SDLK_s)
		{
			bodyTranslation += glm::vec3(temp * glm::vec4(0, 0, +cameraMoveSpeed, 0));
			return true;
		}

		if (event->key.keysym.sym == SDLK_a)
		{
			bodyTranslation += glm::vec3(temp * glm::vec4(-cameraMoveSpeed, 0, 0, 0));
			return true;
		}

		if (event->key.keysym.sym == SDLK_d)
		{
			bodyTranslation += glm::vec3(temp * glm::vec4(+cameraMoveSpeed, 0, 0, 0));
			return true;
		}

		if (event->key.keysym.sym == SDLK_c)
		{
			bodyTranslation.y -= cameraMoveSpeed;
			return true;
		}

		if (event->key.keysym.sym == SDLK_z)
		{
			bodyTranslation.y += cameraMoveSpeed;
			return true;
		}

		if (event->key.keysym.sym == SDLK_SPACE)
		{
			bodyTranslation.y += cameraMoveSpeed;
			return true;
		}

		if (event->key.keysym.sym == SDLK_m)
		{
			if (!camera)
				camera = true;
			else
				camera = false;
			return true;
		}
		return true;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		return true;
	}
	case SDL_MOUSEBUTTONUP:
	{
		return true;
	}
	case SDL_MOUSEMOTION:
	{
		if (camera)
		{
			static double startX, startY;
			double currentX, currentY;

			currentX = event->motion.x;
			currentY = event->motion.y;

			bodyRotation.y -= float(currentX - startX) * cameraTurnSpeed;
			bodyRotation.x -= float(currentY - startY) * cameraTurnSpeed;


			startX = currentX;
			startY = currentY;
		}
	}
	}
	return true;
}


//---------------------------------------------------------
// Purpose: Run the main loop :p
//---------------------------------------------------------
void Cube::RunMainLoop()
{
	bool running = true;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			eventHandler(&event);

			if (event.type == SDL_QUIT)
				running = false;
		}

		displayCube();

		SDL_GL_SwapWindow(window);
	}

	return;
}


//---------------------------------------------------------
// Purpose: Shutdown everything
//---------------------------------------------------------
void Cube::Shutdown()
{
#ifdef _VR
	if (hmd)
	{
		vr::VR_Shutdown();
		hmd = NULL;
	}
#endif


	//Clean up
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


//---------------------------------------------------------
// Purpose: Print a 4x4 glm::matrix (Not transposed!)
//---------------------------------------------------------
void Cube::printMat(const glm::mat4x4& tmp)
{
	double dArray[16] = { 0.0 };
	const float *pSource = (const float*)glm::value_ptr(tmp);
	cout << "Matrix4x4(";
	for (int i = 0; i < 16; ++i)
	{
		dArray[i] = pSource[i];
		if (i != 15)
		{
			cout << dArray[i] << ", ";
		}
		else{
			cout << dArray[i];
		}


		if ((i + 1) % 4 == 0)
		{
			if (i != 15)
			{
				cout << endl;
				cout << "\t";
			}
		}
	}
	cout << ")" << endl;
}


// End of File
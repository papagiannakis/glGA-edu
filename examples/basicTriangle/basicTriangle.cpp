//
//  main.cpp
//  basicTriangle
//
//  Created by George Papagiannakis on 23/10/12.
//  Copyright (c) 2012 University Of Crete & FORTH. All rights reserved.
//

// basic STL streams
#include <iostream>

// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>

//Simple DirectMedia Layer is a cross-platform development library
//designed to provide low level access to audio, keyboard, mouse, joystick, 
//and graphics hardware via OpenGL
//Link : https://www.libsdl.org/
#include <SDL2/SDL.h>
//SDL Wrapper for OpenGL
#include <SDL2/SDL_opengl.h>

// GLM lib
// http://glm.g-truc.net/api/modules.html
#define GLM_SWIZZLE
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

//local
#include "glGA/glGAHelper.h"

// global variables
int         windowWidth = 1024, windowHeight = 768;
GLuint      programTriangle;
GLuint      vaoTriangle;
GLuint      bufferTriangle;

SDL_Window*		gWindow = NULL;
SDL_GLContext	gContext;


// Our function prototypes
bool	init();
void	close();
void    initTriangle();
void    displayTriangle();


//vertices for a 3D triangle
/*      1
*   *
*       *
0    *****    2

*/
glm::vec4   triangleVertices[3] =
{
	glm::vec4(0.0, 0.0, 0.0, 1.0),
	glm::vec4(0.5, 1.0, 0.0, 1.0),
	glm::vec4(1.0, 0.0, 0.0, 1.0)
};

bool init()
{
	//Init flag
	bool success = true;

	//Basic Setup
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		success = false;
	}
	else
	{
		std::cout << std::endl << "Yay! Initialized SDL succesfully!" << std::endl;
		//Use OpenGL Core 3.2
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		//SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		//Create Window
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

		gWindow = SDL_CreateWindow("basicTriangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
		if (gWindow == NULL)
		{
			std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
			success = false;
		}
		else
		{
			std::cout << std::endl << "Yay! Created window sucessfully!" << std::endl << std::endl;
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
				success = false;
			}
			else
			{
				//Initialize GLEW
				glewExperimental = GL_TRUE;
				GLenum glewError = glewInit();
				if (glewError != GLEW_OK)
				{
					std::cout << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
				}

				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					std::cout << "Warning: Unable to set Vsync! SDL Error: " << SDL_GetError << std::endl;
				}
			}
		}
	}
	return success;
}

void close()
{
	// Clean up
	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}

void initTriangle()
{
	//generate and bind a VAO for the 3D axes
	glGenVertexArrays(1, &vaoTriangle);
	glBindVertexArray(vaoTriangle);

	//load shaders
	programTriangle = LoadShaders("./vshaderSimple.vert", "./fshaderSimple.frag");

	glUseProgram(programTriangle);

	//create vertices VBO on server side GPU 
	glGenBuffers(1, &bufferTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, bufferTriangle);
	//fill-in VBO with vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	//connect vertex shader with application vertex data
	GLuint  vPosit = glGetAttribLocation(programTriangle, "vPosition");
	glEnableVertexAttribArray(vPosit);
	glVertexAttribPointer(vPosit, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	//stop using previous VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	glBindVertexArray(0);
}

void displayTriangle()
{
	glUseProgram(programTriangle);
	glBindVertexArray(vaoTriangle);

	glDisable(GL_CULL_FACE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glPopAttrib();
	glBindVertexArray(0);
}

int main(int, char**)
{
	// test a simple GLM vector
	glm::vec4   origin(0.0f, 0.0f, 0.0f, 1.0f);
	// test a simple GLM matrix
	glm::mat4   mat(1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	int running = GL_TRUE;

	if (!init()) {
		std::cout << "Error Initializing! Sorry " << std::endl;
		exit(EXIT_FAILURE);
	}

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// init Scene
	initTriangle();


	//GLFW main loop
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
            running = GL_FALSE;
            }
            
            if (event.type == SDL_QUIT)
			{
				running = GL_FALSE;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.3, 1.0); //black color

		// call function to render our scene
		displayTriangle();

		SDL_GL_SwapWindow(gWindow);
	}


	//close OpenGL window and terminate GLFW
	close();

	std::cout << "Hello, GLEW, SDL2, GLM Graphics World!\n";

	exit(EXIT_SUCCESS);

}




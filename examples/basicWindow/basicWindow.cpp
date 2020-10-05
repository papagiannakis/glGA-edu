//
//  main.cpp
//  basicWindow
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

// function prototypes

//Initializes SDL2 and GLEW
bool	init();

//Shuts down everything
void	close();



// global variables
int				windowWidth = 1024, windowHeight = 768;
SDL_Window*		gWindow = NULL;
SDL_GLContext	gContext;


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

		gWindow = SDL_CreateWindow("basicWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
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
                
                if (GLEW_VERSION_2_1)
                {
                    std::cout<<"\nYay! OpenGL 2.1 is supported and GLSL 1.2!\n"<<std::endl;
                }
                if (GLEW_VERSION_3_2)
                {
                    std::cout<<"Yay! OpenGL 3.2 is supported and GLSL 1.5!\n"<<std::endl;
                }
                
                /*
                 This extension defines an interface that allows various types of data
                 (especially vertex array data) to be cached in high-performance
                 graphics memory on the server, thereby increasing the rate of data
                 transfers.
                 Chunks of data are encapsulated within "buffer objects", which
                 conceptually are nothing more than arrays of bytes, just like any
                 chunk of memory.  An API is provided whereby applications can read
                 from or write to buffers, either via the GL itself (glBufferData,
                 glBufferSubData, glGetBufferSubData) or via a pointer to the memory.
                 */
                if (glewIsSupported("GL_ARB_vertex_buffer_object"))
                    std::cout<<"ARB VBO's are supported"<<std::endl;
                else if (glewIsSupported("GL_APPLE_vertex_buffer_object"))
                    std::cout<<"APPLE VBO's are supported"<<std::endl;
                else
                    std::cout<<"VBO's are not supported,program will not run!!!"<<std::endl;
                
                /*
                 This extension introduces named vertex array objects which encapsulate
                 vertex array state on the client side. The main purpose of these
                 objects is to keep pointers to static vertex data and provide a name
                 for different sets of static vertex data.
                 By extending vertex array range functionality this extension allows multiple
                 vertex array ranges to exist at one time, including their complete sets of
                 state, in manner analogous to texture objects.
                 GenVertexArraysAPPLE creates a list of n number of vertex array object
                 names.  After creating a name, BindVertexArrayAPPLE associates the name with
                 a vertex array object and selects this vertex array and its associated
                 state as current.  To get back to the default vertex array and its
                 associated state the client should bind to vertex array named 0.
                 */
                
                if (glewIsSupported("GL_ARB_vertex_array_object"))
                    std::cout<<"ARB VAO's are supported\n"<<std::endl;
                else if (glewIsSupported("GL_APPLE_vertex_array_object"))//this is the name of the extension for GL2.1 in MacOSX
                    std::cout<<"APPLE VAO's are supported\n"<<std::endl;
                else
                    std::cout<<"VAO's are not supported, program will not run!!!\n"<<std::endl;
                
                std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
                std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
                std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
                std::cout<<"Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;
                

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

int main(int, char**)
{
	// test a simple GLM vector
	/* glm::vec4   origin(0.0f, 0.0f,0.0f,1.0f);
	// test a simple GLM matrix
	glm::mat4   matrix( 1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0);*/


	if (!init()){
		std::cout << "Error Initializing! Sorry " << std::endl;
		exit(EXIT_FAILURE);
	}

	int running = GL_TRUE;



	// main loop
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
		glClearColor(0.0, 0.0, 0.5, 1.0);

		SDL_GL_SwapWindow(gWindow);
	}


	//close everything
	close();

	std::cout << "Hello, GLEW, SDL2, GLM World!\n";

	exit(EXIT_SUCCESS);

}


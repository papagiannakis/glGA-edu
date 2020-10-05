/*
*	Marios Kanakis, implementing:
*	http://casual-effects.blogspot.gr/2016/03/opengl-sample-codeand-openvr-sample-code.html
*	
*	Removed : glfw, shader / texture loading, Procedural sky
*	Added	: glGA, SDL2
*	Kept	: matrix.h , minimalOpenVR.h
*	
*	files:
*	main.cpp
*	matrix.h		: tiny vector and matrix header
*	minimalOpenVR.h : wrapper for OpenVR api
*	initOpenGL.h	: basic functions
*	teapot.h		: instead of the cube render a teapot
*	glob.h			: contains all includes and global variables
*/

#include "initOpenGL.h"


int main (int, char**)
{
	std::cout << " \nOpenGL 4.1 example based on Morgan's McGuire minimalOpenGL Example\n";
	std::cout << " Link provided above!\n\n";


	// Init Everything!
	window = init(windowWidth, windowHeight, "basicCube with OpenVR");

	// Defined in initOpenGL.h
	initBuffers();


	//int timer;
	int running = GL_TRUE;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// our custom event_handler . located : initOpenGL.h
			event_handler(&event);
			
			if (event.type == SDL_QUIT)
				running = GL_FALSE;
		}
		
		displayCube();

		SDL_GL_SwapWindow(window);
	}

	close();

	return 0;
}
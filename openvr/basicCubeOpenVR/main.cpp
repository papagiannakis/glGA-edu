/*
* Marios Kanakis - 28.08.2016
*
* main.cpp
* basicCubeOpenVR is featuring : glGA, OpenGL 4.1, glew, SDL2, glm, OpenVR
*	
* Based on the example of Morgan McGuire at:
*		http://casual-effects.blogspot.gr/2016/03/opengl-sample-codeand-openvr-sample-code.html
*
* and Valve's OpenVR API and hellovr_opengl example found at:
*		https://github.com/ValveSoftware/openvr , 
*		https://github.com/ValveSoftware/openvr/tree/master/samples/hellovr_opengl
*  
* For adding VR support see Cube.h
* Also you will need: Steam account, client and VR tools
* If you don't have an HMD you can use the NULL driver by adding the steamvr.vrsettings file found at OpenVR/basicCubeOpenVR/
* under your Steam installation folder : /Steam/config/ (e.g. D:/Games/Steam/config)
*
* files:
* Cube.h : Simple class for our Cube providing all necessary functions to render a textured cube.
* minimalOpenVR.h : Simple wrapper for the OpenVR API 
* main.cpp : 
*
* helpful links: 
* Download Steam: http://store.steampowered.com/ (Create an account first)
*/

#include "Cube.h"


int main (int, char**)
{
	std::cout << " OpenGL 4.1 example\n based on Morgan's McGuire Minimal OpenGL Example and\n"
				<< " hellovr_opengl Example provided by Valves OpenVR API.\n"
				<< " Links provided above!\n\n";

	Cube *cube = new Cube();

	// initialize our "cube"
	if (!cube->init())
	{
		cube->Shutdown();
		std::cout << "Init failed! Shuting down..." << std::endl;
		return 1;
	}

	// run the main loop
	cube->RunMainLoop();

	std::cout << "App exited normally." << std::endl;

	// clean up everything
	cube->Shutdown();

	return 0;
}
//
//  basicTesselation.cpp
//  basicTesselation
//
//	This is the basic example using the Tesselation shaders. 
//
//  Created by George Papagiannakis on 23/10/12.
//  Copyright (c) 2012 University Of Crete & FORTH. All rights reserved.
//

//basic Input-Output stream
#include <iostream>
#include <stdio.h>

//OpenGL Extension Wrangler Library
//Link : http://glew.sourceforge.net/
#include <GL/glew.h>

//Simple DirectMedia Layer is a cross-platform development library
//designed to provide low level access to audio, keyboard, mouse, joystick, 
//and graphics hardware via OpenGL
//Link : https://www.libsdl.org/
#include <SDL.h>
//SDL Wrapper for OpenGL
#include <SDL_opengl.h>
//SDL extession library for AUDIO support
#include <SDL_mixer.h>

//ImGui
//dear imgui (AKA ImGui), is a bloat-free graphical user interface library for C++. 
//It outputs vertex buffers that you can render in your 3D-pipeline enabled application. 
//It is fast, portable, renderer agnostic and self-contained (no external dependencies).
//Link : https://github.com/ocornut/imgui
#include <imgui.h>

//Implementation of ImGui based on SDL windowing system and OpenGL (shader based only). 
//Must be included at all times, else you have to write your own wrapper which can be difficult. 
//Just a tiny bit modified , most of it is found on the link below. 
//Link : https://github.com/ocornut/imgui/tree/master/examples/sdl_opengl3_example
#include <imgui_impl_sdl_glew.h>

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
#include "glGA/glGAMesh.h"

// Our function prototypes
// Starts up SDL , creates window and initializes OpenGL
bool			init();

// Initializes ImGui
bool			initImGui();

// Display ImGUI basic toolbar
void			displayGui();


// Event Handler
bool			event_handler(SDL_Event* event);

// Frees resources
void			close();

// Initialize and display tesselation
void			initTessexample();
void			displayTessexample();


//Global Variables
const int		SCREEN_WIDTH = 1024;
const int		SCREEN_HEIGHT = 780;
SDL_Window		*gWindow = NULL;
SDL_GLContext	gContext;


//Simple Color for clearing - ImGui
ImVec4 clear_color = ImColor(0, 0, 128);
bool        wireFrame = false;
// flag for checkbox - ImGui
static bool checkbox = false;

GLuint		shaderProgram;
GLuint		tvao;
GLuint		tvbo;


// Functions implementation

bool	init()
{
	//Init flag
	bool success = true;

	//Basic Setup
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
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

		gWindow = SDL_CreateWindow("Tesselation shaders example!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
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

				//Initializes ImGui
				if (!initImGui())
				{
					std::cout << "Error initializing ImGui! " << std::endl;
					success = false;
				}

			}
		}
	}
	return success;
}

void	close()
{
	// Cleanup
	ImGui_Impl_Shutdown();
	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	Mix_Quit();
	SDL_Quit();
}

bool	event_handler(SDL_Event* event)
{
	switch (event->type)
	{
		case SDL_MOUSEWHEEL:
		{
			return true;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			if (event->button.button == SDL_BUTTON_LEFT) 
			if (event->button.button == SDL_BUTTON_RIGHT)
			if (event->button.button == SDL_BUTTON_MIDDLE) 
			return true;
		}
		case SDL_TEXTINPUT:
		{
			return true;
		}
		case SDL_KEYDOWN:
		{
			if (event->key.keysym.sym == SDLK_w)
			{
				if (wireFrame)
				{
					wireFrame = false;
				}
				else
				{
					wireFrame = true;
				}
			}
			return true;
			}
		case SDL_KEYUP:
		{
			return true;
		}
	}
	return false;
}


void	initTessexample(){

    //test_tcs.glsl Tessellation Control Shader
    // test_tes.glsl Tesselation Evaluation Shader
    shaderProgram = AddShaders("test_vs.glsl", "test_fs.glsl", NULL, NULL, "test_tcs.glsl", "test_tes.glsl", NULL);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.2, 0.2, 0.2, 1.0);

	/* i've made the mesh 2 triangles, to help illustrate the numbers used for
	patch size versus points to draw */
	GLfloat points[] = {
		0.0f, 0.75f, 0.0f,
		0.5f, 0.25f, 0.0f,
		-0.5f, 0.25f, 0.0f,
		0.5f, -0.25f, 0.0f,
		0.0f, -0.75f, 0.0f,
		-0.5f, -0.25f, 0.0f
	};


	glGenBuffers(1, &tvbo);
	glBindBuffer(GL_ARRAY_BUFFER, tvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &tvao);
	glBindVertexArray(tvao);
	glBindBuffer(GL_ARRAY_BUFFER, tvbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);


	

}

void	displayTessexample(){

	glUseProgram(shaderProgram);
	
	glBindVertexArray(tvao);
	// set patch parameters - I'm drawing a mesh comprising 2 triangles, so '6'
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CW); // GL_CCW for counter clock-wise
	// NB. front or back alone didn't work on OSX -- had to use F&B here
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_PATCHES, 0, 6);
	// update other events like input handling 

}


bool	initImGui()
{	
	// Setup ImGui binding
	if (!ImGui_Impl_Init(gWindow))
	{
		return false;
	}
	
	// Load Fonts
	// (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
	
	// Marios -> in order to use custom Fonts, 
	//there is a file named extra_fonts inside /_thirdPartyLibs/include/ImGUI/extra_fonts
	//Uncomment the next line -> ImGui::GetIO() and one of the others -> io.Fonts->AddFontFromFileTTF("", 15.0f). 
	//Important : Make sure to check the first parameter is the correct file path of the .ttf or you get an assertion.
	
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	
	return true;
}

void	displayGui()
{
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"

	//Sets the Window size
	//ImGui::SetNextWindowSize(ImVec2(400, 160), ImGuiSetCond_FirstUseEver);
	//ImGui::SetNextWindowPos(ImVec2(10, 0));

	ImGui::Begin("basicTesselation GUI");
	static float f = 0.0f;

	// shows simple text
	ImGui::Text("Hello, world!");

	// goes to a newline
	ImGui::NewLine();

	// creates a simple slider
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

	// puts a line between widgets
	ImGui::Separator();

	// manipulates colors
	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	ImGui::Separator();


	// creates a checkbox
	if (ImGui::Checkbox("Wireframe", &checkbox))
	{
		if (checkbox == false)
		{
			wireFrame = false;
		}

		if (checkbox == true)
		{
			wireFrame = true;
		}
	}

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}


int main(int, char**)
{   
	int running = true;

	if (init() == false)
	{
		std::cout << "Init failed !!!" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	initTessexample();

	// Main loop
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_Impl_ProcessEvent(&event);

			//our custom event_handler
			event_handler(&event);

			if (event.type == SDL_QUIT)
				running = false;

			if (event.key.keysym.sym == SDLK_ESCAPE)
				running = false;
		}
		
		ImGui_Impl_NewFrame(gWindow);

		// Rendering
		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		
		displayGui();
		displayTessexample();

		//However displayTessexample is rendered , we want ImGui to be filled
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Most needed ImGui function 
		//Must be called just before SDL_GL_SwapWindow
		ImGui::Render();
		SDL_GL_SwapWindow(gWindow);
	}

	close(); //Shuts down every little thing...
	return 0;
}

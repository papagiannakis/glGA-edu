//
//  main.cpp
//  basicCube
//
//  Created by George Papagiannakis on 23/10/12.
//  Copyright (c) 2012 University Of Crete & FORTH. All rights reserved.
//

//Implementing basicGUI with SDL2 + ImGUI + OpenGL 

//Credits for the Music: http://www.bensound.com/royalty-free-music

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
#include <SDL2/SDL.h>
//SDL Wrapper for OpenGL
#include <SDL2/SDL_opengl.h>
//SDL extession library for AUDIO support
#include <SDL2/SDL_mixer.h>

//ImGui
//dear imgui (AKA ImGui), is a bloat-free graphical user interface library for C++. 
//It outputs vertex buffers that you can render in your 3D-pipeline enabled application. 
//It is fast, portable, renderer agnostic and self-contained (no external dependencies).
//Link : https://github.com/ocornut/imgui
#include <ImGUI/imgui.h>

//Implementation of ImGui based on SDL windowing system and OpenGL (shader based only). 
//Must be included at all times, else you have to write your own wrapper which can be difficult. 
//Just a tiny bit modified , most of it is found on the link below. 
//Link : https://github.com/ocornut/imgui/tree/master/examples/sdl_opengl3_example
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>

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

// Our function prototypes
//Starts up SDL , creates window and initializes OpenGL
bool			init();

//Initializes rendering program and clear color
bool			initGL();

//Initializes ImGui
bool			initImGui();

//Display ImGUI basic toolbar
void			displayGui();

//Loads media mp3 , images e.t.c
bool			load_media();

//Event Handler
bool			event_handler(SDL_Event* event);

//Frees media and shuts down SDL2
void			close();

void            initCube();
void            displayCube();
void            colorcube();
void            quad(int a, int b, int c, int d);


//Global Variables
const int			SCREEN_WIDTH = 1024;
const int			SCREEN_HEIGHT = 780;
SDL_Window			*gWindow = NULL;
SDL_GLContext		gContext;

//The music that will be played
Mix_Music *gMusic = NULL;

//Simple Color for clearing - ImGui
ImVec4 clear_color = ImColor(0, 0, 128);
// flag for checkbox - ImGui
static bool checkbox = false;

GLuint      program;
GLuint      vao;
GLuint      buffer;
bool        wireFrame = false;
typedef     glm::vec4   color4;
typedef     glm::vec4   point4;
int                     Index = 0;
const       int         NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
// Modelling arrays
point4      points[NumVertices];
color4      colors[NumVertices];
glm::vec3   normals[NumVertices];
glm::mat4   translate(1.0);
GLuint      TranslateMat;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

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
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

#ifdef __APPLE__
		SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
#endif
		//Initialize SDL_mixer
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}

		success = load_media();

		//Create Window
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

#ifdef __APPLE__
		gWindow = SDL_CreateWindow("ImGui + SDL2 + OpenGL4 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI );
#else
		gWindow = SDL_CreateWindow("ImGui + SDL2 + OpenGL4 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
#endif
		
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
					std::cout << "Warning: Unable to set Vsync! SDL Error: " << SDL_GetError() << std::endl;
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
	//Free the music
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	Mix_Quit();
	SDL_Quit();
}

bool	load_media()
{
	bool success = true;
	//Load music
	gMusic = Mix_LoadMUS("bensound-cute.mp3");
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	return success;
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
		if (event->key.keysym.sym == SDLK_9)
		{
			//If there is no music playing
			if (Mix_PlayingMusic() == 0)
			{
				//Play the music
				Mix_PlayMusic(gMusic, -1);
			}
			//If music is being played
			else
			{
				//If the music is paused
				if (Mix_PausedMusic() == 1)
				{
					//Resume the music
					Mix_ResumeMusic();
				}
				//If the music is playing
				else
				{
					//Pause the music
					Mix_PauseMusic();
				}
			}
			break;
		}
		if (event->key.keysym.sym == SDLK_0)
		{
			//Stop the music
			Mix_HaltMusic();
			break;
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

// quad generates two triangles for each face and assigns colors
//    to the vertices
void	quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void	colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void	initCube()
{
	//generate and bind a VAO for the 3D axes
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	colorcube();

	//Load Shaders
	program = LoadShaders("vshaderCube.vert", "fshaderCube.frag");

	glUseProgram(program);

	// Create and initialize a buffer object on the server side (GPU)
	//GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	TranslateMat = glGetUniformLocation(program, "translate");
	glUniformMatrix4fv(TranslateMat, 1, GL_FALSE, glm::value_ptr(translate));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);


	// only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	glBindVertexArray(0);
}

void	displayCube()
{
	glUseProgram(program);
	glBindVertexArray(vao);

	glDisable(GL_CULL_FACE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	translate = glm::translate(glm::mat4(1.0), glm::vec3(-0.5, 0.0, 0.0));
	glUniformMatrix4fv(TranslateMat, 1, GL_FALSE, glm::value_ptr(translate));


	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glPopAttrib();
	glBindVertexArray(0);
}

bool	initImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::SetCurrentContext(ImGui::CreateContext());

	// Setup ImGui binding

	if (!ImGui_ImplOpenGL3_Init("#version 150"))
	{
		return false;
	}
	if (!ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext) )
	{
		return false;
	}

	//ImGui::StyleColorsDark();

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
	ImGui::SetNextWindowSize(ImVec2(400, 160), ImGuiSetCond_FirstUseEver);
	//ImGui::SetNextWindowPos(ImVec2(10, 0));
	ImGui::Begin("basicCube GUI");
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

	//Second Window
	ImGui::SetNextWindowSize(ImVec2(240, 60), ImGuiSetCond_FirstUseEver);
	//ImGui::SetNextWindowPos(ImVec2(10, 200));
	ImGui::Begin("Music");
	ImGui::Text("Music: ");
	ImGui::SameLine(0.0f, -1.0f);
	if (ImGui::Button("Play/Pause"))
	{
		//If there is no music playing
		if (Mix_PlayingMusic() == 0)
		{
			//Play the music
			Mix_PlayMusic(gMusic, -1);
		}
		else if (Mix_PausedMusic() == 1)
		{
			//Resume Music
			Mix_ResumeMusic();
		}
		else
		{
			//Pause Music
			Mix_PauseMusic();
		}
	}
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button("Stop"))
	{
		Mix_HaltMusic();
	}
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

	initCube();

	// Main loop
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			//our custom event_handler
			event_handler(&event);
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			{
				running = GL_FALSE;
			}
			if (event.type == SDL_QUIT)
				running = false;

		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(gWindow);
		ImGui::NewFrame();

		// Rendering
		//glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		displayCube();

		//However displayCube is rendered , we want ImGui to be filled
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		displayGui();

		//Most needed ImGui function  
		//Must be called just before SDL_GL_SwapWindow
		ImGui::Render();
		SDL_GL_MakeCurrent(gWindow, gContext);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		SDL_GL_SwapWindow(gWindow);
	}

	close(); //Shuts down every little thing...
	return 0;
}
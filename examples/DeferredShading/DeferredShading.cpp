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
#include "glGA/glGAMesh.h"

#define INVALID_UNIFORM_LOCATION 0xffffffff

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
void			initGeoexample();
void			displayGeoexample();
void			initCompexample();
void			displayCompexample();
void			createTexture();
void			initTessexample();
void			displayTessexample();
void			initdshading1();
void			displaydshading1();
void			initdshading2();
void			displaydshading2();
void			initlprogram();
void			displaylprogram();
void			initquadprogram();
void			displayquadprogram();

void			initdshading3();
void			displaydshading3();
void			initlprogram3();
void			displaylprogram3();
void			initquadprogram3();
void			displayquadprogram3();
void			initStencil();
void			displayStencil();



#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 780

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

GLuint		LightProgram;
GLuint		shaderProgram;
GLuint		StencilProgram;
GLuint		rayProgram;
GLuint		gvbo;
GLuint		gvao;
GLuint		cvao;
GLuint		cvbo;
GLuint		tvao;
GLuint		tvbo;
GLuint		plvao;
GLuint		plvbo[4];
GLuint		LightBuffer;
GLuint		qvao;
GLuint		qbuffer[4];
GLuint		stvao;
GLuint		stvbo[4];

glm::vec4 Amb = glm::vec4(0.2, 0.2, 0.2, 1.0); /*Ambient*/
glm::vec4 Dif = glm::vec4(0.5, 0.5, 0.7, 1.0); /*Diffuse*/
glm::vec4 Spec = glm::vec4(0.3, 0.6, 0.1, 1.0); /*Specular*/

float ret;
float DiffuseIntensity = 0.2f;
float AmbientIntensity = 0.1f;
float SpecularIntensity = 0.4f;
float AmbDir = 0.1f;
float DifDir = 0.5f;
float mx = 2;
float my = 3;
float mz = 4;
float px = 3;
float py = 4;
glm::vec3 ColDir = glm::vec3(0, 1, 1);
glm::mat4 projection_matrix = glm::perspective(45.0f, (float)(1024 / 780), 0.1f, 100.0f); //Declaration of the projection matrix.
glm::vec3 pointlight_pos = glm::vec3(2, 0, 5);

GLuint modelviewst, projection_st, Colorq, LightDir, projection_lq, LightPosition, LightModel, LightView, LightProjection, model, modelview, modelviewl, projection_l, projection_s, samplerTexture, Color, Position, AttenConstant, AttenLinear, AttenExp, Ambient, Diffuse, Specular, screenSize, LightPos, Shininess, pointlColor, sampler1, sampler2, sampler3, Ambientq, Diffuseq, Specularq, screenSizeq, LightPosq, Shininessq, pointlColorq, sampler1q, sampler2q, sampler3q;

int running = true;
/*
 glm::mat4 view_mat = glm::lookAt(
	glm::vec3(0, 0, 20),
	glm::vec3(0, 5, 0),
	glm::vec3(0, 1, 0)
	);
	*/

glm::mat4 view_mat = glm::lookAt(
                                 glm::vec3(3, 4, 4),
                                 glm::vec3(2, 3, 0),
                                 glm::vec3(0, 1, 0)
                                 );


glm::mat4 model_mat[7];
glm::vec4 Light_Position = glm::vec4(0, 10, 30, 1);
float Shine = 100.0f;

glm::mat4 model_m[7];

Mesh* Mesh1;
Mesh* Mesh2;
Mesh* Mesh3;
Mesh* Mesh4;

GBuffer *gbuffer;
float	scale = 0.05f;
// dimensions of the image
int tex_w = 512, tex_h = 512;
GLuint tex_output;

int work_grp_cnt[3];
int work_grp_size[3];
GLint work_grp_inv;

GLuint		quad_program;
GLuint      program;
GLuint      vao;
GLuint      buffer[4];
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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

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
        
        gWindow = SDL_CreateWindow("ImGui + SDL2 + OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
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
            mz += event->wheel.y * 0.1;
            return true;
        }
        case SDL_MOUSEMOTION:
        {
            
            mx = (event->motion.x - WINDOW_WIDTH / 2) * 0.01;
            
            
            my = -(event->motion.y - WINDOW_HEIGHT / 2) * 0.01;
            
            //	printf("Coordinates : (%f,%f,%f)\n", mx, my,mz);
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
            if (event->key.keysym.sym == SDLK_UP){
                py = py + 0.5;
            }
            if (event->key.keysym.sym == SDLK_DOWN){
                py = py - 0.5;
            }
            if (event->key.keysym.sym == SDLK_LEFT){
                px = px - 0.5;
            }
            if (event->key.keysym.sym == SDLK_RIGHT){
                px = px + 0.5;
            }
            if (event->key.keysym.sym == SDLK_w){
                mz = mz + 0.5;
            }
            if (event->key.keysym.sym == SDLK_s){
                mz = mz - 0.5;
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
            if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE)
            {
                running = GL_FALSE;
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
/*
 void	initCube()
 {
	//generate and bind a VAO for the 3D axes
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
 
	colorcube();
 
	//Load Shaders
	//program = LoadShaders("vshaderCube.vert", "fshaderCube.frag");
 
	program = AddShaders("vshaderCube.vert", "fshaderCube.frag", NULL, NULL, NULL, NULL, NULL);
 
 
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
 */
void initGeoexample(){
    
    shaderProgram = AddShaders("vshadergeo.vert", "fshadergeo.frag", "gshader.geom", NULL, NULL, NULL, NULL);
    
    glGenBuffers(1, &gvbo);
    
    float points[] = {
        //  Coordinates  Color             Sides
        -0.45f, 0.45f, 1.0f, 0.0f, 0.0f, 4.0f,
        0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 8.0f,
        0.45f, -0.45f, 0.0f, 0.0f, 1.0f, 16.0f,
        -0.45f, -0.45f, 1.0f, 1.0f, 0.0f, 32.0f
    };
    
    
    glBindBuffer(GL_ARRAY_BUFFER, gvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    
    // Create VAO
    glGenVertexArrays(1, &gvao);
    glBindVertexArray(gvao);
    
    
    // Specify layout of point data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "pos");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), 0);
    
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)(2 * sizeof(float)));
    
    GLint sidesAttrib = glGetAttribLocation(shaderProgram, "sides");
    glEnableVertexAttribArray(sidesAttrib);
    glVertexAttribPointer(sidesAttrib, 1, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)(5 * sizeof(float)));
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
    glBindVertexArray(0);
    
}

void displayGeoexample(){
    
    glUseProgram(shaderProgram);
    glBindVertexArray(gvao);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDrawArrays(GL_POINTS, 0, 4);
    
}

void createTexture(){
    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT,
                 NULL);
    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
    
    printf("max global (total) work group size x:%i y:%i z:%i\n",
           work_grp_size[0], work_grp_size[1], work_grp_size[2]);
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
    
    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
           work_grp_size[0], work_grp_size[1], work_grp_size[2]);
    
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max local work group invocations %i\n", work_grp_inv);
}

void initCompexample(){
    
    shaderProgram = AddShaders("vshadercomp.vert", "fshadercomp.frag", NULL, "Comshader.comp", NULL, NULL, &rayProgram);
    
    float verts[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), verts, GL_STATIC_DRAW);
    glGenVertexArrays(1, &cvao);
    glBindVertexArray(cvao);
    glEnableVertexAttribArray(0);
    GLintptr stride = 4 * sizeof(float);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);
    glEnableVertexAttribArray(1);
    GLintptr offset = 2 * sizeof(float);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
    
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
    glBindVertexArray(0);
    
}

void displayCompexample(){
    
    { // launch compute shaders!
        glUseProgram(rayProgram);
        glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
    }
    
    // make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    { // normal drawing pass
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(cvao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_output);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
}

void initTessexample(){
    
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

void displayTessexample(){
    
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

void initdshading1(){
    
    //gbuffer = new GBuffer();
    
    //	gbuffer->Init(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    gbuffer = new GBuffer();
    
    gbuffer->Init(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    
    Mesh1 = new Mesh();
    Mesh1->loadMesh("../../../../_glGA-data/data/models/astroboy_walk.dae");
    
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glUseProgram(program);
    
    program = AddShaders("meshStatic.vert", "fshaderMesh.frag", NULL, NULL, NULL, NULL, NULL);
    
    GLuint position = glGetAttribLocation(program, "vPosition");
    GLuint normal = glGetAttribLocation(program, "vNormal");
    GLuint texture = glGetAttribLocation(program, "vTexCoord");
    
    
    
    int sizeTexture = sizeof(Mesh1->TexCoords[0])*Mesh1->TexCoords.size();
    int sizePositions = sizeof(Mesh1->Positions[0])*Mesh1->Positions.size();
    int sizeNormals = sizeof(Mesh1->Normals[0])*Mesh1->Normals.size();
    int sizeIndices = sizeof(Mesh1->Indices[0])*Mesh1->Indices.size();
    
    /*Generating the VBO array.*/
    glGenBuffers(4, buffer);
    /*Positions of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizePositions, &(Mesh1->Positions[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Texture coordinates of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeTexture, &(Mesh1->TexCoords[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(texture);
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Normals of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeNormals, &(Mesh1->Normals[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Indices for the vertices. (VBO)*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &(Mesh1->Indices[0]), GL_STATIC_DRAW);
    
    samplerTexture = glGetUniformLocation(program, "mesh_sampler"); //Binding the samplerTexture uniform to the mesh_sampler uniform of the fragment shader.
    glUniform1i(samplerTexture, 0);	//Passing the samplerTexture to the fragment shader.
    model = glGetUniformLocation(program, "Model");
    modelview = glGetUniformLocation(program, "ModelView"); //Binding the modelview uniform of this program to the ModelView uniform of the vertex shader.
    projection_s = glGetUniformLocation(program, "Projection"); //Binding the projection uniform of this program to the Projection uniform of the vertex shader.
    //LightPos = glGetUniformLocation(program, "LightPosition"); //Binding the light position uniform of this program to the LightPos uniform of the vertex shader.
    //Ambient = glGetUniformLocation(program, "AmbientProduct"); //Binding the ambient product uniform of this program to the AmbientProduct uniform of the vertex shader.
    //Diffuse = glGetUniformLocation(program, "DiffuseProduct"); //Binding the diffuse product uniform of this program to the DiffuseProduct uniform of the vertex shader.
    //Specular = glGetUniformLocation(program, "SpecularProduct"); //Binding the specular product uniform of this program to the SpecularProduct uniform of the vertex shader.
    //Shininess = glGetUniformLocation(program, "Shininess"); //Binding the shininess uniform of this program to the Shininess uniform of the vertex shader.
    
    
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    model_m[0] = glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    
}

void displaydshading1(){
    
    GLint HalfWidth = (GLint)(WINDOW_WIDTH / 2.0f);
    GLint HalfHeight = (GLint)(WINDOW_HEIGHT / 2.0f);
    
    gbuffer->GBuffer::BindForWriting();
    
    glUseProgram(program);
    glBindVertexArray(vao);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    
    
    glm::mat4 model_mat = view_mat * model_m[0];
    
    
    scale += 0.1f;
    model_m[0] = glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    
    
    glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(model_m[0]));
    
    glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(model_mat)); //Passing the value of the modelview uniform of this program to the ModelView uniform of the vertex shader.
    glUniformMatrix4fv(projection_s, 1, GL_FALSE, glm::value_ptr(projection_matrix)); //Passing the value of the projection uniform of this program to the Projection uniform of the vertex shader.
    glUniform1i(samplerTexture, 0);
    
    Mesh1->render();
    
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gbuffer->GBuffer::BindForReading();
    
    gbuffer->GBuffer::SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    gbuffer->GBuffer::SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, HalfHeight, HalfWidth, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    gbuffer->GBuffer::SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, HalfWidth, HalfHeight, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    gbuffer->GBuffer::SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, HalfWidth, 0, WINDOW_WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
    
    
    
}

void initdshading2(){
    
    Mesh1 = new Mesh();
    Mesh1->loadMesh("box.obj");
    
    
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glUseProgram(program);
    
    program = AddShaders("meshStatic2.vert", "fshaderMesh2.frag", NULL, NULL, NULL, NULL, NULL);
    
    
    GLuint position = glGetAttribLocation(program, "vPosition");
    GLuint normal = glGetAttribLocation(program, "vNormal");
    GLuint texture = glGetAttribLocation(program, "vTexCoord");
    
    
    
    int sizeTexture = sizeof(Mesh1->TexCoords[0])*Mesh1->TexCoords.size();
    int sizePositions = sizeof(Mesh1->Positions[0])*Mesh1->Positions.size();
    int sizeNormals = sizeof(Mesh1->Normals[0])*Mesh1->Normals.size();
    int sizeIndices = sizeof(Mesh1->Indices[0])*Mesh1->Indices.size();
    
    /*Generating the VBO array.*/
    glGenBuffers(4, buffer);
    /*Positions of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizePositions, &(Mesh1->Positions[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Texture coordinates of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeTexture, &(Mesh1->TexCoords[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(texture);
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Normals of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeNormals, &(Mesh1->Normals[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Indices for the vertices. (VBO)*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &(Mesh1->Indices[0]), GL_STATIC_DRAW);
    
    samplerTexture = glGetUniformLocation(program, "gColorMap");
    glUniform1i(samplerTexture, 0);
    model = glGetUniformLocation(program, "Model");
    modelview = glGetUniformLocation(program, "ModelView");
    projection_s = glGetUniformLocation(program, "Projection");
    
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    
}


void displaydshading2(){
    
    glUseProgram(program);
    
    gbuffer->GBuffer::BindForWriting();
    
    glBindVertexArray(vao);
    
    
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    
    model_m[0] = glm::translate(glm::mat4(), glm::vec3(0, 0, -5)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    model_m[1] = glm::translate(glm::mat4(), glm::vec3(6, 1, -10)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    model_m[2] = glm::translate(glm::mat4(), glm::vec3(-5, -1, -12)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    model_m[3] = glm::translate(glm::mat4(), glm::vec3(4, 4, -15)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    model_m[4] = glm::translate(glm::mat4(), glm::vec3(-4, 2, -20)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    
    
    
    model_mat[0] = view_mat * model_m[0];
    model_mat[1] = view_mat * model_m[1];
    model_mat[2] = view_mat * model_m[2];
    model_mat[3] = view_mat * model_m[3];
    model_mat[4] = view_mat * model_m[4];
    model_mat[5] = view_mat * model_m[5];
    
    scale += 0.1f;
    
    for (unsigned int i = 0; i < 5; i++){
        
        glUniformMatrix4fv(projection_s, 1, GL_FALSE, glm::value_ptr(projection_matrix));
        
        glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(model_m[i]));
        
        glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(model_mat[i]));
        glUniform1i(samplerTexture, 0);
        
        Mesh1->render();
    }
    
    
    glDepthMask(GL_FALSE);
    
    glDisable(GL_DEPTH_TEST);
    
    
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
    
}

void initlprogram(){
    
    Mesh3 = new Mesh();
    Mesh3->loadMesh("sphere.obj");
    
    
    glGenVertexArrays(1, &plvao);
    glBindVertexArray(plvao);
    
    glUseProgram(LightProgram);
    
    LightProgram = AddShaders("pointlight.vert", "pointlight.frag", NULL, NULL, NULL, NULL, NULL);
    
    
    
    GLuint position = glGetAttribLocation(LightProgram, "vPosition");
    GLuint normal = glGetAttribLocation(LightProgram, "vNormal");
    GLuint texture = glGetAttribLocation(LightProgram, "vTexCoord");
    
    
    
    int sizeTexture = sizeof(Mesh3->TexCoords[0])*Mesh3->TexCoords.size();
    int sizePositions = sizeof(Mesh3->Positions[0])*Mesh3->Positions.size();
    int sizeNormals = sizeof(Mesh3->Normals[0])*Mesh3->Normals.size();
    int sizeIndices = sizeof(Mesh3->Indices[0])*Mesh3->Indices.size();
    
    /*Generating the VBO array.*/
    glGenBuffers(4, plvbo);
    /*Positions of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, plvbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizePositions, &(Mesh3->Positions[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Texture coordinates of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, plvbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeTexture, &(Mesh3->TexCoords[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(texture);
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Normals of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, plvbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeNormals, &(Mesh3->Normals[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Indices for the vertices. (VBO)*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plvbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &(Mesh3->Indices[0]), GL_STATIC_DRAW);
    
    
    
    
    modelviewl = glGetUniformLocation(LightProgram, "ModelView");
    projection_l = glGetUniformLocation(LightProgram, "Projection");
    sampler1 = glGetUniformLocation(LightProgram, "gPositionMap");
    sampler2 = glGetUniformLocation(LightProgram, "gColorMap");
    sampler3 = glGetUniformLocation(LightProgram, "gNormalMap");
    LightPos = glGetUniformLocation(LightProgram, "LightPosition");
    Ambient = glGetUniformLocation(LightProgram, "AmbientI");
    Diffuse = glGetUniformLocation(LightProgram, "DiffuseI");
    Specular = glGetUniformLocation(LightProgram, "SpecularI");
    Shininess = glGetUniformLocation(LightProgram, "Shininess");
    screenSize = glGetUniformLocation(LightProgram, "gScreenSize");
    Color = glGetUniformLocation(LightProgram, "LightColor");
    
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    
}

void displaylprogram(){
    
    float MaxChannel = fmax(fmax(255.0f, 0.0f), 0.0f);
    
    float ret = (-0 + sqrtf(0 * 0 -
                            4 * 0.3f * (0.3f - 256 * MaxChannel * DiffuseIntensity)))
    /
    (2 * 0.3f);
    
    
    ret = ret * 0.1;
    
    glUseProgram(LightProgram);
    glBindVertexArray(plvao);
    
    model_m[5] = glm::translate(glm::mat4(), glm::vec3(2, 0, 5)) * glm::scale(glm::mat4(), glm::vec3(ret,ret,ret));
    glm::mat4 model_mat = view_mat * model_m[5];
    
    glUniformMatrix4fv(modelviewl, 1, GL_FALSE, glm::value_ptr(model_mat));
    glUniformMatrix4fv(projection_l, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniform1f(Ambient, AmbientIntensity);
    glUniform1f(Diffuse, DiffuseIntensity);
    glUniform1f(Specular, SpecularIntensity);
    glUniform1f(Shininess, Shine);
    glUniform3fv(LightPos, 1, glm::value_ptr(glm::vec3(2, 0, 5)));
    glUniform2f(screenSizeq, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    glUniform3fv(Color, 1, glm::value_ptr(glm::vec3(255, 0, 0)));
    glUniform1i(sampler1, GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glUniform1i(sampler2, GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glUniform1i(sampler3, GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    
    Mesh3->render();
    
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
}

void initquadprogram(){
    
    Mesh2 = new Mesh();
    Mesh2->loadMesh("quad.obj");
    
    
    glGenVertexArrays(1, &qvao);
    glBindVertexArray(qvao);
    
    glUseProgram(quad_program);
    
    quad_program = AddShaders("quadshader.vert", "dirlight.frag", NULL, NULL, NULL, NULL, NULL);
    
    
    
    GLuint position = glGetAttribLocation(quad_program, "vPosition");
    GLuint normal = glGetAttribLocation(quad_program, "vNormal");
    GLuint texture = glGetAttribLocation(quad_program, "vTexCoord");
    
    
    
    int sizeTexture = sizeof(Mesh2->TexCoords[0])*Mesh2->TexCoords.size();
    int sizePositions = sizeof(Mesh2->Positions[0])*Mesh2->Positions.size();
    int sizeNormals = sizeof(Mesh2->Normals[0])*Mesh2->Normals.size();
    int sizeIndices = sizeof(Mesh2->Indices[0])*Mesh2->Indices.size();
    
    /*Generating the VBO array.*/
    glGenBuffers(4, qbuffer);
    /*Positions of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, qbuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizePositions, &(Mesh2->Positions[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Texture coordinates of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, qbuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeTexture, &(Mesh2->TexCoords[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(texture);
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Normals of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, qbuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeNormals, &(Mesh2->Normals[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Indices for the vertices. (VBO)*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qbuffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &(Mesh2->Indices[0]), GL_STATIC_DRAW);
    
    
    sampler1q = glGetUniformLocation(quad_program, "gPositionMap");
    sampler2q = glGetUniformLocation(quad_program, "gColorMap");
    sampler3q = glGetUniformLocation(quad_program, "gNormalMap");
    Ambientq = glGetUniformLocation(quad_program, "AmbientI");
    Diffuseq = glGetUniformLocation(quad_program, "DiffuseI");
    screenSizeq = glGetUniformLocation(quad_program, "gScreenSize");
    Colorq = glGetUniformLocation(quad_program, "LightColor");
    LightDir = glGetUniformLocation(quad_program, "LightDir");
    
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    
    
    
}


void displayquadprogram(){
    
    glUseProgram(quad_program);
    glBindVertexArray(qvao);
    
    glUniform1i(sampler1q, GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glUniform1i(sampler2q, GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glUniform1i(sampler3q, GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    
    
    glUniform1f(Ambientq, AmbDir);
    glUniform1f(Diffuseq, DifDir);
    glUniform3f(Colorq, (float)ColDir.x, (float)ColDir.y, (float)ColDir.z);
    glUniform2f(screenSizeq, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    glm::vec3 DirectionL = glm::vec3(-1, 0, 0);
    const float Length = sqrtf(DirectionL.x * DirectionL.x + DirectionL.y * DirectionL.y + DirectionL.z * DirectionL.z);
    glUniform3f(LightDir, (float)DirectionL.x / Length, (float)DirectionL.y / Length, (float)DirectionL.z / Length);
    
    Mesh2->render();
    
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
}

void initdshading3(){
    Mesh1 = new Mesh();
    Mesh1->loadMesh("box.obj");
    
    
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glUseProgram(program);
    
    program = AddShaders("meshStatic2.vert", "fshaderMesh2.frag", NULL, NULL, NULL, NULL, NULL);
    
    
    GLuint position = glGetAttribLocation(program, "vPosition");
    GLuint normal = glGetAttribLocation(program, "vNormal");
    GLuint texture = glGetAttribLocation(program, "vTexCoord");
    
    
    
    int sizeTexture = sizeof(Mesh1->TexCoords[0])*Mesh1->TexCoords.size();
    int sizePositions = sizeof(Mesh1->Positions[0])*Mesh1->Positions.size();
    int sizeNormals = sizeof(Mesh1->Normals[0])*Mesh1->Normals.size();
    int sizeIndices = sizeof(Mesh1->Indices[0])*Mesh1->Indices.size();
    
    /*Generating the VBO array.*/
    glGenBuffers(4, buffer);
    /*Positions of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizePositions, &(Mesh1->Positions[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Texture coordinates of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeTexture, &(Mesh1->TexCoords[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(texture);
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Normals of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeNormals, &(Mesh1->Normals[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Indices for the vertices. (VBO)*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &(Mesh1->Indices[0]), GL_STATIC_DRAW);
    
    samplerTexture = glGetUniformLocation(program, "gColorMap");
    glUniform1i(samplerTexture, 0);
    model = glGetUniformLocation(program, "Model");
    modelview = glGetUniformLocation(program, "ModelView");
    projection_s = glGetUniformLocation(program, "Projection");
    
    
    glBindVertexArray(0);
    
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    
}


void displaydshading3(){
    
    glUseProgram(program);
    glBindVertexArray(vao);
    
    gbuffer->BindForGeomPass();
    
    
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    
    model_m[0] = glm::translate(glm::mat4(), glm::vec3(0, 0, -5)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    model_m[1] = glm::translate(glm::mat4(), glm::vec3(6, 1, -10)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    model_m[2] = glm::translate(glm::mat4(), glm::vec3(-5, -1, -12)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    model_m[3] = glm::translate(glm::mat4(), glm::vec3(4, 4, -15)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    model_m[4] = glm::translate(glm::mat4(), glm::vec3(-4, 2, -20)) * glm::rotate(glm::mat4(), glm::radians(scale), glm::vec3(0, 1, 0));
    
    model_mat[0] = view_mat * model_m[0];
    model_mat[1] = view_mat * model_m[1];
    model_mat[2] = view_mat * model_m[2];
    model_mat[3] = view_mat * model_m[3];
    model_mat[4] = view_mat * model_m[4];
    
    scale += 0.1f;
    
    for (unsigned int i = 0; i < 5; i++){
        glUniformMatrix4fv(projection_s, 1, GL_FALSE, glm::value_ptr(projection_matrix));
        glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(model_m[i]));
        glUniformMatrix4fv(modelview, 1, GL_FALSE, glm::value_ptr(model_mat[i]));
        glUniform1i(samplerTexture, 0);
        Mesh1->render();
    }
    
    
    glDepthMask(GL_FALSE);
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
    
}

void initlprogram3(){
    
    Mesh3 = new Mesh();
    Mesh3->loadMesh("sphere.obj");
    
    
    glGenVertexArrays(1, &plvao);
    glBindVertexArray(plvao);
    
    glUseProgram(LightProgram);
    
    LightProgram = AddShaders("pointlight.vert", "pointlight.frag", NULL, NULL, NULL, NULL, NULL);
    
    
    
    GLuint position = glGetAttribLocation(LightProgram, "vPosition");
    GLuint normal = glGetAttribLocation(LightProgram, "vNormal");
    GLuint texture = glGetAttribLocation(LightProgram, "vTexCoord");
    
    
    
    int sizeTexture = sizeof(Mesh3->TexCoords[0])*Mesh3->TexCoords.size();
    int sizePositions = sizeof(Mesh3->Positions[0])*Mesh3->Positions.size();
    int sizeNormals = sizeof(Mesh3->Normals[0])*Mesh3->Normals.size();
    int sizeIndices = sizeof(Mesh3->Indices[0])*Mesh3->Indices.size();
    
    /*Generating the VBO array.*/
    glGenBuffers(4, plvbo);
    /*Positions of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, plvbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizePositions, &(Mesh3->Positions[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Texture coordinates of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, plvbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeTexture, &(Mesh3->TexCoords[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(texture);
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Normals of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, plvbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeNormals, &(Mesh3->Normals[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Indices for the vertices. (VBO)*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plvbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &(Mesh3->Indices[0]), GL_STATIC_DRAW);
    
    
    
    
    modelviewl = glGetUniformLocation(LightProgram, "ModelView");
    projection_l = glGetUniformLocation(LightProgram, "Projection");
    sampler1 = glGetUniformLocation(LightProgram, "gPositionMap");
    sampler2 = glGetUniformLocation(LightProgram, "gColorMap");
    sampler3 = glGetUniformLocation(LightProgram, "gNormalMap");
    LightPos = glGetUniformLocation(LightProgram, "LightPosition");
    Ambient = glGetUniformLocation(LightProgram, "AmbientI");
    Diffuse = glGetUniformLocation(LightProgram, "DiffuseI");
    Specular = glGetUniformLocation(LightProgram, "SpecularI");
    Shininess = glGetUniformLocation(LightProgram, "Shininess");
    screenSize = glGetUniformLocation(LightProgram, "gScreenSize");
    Color = glGetUniformLocation(LightProgram, "LightColor");
    
    
    glUseProgram(0);
    glBindVertexArray(0);
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    
}

void displaylprogram3(){
    
    glUseProgram(LightProgram);
    glBindVertexArray(plvao);
    
    gbuffer->BindForLightPass();
    
    
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    model_m[5] = glm::translate(glm::mat4(), pointlight_pos) * glm::scale(glm::mat4(), glm::vec3(ret, ret, ret));
    glm::mat4 model_mat = view_mat * model_m[5];
    
    
    
    glUniform1i(sampler1, GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glUniform1i(sampler2, GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glUniform1i(sampler3, GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    glUniformMatrix4fv(modelviewl, 1, GL_FALSE, glm::value_ptr(model_mat));
    glUniformMatrix4fv(projection_l, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniform1f(Ambient, AmbientIntensity);
    glUniform1f(Diffuse, DiffuseIntensity);
    glUniform1f(Specular, SpecularIntensity);
    glUniform1f(Shininess, Shine);
    glUniform3fv(LightPos, 1, glm::value_ptr(pointlight_pos));
    glUniform2f(screenSizeq, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    glUniform3fv(Color, 1, glm::value_ptr(glm::vec3(1, 0, 0)));
    
    Mesh3->render();
    
    glCullFace(GL_BACK);
    
    glDisable(GL_BLEND);
    
    glDisable(GL_CULL_FACE);
    
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
    
    
}


void initquadprogram3(){
    
    Mesh2 = new Mesh();
    Mesh2->loadMesh("quad.obj");
    
    
    glGenVertexArrays(1, &qvao);
    glBindVertexArray(qvao);
    
    glUseProgram(quad_program);
    
    quad_program = AddShaders("quadshader.vert", "dirlight.frag", NULL, NULL, NULL, NULL, NULL);
    
    
    
    GLuint position = glGetAttribLocation(quad_program, "vPosition");
    GLuint normal = glGetAttribLocation(quad_program, "vNormal");
    GLuint texture = glGetAttribLocation(quad_program, "vTexCoord");
    
    
    
    int sizeTexture = sizeof(Mesh2->TexCoords[0])*Mesh2->TexCoords.size();
    int sizePositions = sizeof(Mesh2->Positions[0])*Mesh2->Positions.size();
    int sizeNormals = sizeof(Mesh2->Normals[0])*Mesh2->Normals.size();
    int sizeIndices = sizeof(Mesh2->Indices[0])*Mesh2->Indices.size();
    
    /*Generating the VBO array.*/
    glGenBuffers(4, qbuffer);
    /*Positions of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, qbuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizePositions, &(Mesh2->Positions[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Texture coordinates of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, qbuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeTexture, &(Mesh2->TexCoords[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(texture);
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Normals of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, qbuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeNormals, &(Mesh2->Normals[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Indices for the vertices. (VBO)*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qbuffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &(Mesh2->Indices[0]), GL_STATIC_DRAW);
    
    
    sampler1q = glGetUniformLocation(quad_program, "gPositionMap");
    sampler2q = glGetUniformLocation(quad_program, "gColorMap");
    sampler3q = glGetUniformLocation(quad_program, "gNormalMap");
    Ambientq = glGetUniformLocation(quad_program, "AmbientI");
    Diffuseq = glGetUniformLocation(quad_program, "DiffuseI");
    screenSizeq = glGetUniformLocation(quad_program, "gScreenSize");
    Colorq = glGetUniformLocation(quad_program, "LightColor");
    LightDir = glGetUniformLocation(quad_program, "LightDir");
    
    
    glUseProgram(0);
    glBindVertexArray(0);
    /*
     glEnable(GL_DEPTH_TEST);
     glClearColor(0.0, 0.0, 0.0, 1.0);
     */
    
    
    
}


void displayquadprogram3(){
    
    glUseProgram(quad_program);
    glBindVertexArray(qvao);
    
    gbuffer->BindForLightPass();
    
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
    
    
    glUniform1i(sampler1q, GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glUniform1i(sampler2q, GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glUniform1i(sampler3q, GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    
    glUniform1f(Ambientq, AmbDir);
    glUniform1f(Diffuseq, DifDir);
    glUniform3f(Colorq, (float)ColDir.x, (float)ColDir.y, (float)ColDir.z);
    glUniform2f(screenSizeq, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    glm::vec3 DirectionL = glm::vec3(-1, 0, 0);
    const float Length = sqrtf(DirectionL.x * DirectionL.x + DirectionL.y * DirectionL.y + DirectionL.z * DirectionL.z);
    glUniform3f(LightDir, (float)DirectionL.x / Length, (float)DirectionL.y / Length, (float)DirectionL.z / Length);
    
    
    Mesh2->render();
    
    glDisable(GL_BLEND);
    
    
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
    
    
}

void initStencil(){
    
    glUseProgram(StencilProgram);
    glGenVertexArrays(1, &stvao);
    glBindVertexArray(stvao);
    
    
    StencilProgram = AddShaders("stencil.vert", "stencil.frag", NULL, NULL, NULL, NULL, NULL);
    
    
    GLuint position = glGetAttribLocation(StencilProgram, "vPosition");
    GLuint normal = glGetAttribLocation(StencilProgram, "vNormal");
    GLuint texture = glGetAttribLocation(StencilProgram, "vTexCoord");
    
    
    
    int sizeTexture = sizeof(Mesh3->TexCoords[0])*Mesh3->TexCoords.size();
    int sizePositions = sizeof(Mesh3->Positions[0])*Mesh3->Positions.size();
    int sizeNormals = sizeof(Mesh3->Normals[0])*Mesh3->Normals.size();
    int sizeIndices = sizeof(Mesh3->Indices[0])*Mesh3->Indices.size();
    
    /*Generating the VBO array.*/
    glGenBuffers(4, stvbo);
    /*Positions of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, stvbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizePositions, &(Mesh3->Positions[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Texture coordinates of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, stvbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeTexture, &(Mesh3->TexCoords[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(texture);
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Normals of the 3D model. (VBO)*/
    glBindBuffer(GL_ARRAY_BUFFER, stvbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeNormals, &(Mesh3->Normals[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*Indices for the vertices. (VBO)*/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stvbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &(Mesh3->Indices[0]), GL_STATIC_DRAW);
    
    
    
    modelviewst = glGetUniformLocation(StencilProgram, "ModelView");
    projection_st = glGetUniformLocation(StencilProgram, "Projection");
    
    glBindVertexArray(0);
    
}

void displayStencil(){
    
    glUseProgram(StencilProgram);
    glBindVertexArray(stvao);
    
    gbuffer->BindForStencilPass();
    
    glEnable(GL_DEPTH_TEST);
    
    glDisable(GL_CULL_FACE);
    
    glClear(GL_STENCIL_BUFFER_BIT);
    
    glStencilFunc(GL_ALWAYS, 0, 0);
    
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
    
    
    model_m[5] = glm::translate(glm::mat4(), pointlight_pos) * glm::scale(glm::mat4(), glm::vec3(ret,ret,ret));
    glm::mat4 model_mat = view_mat * model_m[5];
    
    glUniformMatrix4fv(modelviewst, 1, GL_FALSE, glm::value_ptr(model_mat));
    glUniformMatrix4fv(projection_st, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    
    Mesh3->render();
    
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
    
    
}

bool	initImGui()
{
    // Setup ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::SetCurrentContext(ImGui::CreateContext());

	// Setup ImGui binding

	if (!ImGui_ImplOpenGL3_Init("#version 150"))
	{
		return false;
	}
	if (!ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext))
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
    
    if (init() == false)
    {
        std::cout << "Init failed !!!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    gbuffer = new GBuffer();
    
    gbuffer->Init(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    
    //initCube();
    
    initdshading3();
    
    initlprogram3();
    
    initquadprogram3();
    
    initStencil();
    
    glUseProgram(LightProgram);
    
    
    glUniform1i(sampler1, GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glUniform1i(sampler2, GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glUniform1i(sampler3, GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    
    glUseProgram(0);
    
    
    glUseProgram(quad_program);
    
    glUseProgram(0);
    float MaxChannel = fmax(fmax(1.0f, 0.0f), 0.0f);
    
    ret = (-0 + sqrtf(0 * 0 -
                      4 * 0.3f * (0.3f - 256 * MaxChannel * DiffuseIntensity)))
    /
    (2 * 0.3f);
    
    
    // Main loop
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            
            //our custom event_handler
            event_handler(&event);
            
            if (event.type == SDL_QUIT)
                running = false;
            
        }
        
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(gWindow);
		ImGui::NewFrame();
        
        view_mat = glm::lookAt(
                               glm::vec3(px, py, mz),
                               glm::vec3(mx, my, 0),
                               glm::vec3(0, 1, 0)
                               );
        
        
        // Rendering
        //glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearColor(0.0, 0.0, 0.0, 1.0);
        
        displayGui();
        
        gbuffer->StartFrame();
        displaydshading3();
        
        glEnable(GL_STENCIL_TEST);
        displayStencil();
        displaylprogram3();
        glDisable(GL_STENCIL_TEST);
        
        displayquadprogram3();
        gbuffer->BindForFinalPass();
        glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        
        //displayCube();
        //However displayCube is rendered , we want ImGui to be filled
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
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

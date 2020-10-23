//shadow map not working in MacOSX

// basic STL streams
#include <iostream>

// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>

//Update 04/08/16
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>

// Here we decide which of the two versions we want to use
// If your systems supports both, choose to uncomment USE_OPENGL32
// otherwise choose to uncomment USE_OPENGL21
// GLView cna also help you decide before running this program:
//
// FOR MACOSX only, please use OPENGL32 for AntTweakBar to work properly
//
#define USE_OPENGL32

#include <glGA/glGARigMesh.h>

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
#include <fstream>
//local
#include "glGA/glGAHelper.h"
#include "glGA/glGAMesh.h"




// number of Squares for Plane
#define NumOfSQ 50

// global variables
int         windowWidth=1024, windowHeight=768;

//color
float bgColor[] = { 0.0f, 0.0f, 0.0f, 0.1f };

GLuint      programX, programY, programZ, programPlane, program3Dmodel, programPyramid;
GLuint      vao, vaoX, vaoY, vaoZ, vaoPlane, vao3Dmodel, vaoPyramid;
GLuint      bufferX, bufferY, bufferZ, bufferPlane, buffer3Dmodel, bufferPyramid;

GLuint      eyes_uniform;
GLuint		MV_uniformX       , MVP_uniformX;
GLuint		MV_uniformY       , MVP_uniformY;
GLuint		MV_uniformZ       , MVP_uniformZ;
GLuint		MV_uniformPlane   , MVP_uniformPlane   , Normal_uniformPlane, V_uniformPlane;
GLuint		MV_uniform3D      , MVP_uniform3D      , Normal_uniform3D   , V_uniform3D;
GLuint		MV_uniformPyramid , MVP_uniform_Pyramid; 
GLuint		MVlight;
GLuint		TextureMatrix_Uniform;
int timesc = 0;
GLuint		gSampler1,gSampler;

Texture	    *pTexture = NULL;
Mesh		*m		  = NULL;

const		int			NumVerticesl = 2;
const		int			NumVerticesSQ = ( (NumOfSQ) * (NumOfSQ)) * (2) * (3);

bool        wireFrame = false;
bool		camera    = false;

typedef     glm::vec4   color4;
typedef     glm::vec4   point4;

int			IndexSQ = 0;
int			IndexPyramid = 0;

//Modelling arrays
point4		pointPyramid[18];
color4		colorPyramid[18];
point4		pointsq[NumVerticesSQ];
color4		colorsq[NumVerticesSQ];
glm::vec3	normalsq[NumVerticesSQ];
glm::vec4	tex_coords[NumVerticesSQ];

glm::vec3 pos = glm::vec3( 3.0f, 4.0f , 10.0f );

float horizAngle = 3.14f;
float verticAngle = 0.0f;


float speedo = 9.0f;
float mouseSpeedo = 0.005f;

int	  xpos = 0,ypos = 0;

struct Lights
{
	GLint activeLight;

	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat position[4];

	glm::vec3 spotDirection;  
    GLfloat spotExponent;        
    GLfloat spotCutoff;                                     
                              // (range: [0.0,90.0], 180.0)   
    GLfloat spotCosCutoff;      // Derived: cos(Crli)                 
                              // (range: [1.0,0.0],-1.0)

    GLfloat constantAttenuation;  
    GLfloat linearAttenuation;     
    GLfloat quadraticAttenuation;
};

struct MaterialProperties
{

   GLfloat emission[4];   
   GLfloat ambient[4];   
   GLfloat diffuse[4];   
   GLfloat specular[4];   
   GLfloat shininess; 
};

struct MaterialProperties *frontMaterial,*backMaterial;

struct FogProperties
{
	int   enable;
	float density;
	float start;
	float end;

	glm::vec4 color;

	enum equation{FOG_EQUATION_LINEAR , FOG_EQUATION_EXP, FOG_EQUATION_EXP2};
	equation pick;
	float scale;
};

struct FogProperties *Fog;

int				    maxLights = 0;                  // maximum number of dynamic lights allowed by the graphic card
Lights			   *lights = NULL;						// array of lights
MaterialProperties *materials = NULL;
 

//Plane
point4		planeVertices[NumVerticesSQ];
color4		planeColor[NumVerticesSQ];

//update 04/08/16
SDL_Window		*gWindow = NULL;
SDL_GLContext	gContext;

//Vertices of a XYZ axis
// X - axis
point4 Xvertices[2] = {
	point4( 0.0, 0.0 , 0.0 , 1.0),
	point4( 5.0, 0.0 , 0.0 , 1.0)  
};

color4 Xvertex_color[2] = {
	color4( 1.0, 0.0, 0.0, 1.0),
	color4( 1.0, 0.0, 0.0, 1.0)
};

// Y - axis
point4 Yvertices[2] = {
	point4( 0.0, 0.0, 0.0, 1.0),
	point4( 0.0, 5.0, 0.0, 1.0)
};

color4 Yvertex_color[2] = {
	color4( 0.0, 1.0, 0.0, 1.0),
	color4( 0.0, 1.0, 0.0, 1.0)
};

// Z - axis
point4 Zvertices[2] = {
	point4( 0.0, 0.0, 0.0, 1.0),
	point4( 0.0, 0.0, 5.0, 1.0)
};

color4 Zvertex_color[2] = {
	color4( 0.0, 0.0, 1.0, 1.0),
	color4( 0.0, 0.0, 1.0, 1.0)
};

point4 Pyramid[5] = {
	point4(-1.0, 6.0, 1.0, 1.0),
	point4(-1.0, 6.0,-1.0, 1.0),
	point4( 1.0, 6.0,-1.0, 1.0),
	point4( 1.0, 6.0, 1.0, 1.0),
	point4( 0.0, 9.0, 0.0, 1.0)
};

color4 PyramidsColor[5] = {
	color4(0.1, 1.0, 0.1, 1.0),
	color4(0.1, 1.0, 0.1, 1.0),
	color4(0.1, 1.0, 0.1, 1.0),
	color4(0.1, 1.0, 0.1, 1.0),
	color4(0.1, 1.0, 0.1, 1.0)
};

// Update function prototypes
bool			initSDL();
bool			event_handler(SDL_Event* event);
void			close();
void			resize_window(int width, int height);
bool			initImGui();
void			displayGui();

// Update functions 04/08/16
bool			initSDL()
{
	//Init flag
	bool success = true;

	//Basic Setup
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
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
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		//SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

#ifdef __APPLE__
		SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
#endif

		//Create Window
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

#ifdef __APPLE__
		gWindow = SDL_CreateWindow("Chapter-9", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );
#else
		gWindow = SDL_CreateWindow("Chapter-9", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

				//Init glViewport first time;
				resize_window(windowWidth, windowHeight);
			}
		}
	}
	return success;
}

bool			event_handler(SDL_Event* event)
{
	switch (event->type)
	{
	case SDL_WINDOWEVENT:
	{
		if (event->window.event == SDL_WINDOWEVENT_RESIZED)
		{
			resize_window(event->window.data1, event->window.data2);
		}
	}
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
			return true;
		}

		if (event->key.keysym.sym == SDLK_SPACE)
		{
			if (camera == false)
			{
				camera = true;
				SDL_WarpMouseInWindow(gWindow, windowWidth / 2, windowHeight / 2);
				SDL_GetMouseState(&xpos, &ypos);
			}
			else
			{
				camera = false;
				SDL_WarpMouseInWindow(gWindow, windowWidth / 2, windowHeight / 2);
				SDL_GetMouseState(&xpos, &ypos);
			}
			return true;
		}
		return true;
	}
	case SDL_KEYUP:
	{
		return true;
	}
	case SDL_MOUSEMOTION:
	{

		return true;
	}
	}
	return false;
}

void			close()
{
	//Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}

void			resize_window(int width, int height)
{
	// Set OpenGL viewport and default camera
	glViewport(0, 0, width, height);

	//float aspect = (GLfloat)width / (GLfloat)height;

	windowWidth = width;
	windowHeight = height;
}

bool			initImGui()
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
	//io.Fonts->AddFontFromFileTTF("../../_thirdPartyLibs/include/ImGUI/extra_fonts/Karla-Regular.ttf", 14.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	return true;
}

void			displayGui()
{
	//ImGui::SetNextWindowSize(ImVec2(200, 200));
	//ImGui::SetWindowPos(ImVec2(10, 10));
	ImGui::Begin("Main");
	ImGui::SetWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);

	static bool checkbox = false;
	if (ImGui::Checkbox("Wireframe", &checkbox))
	{
		if (checkbox == false)
			wireFrame = false;
		else
			wireFrame = true;
	}

	ImGui::Separator();
	ImGui::ColorEdit3("Background Color", bgColor);
	ImGui::Separator();
	if (ImGui::TreeNode("Fog properties"))
	{
		ImGui::SliderInt("Fog Enable", &Fog->enable, 0, 1);
		ImGui::InputFloat("Fog Start", &Fog->start, (1.0F), (5.0F), 2);
		ImGui::InputFloat("Fog End", &Fog->end, (1.0F), (5.0F), 2);
		ImGui::InputFloat("Fog Density", &Fog->density, (0.01F), (0.02F), 2);

		if (Fog->start > 100.0f)
			Fog->start = 100.0f;
		if (Fog->start < 0.0f)
			Fog->start = 0.0f;

		if (Fog->end > 100.0f)
			Fog->end = 100.0f;
		if (Fog->end < 0.0f)
			Fog->end = 0.0f;

		if (Fog->density < 0.01f)
			Fog->density = 0.01f;

		const char* items[] = { "Linear Equation", "Exp Equation", "Exp2 Equation" };
		static int current_item = 0;
		ImGui::Combo("Fog Equation", &current_item, items, 3);

		if (current_item == 0)
			Fog->pick = FogProperties::FOG_EQUATION_LINEAR;
		else if (current_item == 1)
			Fog->pick = FogProperties::FOG_EQUATION_EXP;
		else
			Fog->pick = FogProperties::FOG_EQUATION_EXP2;

		ImGui::ColorEdit3("Fog Color", (float*)&Fog->color);

		ImGui::TreePop();
	}
	ImGui::End();

	ImGui::Begin("Lights-Materials");
	ImGui::SetWindowPos(ImVec2(10, 220), ImGuiSetCond_Once);
	ImGui::SetWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);

	if (ImGui::TreeNode("Edit Lights"))
	{
		if (ImGui::TreeNode("Light #1"))
		{
			ImGui::SliderInt("activeLight", &lights[0].activeLight, 0, 1);
			ImGui::ColorEdit3("Ambient", lights[0].ambient);
			ImGui::ColorEdit3("Diffuse", lights[0].diffuse);
			ImGui::ColorEdit3("Specular", lights[0].specular);
			if (ImGui::TreeNode("Position"))
			{
				ImGui::SliderFloat3("Position", lights[0].position, -100.0f, 100.0f, "%.2f");
				ImGui::InputFloat("X", &lights[0].position[0], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Y", &lights[0].position[1], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Z", &lights[0].position[2], 0.5F, 0.1F, 2);
				ImGui::TreePop();
			}
			ImGui::InputFloat("spotCutoff", &lights[0].spotCutoff, 1.0F, 5.0F, 2);
			ImGui::InputFloat("spotCosCutoff", &lights[0].spotCosCutoff, 1.0F, 5.0F, 2);
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNode("Light #2"))
		{
			ImGui::SliderInt("activeLight", &lights[1].activeLight, 0, 1);
			ImGui::ColorEdit3("Ambient", lights[1].ambient);
			ImGui::ColorEdit3("Diffuse", lights[1].diffuse);
			ImGui::ColorEdit3("Specular", lights[1].specular);
			if (ImGui::TreeNode("Position"))
			{
				ImGui::SliderFloat3("Position", lights[1].position, -100.0f, 100.0f, "%.2f");
				ImGui::InputFloat("X", &lights[1].position[0], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Y", &lights[1].position[1], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Z", &lights[1].position[2], 0.5F, 0.1F, 2);
				ImGui::TreePop();
			}
			ImGui::InputFloat("spotCutoff", &lights[1].spotCutoff, 1.0F, 5.0F, 2);
			ImGui::InputFloat("spotCosCutoff", &lights[1].spotCosCutoff, 1.0F, 5.0F, 2);
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNode("Light #3"))
		{
			ImGui::SliderInt("activeLight", &lights[2].activeLight, 0, 1);
			ImGui::ColorEdit3("Ambient", lights[2].ambient);
			ImGui::ColorEdit3("Diffuse", lights[2].diffuse);
			ImGui::ColorEdit3("Specular", lights[2].specular);
			if (ImGui::TreeNode("Position"))
			{
				ImGui::SliderFloat3("Position", lights[2].position, -100.0f, 100.0f, "%.2f");
				ImGui::InputFloat("X", &lights[2].position[0], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Y", &lights[2].position[1], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Z", &lights[2].position[2], 0.5F, 0.1F, 2);
				ImGui::TreePop();
			}
			ImGui::InputFloat("spotCutoff", &lights[2].spotCutoff, 1.0F, 5.0F, 2);
			ImGui::InputFloat("spotCosCutoff", &lights[2].spotCosCutoff, 1.0F, 5.0F, 2);
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNode("Light #4"))
		{
			ImGui::SliderInt("activeLight", &lights[3].activeLight, 0, 1);
			ImGui::ColorEdit3("Ambient", lights[3].ambient);
			ImGui::ColorEdit3("Diffuse", lights[3].diffuse);
			ImGui::ColorEdit3("Specular", lights[3].specular);
			if (ImGui::TreeNode("Position"))
			{
				ImGui::SliderFloat3("Position", lights[3].position, -100.0f, 100.0f, "%.2f");
				ImGui::InputFloat("X", &lights[3].position[0], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Y", &lights[3].position[1], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Z", &lights[3].position[2], 0.5F, 0.1F, 2);
				ImGui::TreePop();
			}
			ImGui::InputFloat("spotCutoff", &lights[3].spotCutoff, 1.0F, 5.0F, 2);
			ImGui::InputFloat("spotCosCutoff", &lights[3].spotCosCutoff, 1.0F, 5.0F, 2);
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNode("Light #5"))
		{
			ImGui::SliderInt("activeLight", &lights[4].activeLight, 0, 1);
			ImGui::ColorEdit3("Ambient", lights[4].ambient);
			ImGui::ColorEdit3("Diffuse", lights[4].diffuse);
			ImGui::ColorEdit3("Specular", lights[4].specular);
			if (ImGui::TreeNode("Position"))
			{
				ImGui::SliderFloat3("Position", lights[4].position, -100.0f, 100.0f, "%.2f");
				ImGui::InputFloat("X", &lights[4].position[0], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Y", &lights[4].position[1], 0.5F, 0.1F, 2);
				ImGui::InputFloat("Z", &lights[4].position[2], 0.5F, 0.1F, 2);
				ImGui::TreePop();
			}
			ImGui::InputFloat("spotCutoff", &lights[4].spotCutoff, 1.0F, 5.0F, 2);
			ImGui::InputFloat("spotCosCutoff", &lights[4].spotCosCutoff, 1.0F, 5.0F, 2);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Materials"))
	{
		if (ImGui::TreeNode("FrontMaterial"))
		{
			ImGui::ColorEdit4("Ambient", frontMaterial->ambient);
			ImGui::ColorEdit4("Diffuse", frontMaterial->diffuse);
			ImGui::ColorEdit4("Specular", frontMaterial->specular);
			ImGui::ColorEdit4("Emission", frontMaterial->emission);
			ImGui::SliderFloat("Shininess", &frontMaterial->shininess, 0.0f, 100.0f, "%.2f");
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNode("BackMaterial"))
		{
			ImGui::ColorEdit4("Ambient", backMaterial->ambient);
			ImGui::ColorEdit4("Diffuse", backMaterial->diffuse);
			ImGui::ColorEdit4("Specular", backMaterial->specular);
			ImGui::ColorEdit4("Emission", backMaterial->emission);
			ImGui::SliderFloat("Shininess", &backMaterial->shininess, 0.0f, 100.0f, "%.2f");
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	ImGui::End();
}


int getActiveLights(Lights *ptr)
{
	int activeLight = 0;

	if(ptr == NULL)
	{
		return activeLight;
	}
	else
	{
		for(int i=0;i<maxLights;i++)
		{
			if(ptr[i].activeLight == 1)
			{
				activeLight++;
			}
		}

		return activeLight;
	}
}


void initLights()
{
	// Get the max number of lights allowed by the graphic card
    glGetIntegerv(GL_MAX_LIGHTS, &maxLights);

	if(maxLights > 8)
	{
		maxLights = 5;
	}

    maxLights = 5;
    
	lights = new Lights[maxLights];

	for(int i=0;i<maxLights;i++)
	{
		lights[i].activeLight = 0;
	}

	//#Light-1- Directional Light
	lights[0].activeLight = 0;
	lights[0].ambient[0] = 0.3f;lights[0].ambient[1] = 0.3f;lights[0].ambient[2] = 0.3f;lights[0].ambient[3] = 1.0f;
	lights[0].diffuse[0] = 1.0f;lights[0].diffuse[1] = 1.0f;lights[0].diffuse[2] = 1.0f;lights[0].diffuse[3] = 1.0f;
	lights[0].specular[0] = 1.0f;lights[0].specular[1] = 1.0f;lights[0].specular[2] = 1.0f;lights[0].specular[3] = 1.0f;
	lights[0].position[0] = 10.0f;lights[0].position[1] = 1.0f;lights[0].position[2] = 0.0f;lights[0].position[3] = 0.0f;

	//#Light-2- Point Light
	lights[1].activeLight = 0;
	lights[1].ambient[0] = 0.3f;lights[1].ambient[1] = 0.3f;lights[1].ambient[2] = 0.3f;lights[1].ambient[3] = 1.0f;
	lights[1].diffuse[0] = 1.0f;lights[1].diffuse[1] = 1.0f;lights[1].diffuse[2] = 1.0f;lights[1].diffuse[3] = 1.0f;
	lights[1].specular[0] = 1.0f;lights[1].specular[1] = 1.0f;lights[1].specular[2] = 1.0f;lights[1].specular[3] = 1.0f;
	lights[1].position[0] = -4.5f;lights[1].position[1] = 1.0f;lights[1].position[2] = 4.0f;lights[1].position[3] = 1.0f;
	lights[1].constantAttenuation = 1.0f;lights[1].linearAttenuation = 0.0f;lights[1].quadraticAttenuation = 0.0f;
	lights[1].spotCutoff = 180.0f;

	//#lights-2- Point Light
	lights[2].activeLight = 0;
	lights[2].ambient[0] = 0.3f;lights[2].ambient[1] = 0.3f;lights[2].ambient[2] = 0.3f;lights[2].ambient[3] = 1.0f;
	lights[2].diffuse[0] = 1.0f;lights[2].diffuse[1] = 1.0f;lights[2].diffuse[2] = 1.0f;lights[2].diffuse[3] = 1.0f;
	lights[2].specular[0] = 1.0f;lights[2].specular[1] = 1.0f;lights[2].specular[2] = 1.0f;lights[2].specular[3] = 1.0f;
	lights[2].position[0] = -4.5f;lights[2].position[1] = 1.0f;lights[2].position[2] = -4.0f;lights[2].position[3] = 1.0f;
	lights[2].constantAttenuation = 1.0f;lights[2].linearAttenuation = 0.0f;lights[2].quadraticAttenuation = 0.0f;
	lights[2].spotCutoff = 180.0f;

	//#lights-3- Spot Light
	lights[3].activeLight = 0;
	lights[3].ambient[0] = 0.3f;lights[3].ambient[1] = 0.3f;lights[3].ambient[2] = 0.3f;lights[3].ambient[3] = 1.0f;
	lights[3].diffuse[0] = 1.0f;lights[3].diffuse[1] = 1.0f;lights[3].diffuse[2] = 1.0f;lights[3].diffuse[3] = 1.0f;
	lights[3].specular[0] = 1.0f;lights[3].specular[1] = 1.0f;lights[3].specular[2] = 1.0f;lights[3].specular[3] = 1.0f;
	lights[3].position[0] = -4.5f;lights[3].position[1] = 1.0f;lights[3].position[2] = 4.0f;lights[3].position[3] = 1.0f;
	lights[3].constantAttenuation = 1.0f;lights[3].linearAttenuation = 0.0f;lights[3].quadraticAttenuation = 0.0f;
	lights[3].spotCutoff = 45.0f;
	lights[3].spotDirection = glm::vec3(-1.0f,-1.0f,1.0f);
	lights[3].spotExponent = 15.0f;
	lights[3].spotCosCutoff = glm::cos(glm::radians(glm::clamp(lights[3].spotCutoff,0.0f,90.0f)));

	//#lights-4- Spot Light
	lights[4].activeLight = 0;
	lights[4].ambient[0] = 0.3f;lights[4].ambient[1] = 0.3f;lights[4].ambient[2] = 0.3f;lights[4].ambient[3] = 1.0f;
	lights[4].diffuse[0] = 1.0f;lights[4].diffuse[1] = 1.0f;lights[4].diffuse[2] = 1.0f;lights[4].diffuse[3] = 1.0f;
	lights[4].specular[0] = 1.0f;lights[4].specular[1] = 1.0f;lights[4].specular[2] = 1.0f;lights[4].specular[3] = 1.0f;
	lights[4].position[0] = -4.5f;lights[4].position[1] = 1.0f;lights[4].position[2] = -4.0f;lights[4].position[3] = 1.0f;
	lights[4].constantAttenuation = 1.0f;lights[4].linearAttenuation = 0.0f;lights[4].quadraticAttenuation = 0.0f;
	lights[4].spotCutoff = 45.0f;
	lights[4].spotDirection = glm::vec3(-1.0f,-1.0f,1.0f);
	lights[4].spotExponent = 40.0f;
	lights[4].spotCosCutoff = glm::cos(glm::radians(glm::clamp(lights[4].spotCutoff,0.0f,90.0f)));
	
}

void initMaterials()
{
	frontMaterial = new MaterialProperties;

	frontMaterial->ambient[0] = 0.2f;frontMaterial->ambient[1] = 0.2f;frontMaterial->ambient[2] = 0.2f;frontMaterial->ambient[3] = 1.0f;
	frontMaterial->diffuse[0] = 1.0f;frontMaterial->diffuse[1] = 0.8f;frontMaterial->diffuse[2] = 0.8f;frontMaterial->diffuse[3] = 1.0f;
	frontMaterial->specular[0] = 1.0f;frontMaterial->specular[1] = 1.0f;frontMaterial->specular[2] = 1.0f;frontMaterial->specular[3] = 1.0f;
	frontMaterial->emission[0] = 0.0f;frontMaterial->emission[1] = 0.0f;frontMaterial->emission[2] = 0.0f;frontMaterial->emission[3] = 1.0f;
	frontMaterial->shininess = 5.0f;

	backMaterial = new MaterialProperties;

	backMaterial->ambient[0] = 0.2f;backMaterial->ambient[1] = 0.2f;backMaterial->ambient[2] = 0.2f;backMaterial->ambient[3] = 1.0f;
	backMaterial->diffuse[0] = 0.0f;backMaterial->diffuse[1] = 0.0f;backMaterial->diffuse[2] = 1.0f;backMaterial->diffuse[3] = 1.0f;
	backMaterial->specular[0] = 1.0f;backMaterial->specular[1] = 1.0f;backMaterial->specular[2] = 1.0f;backMaterial->specular[3] = 1.0f;
	backMaterial->emission[0] = 0.0f;backMaterial->emission[1] = 0.0f;backMaterial->emission[2] = 0.0f;backMaterial->emission[3] = 1.0f;
	backMaterial->shininess = 5.0f;
}

void initFog()
{
	Fog = new FogProperties;

	Fog->enable = 1;

	Fog->density = 0.1f;
	Fog->start   = 0.0f;
	Fog->end		= 20.0f;

	Fog->color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);

	Fog->pick = (FogProperties::equation)(FogProperties::FOG_EQUATION_LINEAR);

	Fog->scale = 1.0f / (Fog->end - Fog->start); 
	
}

void LoadUniforms(GLuint program)
{
	//Load Lights
	//Directional Light -1-
	
	glUniform1iv( glGetUniformLocation(program, "lights[0].activeLight"),1,&(lights[0].activeLight));
	glUniform4fv( glGetUniformLocation(program, "lights[0].ambient"),1,lights[0].ambient);
	glUniform4fv( glGetUniformLocation(program, "lights[0].diffuse"),1,lights[0].diffuse);
	glUniform4fv( glGetUniformLocation(program, "lights[0].specular"),1,lights[0].specular);
	glUniform4fv( glGetUniformLocation(program, "lights[0].position"),1,lights[0].position);
	
	//Point Light -2-
	
	glUniform1iv( glGetUniformLocation(program, "lights[1].activeLight"),1,&(lights[1].activeLight));
	glUniform4fv( glGetUniformLocation(program, "lights[1].ambient"),1,lights[1].ambient);
	glUniform4fv( glGetUniformLocation(program, "lights[1].diffuse"),1,lights[1].diffuse);
	glUniform4fv( glGetUniformLocation(program, "lights[1].specular"),1,lights[1].specular);
	glUniform4fv( glGetUniformLocation(program, "lights[1].position"),1,lights[1].position);
	glUniform1fv( glGetUniformLocation(program, "lights[1].constantAttenuation"),1,&(lights[1].constantAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[1].linearAttenuation"),1,&(lights[1].linearAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[1].quadraticAttenuation"),1,&(lights[1].quadraticAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[1].spotCutoff"),1,&(lights[1].spotCutoff));
	
	//Point Light -3-
	
	glUniform1iv( glGetUniformLocation(program, "lights[2].activeLight"),1,&(lights[2].activeLight));
	glUniform4fv( glGetUniformLocation(program, "lights[2].ambient"),1,lights[2].ambient);
	glUniform4fv( glGetUniformLocation(program, "lights[2].diffuse"),1,lights[2].diffuse);
	glUniform4fv( glGetUniformLocation(program, "lights[2].specular"),1,lights[2].specular);
	glUniform4fv( glGetUniformLocation(program, "lights[2].position"),1,lights[2].position);
	glUniform1fv( glGetUniformLocation(program, "lights[2].constantAttenuation"),1,&(lights[2].constantAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[2].linearAttenuation"),1,&(lights[2].linearAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[2].quadraticAttenuation"),1,&(lights[2].quadraticAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[2].spotCutoff"),1,&(lights[2].spotCutoff));
	
	//Spot Light -4-
	
	glUniform1iv( glGetUniformLocation(program, "lights[3].activeLight"),1,&(lights[3].activeLight));
	glUniform4fv( glGetUniformLocation(program, "lights[3].ambient"),1,lights[3].ambient);
	glUniform4fv( glGetUniformLocation(program, "lights[3].diffuse"),1,lights[3].diffuse);
	glUniform4fv( glGetUniformLocation(program, "lights[3].specular"),1,lights[3].specular);
	glUniform4fv( glGetUniformLocation(program, "lights[3].position"),1,lights[3].position);
	glUniform1fv( glGetUniformLocation(program, "lights[3].constantAttenuation"),1,&(lights[3].constantAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[3].linearAttenuation"),1,&(lights[3].linearAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[3].quadraticAttenuation"),1,&(lights[3].quadraticAttenuation));
	glUniform1fv( glGetUniformLocation(program, "lights[3].spotCutoff"),1,&(lights[3].spotCutoff));
	glUniform3fv( glGetUniformLocation(program, "lights[3].spotDirection"),1,glm::value_ptr(lights[3].spotDirection));
	glUniform1fv( glGetUniformLocation(program, "lights[3].spotExponent"),1,&(lights[3].spotExponent));
	lights[3].spotCosCutoff = glm::cos(glm::radians(glm::clamp(lights[3].spotCutoff,0.0f,90.0f)));
	glUniform1fv( glGetUniformLocation(program, "lights[3].spotCosCutoff"),1,&(lights[3].spotCosCutoff));
	
	//Spot Light -5-
	
	//glUniform1iv( glGetUniformLocation(program, "lights[4].activeLight"),1,&(lights[4].activeLight));
	//glUniform4fv( glGetUniformLocation(program, "lights[4].ambient"),1,lights[4].ambient);
	//glUniform4fv( glGetUniformLocation(program, "lights[4].diffuse"),1,lights[4].diffuse);
	//glUniform4fv( glGetUniformLocation(program, "lights[4].specular"),1,lights[4].specular);
	//glUniform4fv( glGetUniformLocation(program, "lights[4].position"),1,lights[4].position);
	//glUniform1fv( glGetUniformLocation(program, "lights[4].constantAttenuation"),1,&(lights[4].constantAttenuation));
	//glUniform1fv( glGetUniformLocation(program, "lights[4].linearAttenuation"),1,&(lights[4].linearAttenuation));
	//glUniform1fv( glGetUniformLocation(program, "lights[4].quadraticAttenuation"),1,&(lights[4].quadraticAttenuation));
	//glUniform1fv( glGetUniformLocation(program, "lights[4].spotCutoff"),1,&(lights[4].spotCutoff));
	//glUniform3fv( glGetUniformLocation(program, "lights[4].spotDirection"),1,glm::value_ptr(lights[4].spotDirection));
	//glUniform1fv( glGetUniformLocation(program, "lights[4].spotExponent"),1,&(lights[4].spotExponent));
	//glUniform1fv( glGetUniformLocation(program, "lights[4].spotCosCutoff"),1,&(lights[4].spotCosCutoff));
	
	//Load Material Properties
	glUniform4fv( glGetUniformLocation(program, "frontMaterial.ambient"),1,frontMaterial->ambient);
	glUniform4fv( glGetUniformLocation(program, "frontMaterial.diffuse"),1,frontMaterial->diffuse);
	glUniform4fv( glGetUniformLocation(program, "frontMaterial.specular"),1,frontMaterial->specular);
	glUniform4fv( glGetUniformLocation(program, "frontMaterial.emission"),1,frontMaterial->emission);
	glUniform1fv( glGetUniformLocation(program, "frontMaterial.shininess"),1,&(frontMaterial->shininess));

	glUniform4fv( glGetUniformLocation(program, "backMaterial.ambient"),1,backMaterial->ambient);
	glUniform4fv( glGetUniformLocation(program, "backMaterial.diffuse"),1,backMaterial->diffuse);
	glUniform4fv( glGetUniformLocation(program, "backMaterial.specular"),1,backMaterial->specular);
	glUniform4fv( glGetUniformLocation(program, "backMaterial.emission"),1,backMaterial->emission);
	glUniform1fv( glGetUniformLocation(program, "backMaterial.shininess"),1,&(backMaterial->shininess));

	glUniform1iv( glGetUniformLocation(program, "Fog.enable"),1,&(Fog->enable));
	glUniform1fv( glGetUniformLocation(program, "Fog.density"),1,&(Fog->density));
	glUniform1fv( glGetUniformLocation(program, "Fog.end"),1,&(Fog->end));
	glUniform4fv( glGetUniformLocation(program, "Fog.color"),1,glm::value_ptr(Fog->color));
	if(Fog->pick == (FogProperties::equation)(FogProperties::FOG_EQUATION_LINEAR))
	{
		int x = 0;
		glUniform1iv( glGetUniformLocation(program, "Fog.equation"),1,&(x));
	}
	else if(Fog->pick == (FogProperties::equation)(FogProperties::FOG_EQUATION_EXP))
	{
		int x = 1;
		glUniform1iv( glGetUniformLocation(program, "Fog.equation"),1,&(x));
	}
	else
	{
		int x = 2;
		glUniform1iv( glGetUniformLocation(program, "Fog.equation"),1,&(x));
	}
	Fog->scale = 1.0f / (Fog->end - Fog->start); 
	glUniform1fv( glGetUniformLocation(program, "Fog.scale"),1,&(Fog->scale));
	
	
}

void quadSQ( int a, int b, int c, int d )
{
    //specify temporary vectors along each quad's edge in order to compute the face
    // normal using the cross product rule
    glm::vec3 u = (planeVertices[b]-planeVertices[a]).xyz();
    glm::vec3 v = (planeVertices[c]-planeVertices[b]).xyz();
    glm::vec3 norm = glm::cross(u, v);
    glm::vec3 normal= glm::normalize(norm);
    
	normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[a]; pointsq[IndexSQ] = planeVertices[a]; tex_coords[IndexSQ] = glm::vec4(0.0,1.0,0.0,0.0);IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[b]; pointsq[IndexSQ] = planeVertices[b]; tex_coords[IndexSQ] = glm::vec4(1.0,1.0,0.0,0.0);IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[c]; pointsq[IndexSQ] = planeVertices[c]; tex_coords[IndexSQ] = glm::vec4(1.0,0.0,0.0,0.0);IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[a]; pointsq[IndexSQ] = planeVertices[a]; tex_coords[IndexSQ] = glm::vec4(0.0,1.0,0.0,0.0);IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[c]; pointsq[IndexSQ] = planeVertices[c]; tex_coords[IndexSQ] = glm::vec4(1.0,0.0,0.0,0.0);IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[d]; pointsq[IndexSQ] = planeVertices[d]; tex_coords[IndexSQ] = glm::vec4(0.0,0.0,0.0,0.0);IndexSQ++;
}

void init3Dmodel()
{

    GLuint m_Buffers[4];
    m = new Mesh();
    m->loadMesh("./Models/box1/models/CargoCube01.dae");
    //m->loadMesh("../../_glGA-data/data/models/sphere.dae");
    
    glGenVertexArrays(1, &vao3Dmodel);
    glBindVertexArray(vao3Dmodel);
    
    //create the VBO
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    
    
    //Load shaders and use the resulting shader program
    program3Dmodel = LoadShaders("vshader3D.vert", "fshader3D.frag");
    glUseProgram(program3Dmodel);
    
    std::cout << "this is Position size :  " << m->Positions.size() << " and this is Normal size : " << m->Normals.size() << " and this is TexCoords size : " << m->TexCoords.size() << std::endl;
    std::cout << "this is numVertices " << m->numVertices << std::endl;
    std::cout << "this is numSamplers " << m->m_TextureSamplers.size() << std::endl;
    std::cout << "this is numTextures " << m->m_Textures.size() << std::endl;
    
    // Vertex VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m->Positions[0]) * m->Positions.size(), &m->Positions[0], GL_STATIC_DRAW);
    // Connect vertex arrays to the the shader attributes: vPosition, vNormal, vTexCoord
    GLuint vPositionMesh = glGetAttribLocation(program3Dmodel, "vPosition");
    glEnableVertexAttribArray(vPositionMesh);
    glVertexAttribPointer(vPositionMesh, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //stop using previous VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // TEXCOORD VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m->TexCoords[0]) * m->TexCoords.size(), &m->TexCoords[0], GL_STATIC_DRAW);
    GLuint vTexCoordMesh = glGetAttribLocation(program3Dmodel, "vTexCoord");
    glEnableVertexAttribArray(vTexCoordMesh);
    glVertexAttribPointer(vTexCoordMesh, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //stop using previous VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Normal VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m->Normals[0]) * m->Normals.size(), &m->Normals[0], GL_STATIC_DRAW);
    GLuint vNormalMesh = glGetAttribLocation(program3Dmodel, "vNormal");
    glEnableVertexAttribArray(vNormalMesh);
    glVertexAttribPointer(vNormalMesh, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //stop using previous VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Index VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m->Indices[0]) * m->Indices.size(), &m->Indices[0], GL_STATIC_DRAW);
    
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    
    
    glBindVertexArray(0);
}

void initPyramid()
{
	pointPyramid[0]  = Pyramid[0]; pointPyramid[1]   = Pyramid[1]; pointPyramid[2]   = Pyramid[2];
	pointPyramid[3]  = Pyramid[0]; pointPyramid[4]   = Pyramid[3]; pointPyramid[5]   = Pyramid[2];
	pointPyramid[6]  = Pyramid[0]; pointPyramid[7]   = Pyramid[4]; pointPyramid[8]   = Pyramid[3];
	pointPyramid[9]  = Pyramid[0]; pointPyramid[10]  = Pyramid[4]; pointPyramid[11]  = Pyramid[1];
	pointPyramid[12] = Pyramid[1]; pointPyramid[13]  = Pyramid[4]; pointPyramid[14]  = Pyramid[2];
	pointPyramid[15] = Pyramid[2]; pointPyramid[16]  = Pyramid[4]; pointPyramid[17]  = Pyramid[3];

	for(int i = 0;i < 18; i++)
	{
		colorPyramid[i] = PyramidsColor[0];
	}

	//generate and bind a VAO for the 3D axes
	glGenVertexArrays(1, &vaoPyramid);
    glBindVertexArray(vaoPyramid);

	// Load shaders and use the resulting shader program
    programPyramid = LoadShaders( "vPyramidShader.vert", "fPyramidShader.frag" );
    glUseProgram( programPyramid );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
	glGenBuffers( 1, &bufferPyramid);
    glBindBuffer( GL_ARRAY_BUFFER, bufferPyramid );
	glBufferData( GL_ARRAY_BUFFER, sizeof(pointPyramid) + sizeof(colorPyramid),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointPyramid), pointPyramid );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointPyramid), sizeof(colorPyramid), colorPyramid );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programPyramid, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    GLuint vColor = glGetAttribLocation( programPyramid, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(pointPyramid)) );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void initX()
{
	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoX);
    glBindVertexArray(vaoX);

	// Load shaders and use the resulting shader program
    programX = LoadShaders( "vAxisShader.vert", "fAxisShader.frag" );
    glUseProgram( programX );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferX );
    glBindBuffer( GL_ARRAY_BUFFER, bufferX );
    glBufferData( GL_ARRAY_BUFFER, sizeof(Xvertices) + sizeof(Xvertex_color),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Xvertices), Xvertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(Xvertices), sizeof(Xvertex_color), Xvertex_color );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programX, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    GLuint vColor = glGetAttribLocation( programX, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(Xvertices)) );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void initY()
{
	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoY);
    glBindVertexArray(vaoY);

	// Load shaders and use the resulting shader program
    programY = LoadShaders("vAxisShader.vert", "fAxisShader.frag" );
    glUseProgram( programY );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferY );
    glBindBuffer( GL_ARRAY_BUFFER, bufferY );
    glBufferData( GL_ARRAY_BUFFER, sizeof(Yvertices) + sizeof(Yvertex_color),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Yvertices), Yvertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(Yvertices), sizeof(Yvertex_color), Yvertex_color );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programY, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    GLuint vColor = glGetAttribLocation( programY, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(Yvertices)) );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void initZ()
{
	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoZ);
    glBindVertexArray(vaoZ);



	// Load shaders and use the resulting shader program
    programZ = LoadShaders( "vAxisShader.vert", "fAxisShader.frag" );
    glUseProgram( programZ );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferZ );
    glBindBuffer( GL_ARRAY_BUFFER, bufferZ );
    glBufferData( GL_ARRAY_BUFFER, sizeof(Zvertices) + sizeof(Zvertex_color),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Zvertices), Zvertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(Zvertices), sizeof(Zvertex_color), Zvertex_color );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programZ, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    GLuint vColor = glGetAttribLocation( programZ, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(Zvertices)) );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void initPlane()
{
	float numX =-10.0f,numX1 = -10.5f;
	float numZ = -9.5f,numZ1 = -10.0f;

	planeVertices[0] = point4 ( numX, 0.0, numZ1, 1.0);  planeColor[0] = color4 (0.603922, 0.803922, 0.196078, 1.0);
	planeVertices[1] = point4 ( numX, 0.0, numZ, 1.0);   planeColor[1] = color4 (0.603922, 0.803922, 0.196078, 1.0);
	planeVertices[2] = point4 ( numX1, 0.0, numZ, 1.0);  planeColor[2] = color4 (0.603922, 0.803922, 0.196078, 1.0);
	planeVertices[3] = point4 ( numX1, 0.0, numZ1, 1.0); planeColor[3] = color4 (0.603922, 0.803922, 0.196078, 1.0);

	int k = 4;
	int counter = 0;
	for(k=4;k<NumVerticesSQ-4;k=k+4)
	{
		numX+=0.5f; numX1+=0.5f; counter++;

		planeVertices[k]   =   point4 (numX,  0.0, numZ1, 1.0); planeColor[k]   = color4 (0.603922, 0.803922, 0.196078, 1.0);
		planeVertices[k+1]	=	point4 (numX,  0.0, numZ, 1.0); planeColor[k+1] = color4 (0.603922, 0.803922, 0.196078, 1.0);
		planeVertices[k+2]	=	point4 (numX1, 0.0, numZ, 1.0); planeColor[k+2] = color4 (0.603922, 0.803922, 0.196078, 1.0);
		planeVertices[k+3]	=   point4 (numX1, 0.0, numZ1, 1.0); planeColor[k+3] = color4 (0.603922, 0.803922, 0.196078, 1.0);

		if( counter == (NumOfSQ - 1) )
		{
			numX = -10.0f;numX1 = -10.5f;k+=4;
			counter = 0;

			numZ+=0.5f;numZ1+=0.5f;

			planeVertices[k]   =   point4 (numX,  0.0, numZ1, 1.0); planeColor[k]   = color4 (0.603922, 0.803922, 0.196078, 1.0);
			planeVertices[k+1]	=	point4 (numX,  0.0, numZ, 1.0); planeColor[k+1] = color4 (0.603922, 0.803922, 0.196078, 1.0);
			planeVertices[k+2]	=	point4 (numX1, 0.0, numZ, 1.0); planeColor[k+2] = color4 (0.603922, 0.803922, 0.196078, 1.0);
			planeVertices[k+3]	=   point4 (numX1, 0.0, numZ1, 1.0); planeColor[k+3] = color4 (0.603922, 0.803922, 0.196078, 1.0);
            cout << (k+3) << endl;
            cout << NumVerticesSQ << endl;
		}

	}

	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoPlane);
    glBindVertexArray(vaoPlane);

	pTexture = new Texture(GL_TEXTURE_2D,"./Textures/nvidia_logo.jpg");
	//pTexture = new Texture(GL_TEXTURE_2D,"./Textures/NVIDIA.jpg");

	if (!pTexture->loadTexture()) {

		exit(EXIT_FAILURE);
    }

	int lp = 0,a=1,b=0,c=3,d=2;

	for(lp = 0;lp < (NumOfSQ * NumOfSQ);lp++)
	{
		quadSQ(a,b,c,d);

		a+=4;b+=4;c+=4;d+=4;
	}

	// Load shaders and use the resulting shader program
    programPlane = LoadShaders( "vPlaneShader.vert", "fPlaneShader.frag" );
    glUseProgram( programPlane );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferPlane );
    glBindBuffer( GL_ARRAY_BUFFER, bufferPlane );
	
	glBufferData( GL_ARRAY_BUFFER, sizeof(pointsq) + sizeof(colorsq) + sizeof(normalsq) + sizeof(tex_coords),NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsq), pointsq );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsq), sizeof(colorsq), colorsq );
	glBufferSubData( GL_ARRAY_BUFFER,sizeof(pointsq) + sizeof(colorsq),sizeof(normalsq),normalsq );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsq) + sizeof(colorsq) + sizeof(normalsq) ,sizeof(tex_coords) , tex_coords );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programPlane, "MCvertex" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    GLuint vColor = glGetAttribLocation( programPlane, "vColor" ); 
    glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(pointsq)) );

	GLuint vNormal = glGetAttribLocation( programPlane, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(pointsq) + sizeof(colorsq)) );

	GLuint vText = glGetAttribLocation( programPlane, "MultiTexCoord0" );
    glEnableVertexAttribArray( vText );
	glVertexAttribPointer( vText, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(pointsq) + sizeof(colorsq) + sizeof(normalsq)) );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void checkActiveUniforms()
{
	GLint nUniforms, maxLen;
	glGetProgramiv( program3Dmodel, GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLen);
	glGetProgramiv( program3Dmodel, GL_ACTIVE_UNIFORMS,&nUniforms);
	GLchar * name = (GLchar *) malloc( maxLen );
	GLint size, location;
	GLsizei written;
	GLenum type;

	printf(" Location | Name\n");
	printf("------------------------------------------------\n");

	for( int i = 0; i < nUniforms; ++i ) {

		glGetActiveUniform( program3Dmodel, i, maxLen, &written,&size, &type, name );
		location = glGetUniformLocation(program3Dmodel, name);
		printf(" %-8d | %s\n", location, name);
	}
	free(name);
}

void display3Dmodel(glm::mat4 &tsl,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
	glUseProgram(program3Dmodel);
	glBindVertexArray(vao3Dmodel);

	glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
	MV_uniform3D = glGetUniformLocation(program3Dmodel, "MV_mat");
	MVP_uniform3D = glGetUniformLocation(program3Dmodel, "MVP_mat");
	Normal_uniform3D = glGetUniformLocation(program3Dmodel, "Normal_mat");
	MVlight = glGetUniformLocation(program3Dmodel, "MVl_mat");
	eyes_uniform = glGetUniformLocation(program3Dmodel, "eyes");

	//Calculation of ModelView Matrix
	glm::mat4 model_mat = tsl;
	glm::mat4 view_mat = glm::lookAt(positionv,positionv + directionv,upv);

	glm::mat4 Model = glm::mat4();
	glm::mat4 ModelView = view_mat * Model;
	glUniformMatrix4fv(MVlight,1, GL_FALSE, glm::value_ptr(ModelView));

	glUniformMatrix4fv(V_uniform3D,1,GL_FALSE,glm::value_ptr(view_mat));
	glm::mat4 MV_mat = view_mat * model_mat;
	glUniformMatrix4fv(MV_uniform3D,1, GL_FALSE, glm::value_ptr(MV_mat)); 

	//Calculation of Normal Matrix
	glm::mat3 Normal_mat = glm::transpose(glm::inverse(glm::mat3(MV_mat)));
	glUniformMatrix3fv(Normal_uniform3D,1, GL_FALSE, glm::value_ptr(Normal_mat));

	//Calculation of ModelViewProjection Matrix
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_mat = glm::perspective(45.0f, aspect,0.1f,100.0f);
	glm::mat4 MVP_mat = projection_mat * MV_mat;
	glUniformMatrix4fv(MVP_uniform3D, 1, GL_FALSE, glm::value_ptr(MVP_mat));

	glm::mat4 InView = glm::inverse(view_mat);
	float eyse[] = {InView[3][0],InView[3][1],InView[3][2],InView[3][3]};
	glm::vec4 eye = glm::vec4(positionv,1.0);
	glUniform4fv(eyes_uniform,1,glm::value_ptr(eye));

	gSampler1 = glGetUniformLocationARB(program3Dmodel, "gSampler1");
	glUniform1iARB(gSampler1, 0);

	LoadUniforms(program3Dmodel);

	m->render(); 
	if(timesc < 1)
		checkActiveUniforms();
	timesc++;
	glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}

void displayPyramid(glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
	glUseProgram(programPyramid);
	glBindVertexArray(vaoPyramid);

	glDisable(GL_CULL_FACE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	MV_uniformPyramid = glGetUniformLocation(programPyramid, "MV_mat");
	MVP_uniform_Pyramid = glGetUniformLocation(programPyramid, "MVP_mat");

	glUniform1fv( glGetUniformLocation(programPyramid, "Fog.density"),1,&(Fog->density));
	glUniform1fv( glGetUniformLocation(programPyramid, "Fog.start"),1,&(Fog->start));
	glUniform1fv( glGetUniformLocation(programPyramid, "Fog.end"),1,&(Fog->end));
	glUniform4fv( glGetUniformLocation(programPyramid, "Fog.color"),1,glm::value_ptr(Fog->color));
//	glUniform1iv( glGetUniformLocation(programPyramid, "Fog.equation"),1,&(Fog->equation));
	glUniform1fv( glGetUniformLocation(programPyramid, "Fog.scale"),1,&(Fog->scale));
	glUniform1iv( glGetUniformLocation(programPyramid, "Fog.enable"),1,&(Fog->enable));

	// Calculation of ModelView Matrix
	glm::mat4 model_mat_Pyramid = glm::mat4();
	glm::mat4 view_mat_Pyramid = glm::lookAt(positionv,positionv + directionv,upv);
	glm::mat4 MV_mat_Pyramid = view_mat_Pyramid * model_mat_Pyramid;
	glUniformMatrix4fv(MV_uniformPyramid,1, GL_FALSE, glm::value_ptr(MV_mat_Pyramid));

	// Calculation of ModelViewProjection Matrix
	float aspect_Pyramid = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_mat_Pyramid = glm::perspective(45.0f, aspect_Pyramid,0.1f,100.0f);
	glm::mat4 MVP_mat_Pyramid = projection_mat_Pyramid * MV_mat_Pyramid;
	glUniformMatrix4fv(MVP_uniform_Pyramid, 1, GL_FALSE, glm::value_ptr(MVP_mat_Pyramid));

    glDrawArrays( GL_TRIANGLES, 0, 18);
    
    glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);

}

void displayX(glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
	glUseProgram(programX);
	glBindVertexArray(vaoX);

	glDisable(GL_CULL_FACE);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
	MV_uniformX = glGetUniformLocation(programX, "MV_mat");
	MVP_uniformX = glGetUniformLocation(programX, "MVP_mat");

	// Calculation of ModelView Matrix
	glm::mat4 model_matx = glm::mat4();
	glm::mat4 view_matx = glm::lookAt(positionv,positionv + directionv,upv);
	glm::mat4 MV_matx = view_matx * model_matx;
	glUniformMatrix4fv(MV_uniformX,1, GL_FALSE, glm::value_ptr(MV_matx));

	// Calculation of ModelViewProjection Matrix
	float aspectx = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_matx = glm::perspective(45.0f, aspectx,0.1f,100.0f);
	glm::mat4 MVP_matx = projection_matx * MV_matx;
	glUniformMatrix4fv(MVP_uniformX, 1, GL_FALSE, glm::value_ptr(MVP_matx));

    glDrawArrays( GL_LINES, 0, NumVerticesl );
    
    glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}

void displayY(glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
	glUseProgram(programY);
	glBindVertexArray(vaoY);

	glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
	MV_uniformY = glGetUniformLocation(programY, "MV_mat");
	MVP_uniformY = glGetUniformLocation(programY, "MVP_mat");

	// Calculation of ModelView Matrix
	glm::mat4 model_maty = glm::mat4();
	glm::mat4 view_maty = glm::lookAt(positionv,positionv + directionv,upv);
	glm::mat4 MV_maty = view_maty * model_maty;
	glUniformMatrix4fv(MV_uniformY,1, GL_FALSE, glm::value_ptr(MV_maty));

	// Calculation of ModelViewProjection Matrix
	float aspecty = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_maty = glm::perspective(45.0f, aspecty,0.1f,100.0f);
	glm::mat4 MVP_maty = projection_maty * MV_maty;
	glUniformMatrix4fv(MVP_uniformY, 1, GL_FALSE, glm::value_ptr(MVP_maty));

    glDrawArrays( GL_LINES, 0, NumVerticesl );
    
    glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}

void displayZ(glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
	glUseProgram(programZ);
	glBindVertexArray(vaoZ);

	glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	MV_uniformZ = glGetUniformLocation(programZ, "MV_mat");
	MVP_uniformZ = glGetUniformLocation(programZ, "MVP_mat");

	// Calculation of ModelView Matrix
	glm::mat4 model_matz = glm::mat4();
	glm::mat4 view_matz = glm::lookAt(positionv,positionv + directionv,upv);
	glm::mat4 MV_matz = view_matz * model_matz;
	glUniformMatrix4fv(MV_uniformZ,1, GL_FALSE, glm::value_ptr(MV_matz));

	// Calculation of ModelViewProjection Matrix
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_matz = glm::perspective(45.0f, aspect,0.1f,100.0f);
	glm::mat4 MVP_matz = projection_matz * MV_matz;
	glUniformMatrix4fv(MVP_uniformZ, 1, GL_FALSE, glm::value_ptr(MVP_matz));
    
    glDrawArrays( GL_LINES, 0, NumVerticesl );
    
    glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}

void displayPlane(glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
	glUseProgram(programPlane);
	glBindVertexArray(vaoPlane);

	glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);	
	
    if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	V_uniformPlane = glGetUniformLocation(programPlane, "V_mat" );
	MV_uniformPlane = glGetUniformLocation(programPlane, "MV_mat");
	MVP_uniformPlane = glGetUniformLocation(programPlane, "MVP_mat");
	Normal_uniformPlane = glGetUniformLocation(programPlane, "Normal_mat");
	GLuint eyes_uniformPlane = glGetUniformLocation(programPlane, "eyes");
	TextureMatrix_Uniform = glGetUniformLocation(programPlane, "TextureMatrix");

	glm::mat4 TexMat = glm::mat4();
	//TexMat = glm::scale(glm::mat4(1.0),glm::vec3(-2.0,-2.0,-2.0));

	glUniformMatrix4fv(TextureMatrix_Uniform,1,GL_FALSE,glm::value_ptr(TexMat));

	// Calculation of ModelView Matrix
	glm::mat4 model_mat_plane =  glm::mat4();
	glm::mat4 view_mat_plane = glm::lookAt(positionv,positionv + directionv,upv);
	glUniformMatrix4fv(V_uniformPlane,1,GL_FALSE, glm::value_ptr(view_mat_plane));
	glm::mat4 MV_mat_plane = view_mat_plane * model_mat_plane;
	glUniformMatrix4fv(MV_uniformPlane,1, GL_FALSE, glm::value_ptr(MV_mat_plane));

	// Calculation of Normal Matrix
	glm::mat3 Normal_mat_plane = glm::transpose(glm::inverse(glm::mat3(MV_mat_plane)));
	glUniformMatrix3fv(Normal_uniformPlane,1, GL_FALSE, glm::value_ptr(Normal_mat_plane));

	glm::mat4 InView = glm::inverse(view_mat_plane);
	float eye[] = {InView[3][0],InView[3][1],InView[3][2],InView[3][3]};
	glUniformMatrix4fv(eyes_uniformPlane,1,GL_FALSE,eye);

	// Calculation of ModelViewProjection Matrix
	float aspect_plane = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_mat_plane = glm::perspective(45.0f, aspect_plane,0.1f,100.0f);
	glm::mat4 MVP_mat_plane = projection_mat_plane * MV_mat_plane;
	glUniformMatrix4fv(MVP_uniformPlane, 1, GL_FALSE, glm::value_ptr(MVP_mat_plane));

	LoadUniforms(programPlane);

	gSampler =  glGetUniformLocationARB(programPlane, "gSampler");
	glUniform1iARB(gSampler, 0);
    
	pTexture->bindTexture(GL_TEXTURE0);

    glDrawArrays( GL_TRIANGLES, 0, NumVerticesSQ );

	//GLExitIfError();
	//GLCheckError();
    
    glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}

int main (int argc, char * argv[])
{
	// Current time
	double time = 0;

	 // initialise GLFW
    int running = GL_TRUE;

	if (!initSDL())
	{
		exit(EXIT_FAILURE);
	}
    
  
    glEnable(GL_TEXTURE_2D);

	 // Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	initX();	// initialize X axis

	initY();	// initialize Y axis

	initZ();	// initialize Z axis

	initPlane(); //initialize Plane

	init3Dmodel(); // initialize 3D model

	//initPyramid();

	initLights(); // initialize Lighting of the Scene
	initMaterials(); // initialize Material properties of the Objects and the Scene
    
	initFog(); // initialize Fog Properties

	// Initialize time
    time = SDL_GetTicks();
	uint32 currentTime;
	uint32 lastTime = 0U;
    
#ifdef __APPLE__
	int *w = (int*)malloc(sizeof(int));
	int *h = (int*)malloc(sizeof(int));
#endif

	while (running) {
		glm::vec3 direction(cos(verticAngle) * sin(horizAngle), sin(verticAngle), cos(verticAngle) * cos(horizAngle));

		glm::vec3 right = glm::vec3(sin(horizAngle - 3.14f / 2.0f), 0, cos(horizAngle - 3.14f / 2.0f));

		glm::vec3 up = glm::cross(right, direction);

		currentTime =  SDL_GetTicks();
		float dTime = float(currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			event_handler(&event);

			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_UP)
					pos += direction * dTime * speedo;

				if (event.key.keysym.sym == SDLK_DOWN)
					pos -= direction * dTime * speedo;

				if (event.key.keysym.sym == SDLK_RIGHT)
					pos += right * dTime * speedo;

				if (event.key.keysym.sym == SDLK_LEFT)
					pos -= right * dTime * speedo;
			}
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                running = GL_FALSE;
            }
            if (event.type == SDL_QUIT)
				running = GL_FALSE;
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(gWindow);
		ImGui::NewFrame();
		glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        glClearColor( bgColor[0], bgColor[1], bgColor[2], bgColor[3]); //black color

		if(camera == true)
		{
			SDL_GetMouseState(&xpos, &ypos);
			SDL_WarpMouseInWindow(gWindow, windowWidth / 2, windowHeight / 2);
			//glfwGetMousePos(&xpos,&ypos);
			//glfwSetMousePos(windowWidth/2, windowHeight/2);
		
			horizAngle  += mouseSpeedo * float(windowWidth/2 - xpos );
			verticAngle += mouseSpeedo * float( windowHeight/2 - ypos );
		}


		glm::mat4 cube1 = glm::mat4();
		cube1 = glm::translate(cube1,glm::vec3(-4.0,1.0,4.0));

		glm::mat4 cube2 = glm::mat4();
		cube2 = glm::translate(cube2,glm::vec3(-4.0,1.0,-4.0));

		glm::mat4 cube3 = glm::mat4();
		cube3 = glm::translate(cube3,glm::vec3(4.0,1.0,-4.0));

		glm::mat4 cube4 = glm::mat4();
		cube4 = glm::translate(cube4,glm::vec3(4.0,1.0,4.0));


		displayX(pos,direction,up);
		displayY(pos,direction,up);
		displayZ(pos,direction,up);
		displayPlane(pos,direction,up);
        //displayPyramid(pos,direction,up);

		display3Dmodel(cube1,pos,direction,up);
		display3Dmodel(cube2,pos,direction,up);
		display3Dmodel(cube3,pos,direction,up);
		display3Dmodel(cube4,pos,direction,up);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		displayGui();

		ImGui::Render();
		SDL_GL_MakeCurrent(gWindow, gContext);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(gWindow);	
#ifdef __APPLE__
		if(w!=NULL && h!=NULL){
			SDL_GL_GetDrawableSize(gWindow, w, h);
			resize_window(*w, *h);
		}
#endif
    }

	//close OpenGL window and  terminate ImGui and SDL2
	close();
    
    exit(EXIT_SUCCESS);
    
}

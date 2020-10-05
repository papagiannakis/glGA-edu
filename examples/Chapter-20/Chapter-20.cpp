// basic STL streams
#include <iostream>
#include <sstream>
// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>

// Update 06/08/16
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
#include <glGA/glGARigMesh.h>


#if defined(__APPLE__)
#include <mach/mach_time.h>
#endif

// number of Squares for Plane
#define NumOfSQ 20

/**********************WATER DEFINES**************************/

#define WMR 128 // water mesh resolution

#define WMS 10.0f // water mesh size

#define MAXY 0.0625f // waves amplitude

#define MAX_WAVES 32

#define M_PI 3.14159265358979323846

#define WNBMTR 512 // water normal bump map texture resolution, must be >= WMR

int mtime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (int)(tv.tv_sec*1000 + (tv.tv_usec / 1000));
}


class CWave
{
public:
    float StartTime, Speed, MaxY, FrequencyMPIM2;
    glm::vec2 Position;
    
public:
    CWave();
    ~CWave();
};

float WaterLevel = 0.5f;

CWave Waves[MAX_WAVES];

int Wave = 0;
int WaterVerticesCount;

GLuint programWater,programBumpMap,programNormalMap;
GLuint vaoWater,vaoBumpMap,vaoNormalMap;
GLuint bufferWater,bufferBumpMap,bufferNormalMap;

GLuint WaterNormalBumpMapTexture;
GLuint ReflectionTexture,RefractionTexture,DepthTexture;

GLuint MVP_uniformWater;

GLuint FBO;

glm::vec3 directionIntrs,upIntrs;

glm::mat4 BiasMatrixInverse = glm::mat4(2.0f,  0.0f,  0.0f, 0.0f,
                                        0.0f,  2.0f,  0.0f, 0.0f,
                                        0.0f,  0.0f,  2.0f, 0.0f,
                                        -1.0f, -1.0f, -1.0f, 1.0f);

glm::mat4 ProjectionBiasMatrixInverse;

glm::vec4 QuadVertices[] = {
    glm::vec4(-1.0f, -1.0f, 0.0f,1.0f),  // a
    glm::vec4( 1.0f, -1.0f, 0.0f,1.0f),  // b
    glm::vec4( 1.0f,  1.0f, 0.0f,1.0f),  // c
    glm::vec4(-1.0f, -1.0f, 0.0f,1.0f),  // a
    glm::vec4( 1.0f,  1.0f, 0.0f,1.0f),  // c
    glm::vec4(-1.0f,  1.0f, 0.0f,1.0f)   // d
};


struct Lighting
{
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec4 position;
    
    GLfloat constantAttenuation;
    GLfloat linearAttenuation;
    GLfloat quadraticAttenuation;
};

Lighting *SceneLight = NULL;

/******************************END****************************/
// update globals
SDL_Window		*gWindow = NULL;
SDL_GLContext	gContext;
float bgColor[] = { 0.0f, 0.0f, 0.0f, 0.1f };
double FPS;

// global variables
int         windowWidth=1024, windowHeight=768;

GLuint      program3Dmodel;
GLuint      vao3Dmodel;
GLuint      buffer3Dmodel;

GLuint		M_uniform3D      , MVP_uniform3D     , Normal_uniform3D;

GLuint		TextureMatrix_Uniform;
int			timesc = 0;
GLuint		sample;

Texture	    *pTexture		= NULL;
Mesh		*m				= NULL;

const		int			NumVerticesSQ = ( (NumOfSQ) * (NumOfSQ)) * (2) * (3);
const       int         NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

bool        wireFrame = false;
bool		camera    = false;

bool		SYNC	  = true;

typedef     glm::vec4   color4;
typedef     glm::vec4   point4;

int			Index = 0,IndexSQ1 = 0,IndexCube = 0;

//Modelling arrays
point4      points[NumVertices];
glm::vec2	tex_coords[NumVertices];

glm::vec3 pos = glm::vec3( 0.0f, 1.5f , 5.0f );

float Scale = 0.2f;
float Time  = 0.0f;

float angle = 0.0f;

float horizAngle = 3.14f;
float verticAngle = 0.0f;


float speedo = 3.0f;
float mouseSpeedo = 0.005f;

int	  xpos = 0,ypos = 0;

float zNear;
float zFar;
float FOV;
float initialFoV = 45.0f;

// Scene orientation (stored as a quaternion)
float Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };

//Plane
point4		planeVertices[NumVerticesSQ];
color4		planeColor[NumVerticesSQ];

// Update function prototypes
bool			initSDL();
bool			event_handler(SDL_Event* event);
void			close();
void			resize_window(int width, int height);
bool			initImGui();
void			displayGui();

void setWaveInShader(int Wave);
void addWaveOnClick();
void initWater();
void displayWater(glm::mat4 &md,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv);
void initBumpMap();
void displayBumpMap();
void initNormalMap();
void displayNormalMap();

// Update functions 
bool			initSDL()
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
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		//SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);


		//Create Window
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

		gWindow = SDL_CreateWindow("Chapter20", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
				return true;

		if (event->button.button == SDL_BUTTON_RIGHT)
				return true;

		if (event->button.button == SDL_BUTTON_MIDDLE)
				return true;
	}
	case SDL_MOUSEBUTTONUP:
	{
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			std::cout << "\nLeft Mouse Button pressed!\n";
			addWaveOnClick();
			return true;
		}

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
				SDL_GetMouseState(&xpos, &ypos);
				SDL_WarpMouseInWindow(gWindow, windowWidth / 2, windowHeight / 2);
				SDL_ShowCursor(0);
			}
			else
			{
				camera = false;
				SDL_GetMouseState(&xpos, &ypos);
				SDL_WarpMouseInWindow(gWindow, windowWidth / 2, windowHeight / 2);
				SDL_ShowCursor(1);
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

	float aspect = (GLfloat)width / (GLfloat)height;

	windowWidth = width;
	windowHeight = height;

	glBindTexture(GL_TEXTURE_2D, ReflectionTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, RefractionTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, DepthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
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

void			displayGui(){
	ImGui::Begin("Main Editor");
	ImGui::SetWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);
	ImGui::SetWindowPos(ImVec2(10, 10), ImGuiSetCond_Once);

	static bool checkbox = false;
	if (ImGui::Checkbox("Wireframe", &checkbox))
	{
		if (checkbox)
			wireFrame = true;
		else
			wireFrame = false;
	}
	ImGui::Separator();
	ImGui::ColorEdit3("bgColor", bgColor);
	ImGui::Separator();
	if (ImGui::TreeNode("Scene Rotation"))
	{
		ImGui::InputFloat4("SceneRotation", (float*)&Rotation, 2);
		ImGui::Separator();
		ImGui::SliderFloat("X", (float*)&Rotation[0], -1.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Y", (float*)&Rotation[1], -1.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Z", (float*)&Rotation[2], -1.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("W", (float*)&Rotation[3], -1.0f, 1.0f, "%.2f");
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::Button("Reset View", ImVec2(100, 20)))
	{
		Rotation[0] = 0.0f; Rotation[1] = 0.0f; Rotation[2] = 0.0f; Rotation[3] = 1.0f;

		pos = glm::vec3(5.0f, 3.0f, 18.0f);

		zNear = 0.1f;
		zFar = 100.0f;
		FOV = 45.0f;

		horizAngle = 3.14f;
		verticAngle = 0.0f;

	}
	ImGui::Separator();
	if (ImGui::TreeNode("Projection Properties"))
	{
		ImGui::SliderFloat("Near Clip Plane", &zNear, 0.5, 100.0, "%.2f");
		ImGui::Separator();

		ImGui::SliderFloat("Far Clip Plane", &zFar, 0.5, 1000.0, "%.2f");
		ImGui::Separator();

		ImGui::SliderFloat("Field of View", &FOV, 0.0f, 100.0f, "%.2f");

		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Frame Rate"))
	{
		ImGui::BulletText("MS per 1 Frame %.2f", FPS);
		ImGui::NewLine();
		ImGui::BulletText("Frames Per Second %.2f", ImGui::GetIO().Framerate);
		ImGui::NewLine();
		ImGui::BulletText("vSYNC %d", SYNC);
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Light Direction"))
	{
		ImGui::SliderFloat3("Direction", (float*)&(SceneLight->position), -10.0f, 10.0f, "%.2f");
		ImGui::TreePop();
	}
	ImGui::End();
}

#if !defined(__APPLE__)
/*void setVSync(bool sync)
{
    // Function pointer for the wgl extention function we need to enable/disable
    // vsync
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
    PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
    
    //const char *extensions = (char*)glGetString( GL_EXTENSIONS );
    
    //if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
    //if(glewIsSupported("WGL_EXT_swap_control"))
    //{
    //	std::cout<<"\nWGL_EXT_swap_control Extension is not supported.\n";
    //	return;
    //}
    //else
    //{
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
    
    if( wglSwapIntervalEXT )
        wglSwapIntervalEXT(sync);
    
    std::cout<<"\nDONE :: "<<sync<<"\n";
    //}
}*/
#endif


CWave::CWave()
{
    Speed = 1.0f;
    MaxY = 0.03125f;
    FrequencyMPIM2 = 4.0f * (float)M_PI * 2.0;
}

CWave::~CWave()
{
}

// Routine to convert a quaternion to a 4x4 matrix
glm::mat4 ConvertQuaternionToMatrix(float *quat,glm::mat4 &mat)
{
    float yy2 = 2.0f * quat[1] * quat[1];
    float xy2 = 2.0f * quat[0] * quat[1];
    float xz2 = 2.0f * quat[0] * quat[2];
    float yz2 = 2.0f * quat[1] * quat[2];
    float zz2 = 2.0f * quat[2] * quat[2];
    float wz2 = 2.0f * quat[3] * quat[2];
    float wy2 = 2.0f * quat[3] * quat[1];
    float wx2 = 2.0f * quat[3] * quat[0];
    float xx2 = 2.0f * quat[0] * quat[0];
    mat[0][0] = - yy2 - zz2 + 1.0f;
    mat[0][1] = xy2 + wz2;
    mat[0][2] = xz2 - wy2;
    mat[0][3] = 0;
    mat[1][0] = xy2 - wz2;
    mat[1][1] = - xx2 - zz2 + 1.0f;
    mat[1][2] = yz2 + wx2;
    mat[1][3] = 0;
    mat[2][0] = xz2 + wy2;
    mat[2][1] = yz2 - wx2;
    mat[2][2] = - xx2 - yy2 + 1.0f;
    mat[2][3] = 0;
    mat[3][0] = mat[3][1] = mat[3][2] = 0;
    mat[3][3] = 1;
    
    return mat;
}

void checkActiveUniforms()
{
    return;
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

void addWaveOnClick()
{
    int xpos, ypos;
    SDL_GetMouseState(&xpos, &ypos);
    
    float s = (float)xpos / (float)(windowWidth - 1);
    float t = 1.0f - (float)ypos / (float)(windowHeight - 1);
    
    glm::mat4 ViewMatrix = glm::lookAt(pos,pos + directionIntrs,upIntrs);
    
    float aspect_Intrs = (GLfloat)windowWidth / (GLfloat)windowHeight;
    glm::mat4 ProjectionMatrix = glm::perspective(FOV, aspect_Intrs,zNear,zFar);
    
    ProjectionBiasMatrixInverse = glm::inverse(ProjectionMatrix) * BiasMatrixInverse;
    
    glm::vec4 Position = glm::inverse(ViewMatrix) * (ProjectionBiasMatrixInverse * glm::vec4(s, t, 0.5f, 1.0f));
    Position /= Position.w;
    
    glm::vec3 Ray = glm::normalize(*(glm::vec3*)&Position - pos);
    
    glm::vec3 Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    float D = -glm::dot(Normal, glm::vec3(0.0f, WaterLevel, 0.0f));
    
    float NdotR = -glm::dot(Normal, Ray);
    
    if(NdotR != 0.0f)
    {
        float Distance = (glm::dot(Normal, pos) + D) / NdotR;
        
        if(Distance > 0.0)
        {
            glm::vec3 Position = Ray * Distance + pos;
            
            float WMSD2 = WMS / 2.0f;
            
            if(Position.x >= -WMSD2 && Position.x <= WMSD2 && Position.z >= -WMSD2 && Position.z <= WMSD2)
            {
                Waves[Wave].Position = glm::vec2(Position.x, Position.z);
                
#if defined(__APPLE__)
                Waves[Wave].StartTime = (float)mtime() * 0.001f;
#else
                Waves[Wave].StartTime = (float)GetTickCount() * 0.001f;
#endif
                Waves[Wave].MaxY = MAXY;
                
                setWaveInShader(Wave++);
                
                Wave %= MAX_WAVES;
            }
        }
    }
}

/*void addWaveOnClick()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
    
    float s = (float)x / (float)(windowWidth - 1);
    float t = 1.0f - (float)y / (float)(windowHeight - 1);
    
    glm::mat4 ViewMatrix = glm::lookAt(pos,pos + directionIntrs,upIntrs);
    
    float aspect_Intrs = (GLfloat)windowWidth / (GLfloat)windowHeight;
    glm::mat4 ProjectionMatrix = glm::perspective(FOV, aspect_Intrs,zNear,zFar);
    
    ProjectionBiasMatrixInverse = glm::inverse(ProjectionMatrix) * BiasMatrixInverse;
    
    glm::vec4 Position = glm::inverse(ViewMatrix) * (ProjectionBiasMatrixInverse * glm::vec4(s, t, 0.5f, 1.0f));
    Position /= Position.w;
    
    glm::vec3 Ray = glm::normalize(*(glm::vec3*)&Position - pos);
    
    glm::vec3 Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    float D = -glm::dot(Normal, glm::vec3(0.0f, WaterLevel, 0.0f));
    
    float NdotR = -glm::dot(Normal, Ray);
    
    if(NdotR != 0.0f)
    {
        float Distance = (glm::dot(Normal, pos) + D) / NdotR;
        
        if(Distance > 0.0)
        {
            glm::vec3 Position = Ray * Distance + pos;
            
            float WMSD2 = WMS / 2.0f;
            
            if(Position.x >= -WMSD2 && Position.x <= WMSD2 && Position.z >= -WMSD2 && Position.z <= WMSD2)
            {
                Waves[Wave].Position = glm::vec2(Position.x, Position.z);
                
#if defined(__APPLE__)
                Waves[Wave].StartTime = (float)mtime() * 0.001f;
#else
                Waves[Wave].StartTime = (float)SDL_GetTicks() / 1000.0f;
#endif
                Waves[Wave].MaxY = MAXY;
                
                setWaveInShader(Wave++);
                
                Wave %= MAX_WAVES;
            }
        }
    }
}*/

void setWaveInShader(int Wave)
{
    std::cout<<"\nWave setted!\n";
    
    std::string strONE    = "", strTWO = "", strTHREE = "", strFOUR = "", strFIVE = "";
    std::string strWa     = "Waves[";
    std::string strWSTime = "].StartTime";
    std::string strWSpeed = "].Speed";
    std::string strWMaxY  = "].MaxY";
    std::string strWfrency= "].FrequencyMPIM2";
    std::string stwWPos   = "].Position";
    
    std::string strNum = static_cast<std::ostringstream*>( &(std::ostringstream() << Wave) )->str();
    strONE	 += strWa;
    strONE	 += strNum;
    strONE	 += strWSTime;
    
    strTWO	 += strWa;
    strTWO	 += strNum;
    strTWO	 += strWSpeed;
    
    strTHREE += strWa;
    strTHREE += strNum;
    strTHREE += strWMaxY;
    
    strFOUR += strWa;
    strFOUR += strNum;
    strFOUR += strWfrency;
    
    strFIVE += strWa;
    strFIVE += strNum;
    strFIVE += stwWPos;
    
    const GLchar *one   = strONE.c_str();
    const GLchar *two   = strTWO.c_str();
    const GLchar *three = strTHREE.c_str();
    const GLchar *four  = strFOUR.c_str();
    const GLchar *five  = strFIVE.c_str();
    
    glUseProgram(programBumpMap);
    glUniform1f(glGetUniformLocation(programBumpMap,one), Waves[Wave].StartTime);
    glUniform1f(glGetUniformLocation(programBumpMap,two), Waves[Wave].Speed);
    glUniform1f(glGetUniformLocation(programBumpMap,three), Waves[Wave].MaxY);
    glUniform1f(glGetUniformLocation(programBumpMap,four), Waves[Wave].FrequencyMPIM2);
    glUniform2fv(glGetUniformLocation(programBumpMap,five), 1,glm::value_ptr(Waves[Wave].Position));
    glUseProgram(0);
}

void initWater()
{
    WaterVerticesCount = WMR * WMR * 6;
    
    glm::vec2 *WaterVertices = new glm::vec2[WaterVerticesCount];
    
    float MWMSD2 = -WMS / 2.0f, WMSDWMR = WMS / (float)WMR;
    
    int i = 0;
    
    for(int y = 0; y < WMR; y++)
    {
        for(int x = 0; x < WMR; x++)
        {
            WaterVertices[i].x = MWMSD2 + x * WMSDWMR;				// a
            WaterVertices[i++].y = MWMSD2 + y * WMSDWMR;
            
            WaterVertices[i].x = MWMSD2 + (x + 1) * WMSDWMR;		// b
            WaterVertices[i++].y = MWMSD2 + y * WMSDWMR;
            
            WaterVertices[i].x = MWMSD2 + (x + 1) * WMSDWMR;		// c
            WaterVertices[i++].y = MWMSD2 + (y + 1) * WMSDWMR;
            
            
            WaterVertices[i].x = MWMSD2 + x * WMSDWMR;				// a
            WaterVertices[i++].y = MWMSD2 + y * WMSDWMR;
            
            WaterVertices[i].x = MWMSD2 + (x + 1) * WMSDWMR;		// c
            WaterVertices[i++].y = MWMSD2 + (y + 1) * WMSDWMR;
            
            WaterVertices[i].x = MWMSD2 + x * WMSDWMR;				// d
            WaterVertices[i++].y = MWMSD2 + (y + 1) * WMSDWMR;
        }
    }
    
    glGenVertexArrays(1, &vaoWater);
    glBindVertexArray(vaoWater);
    
    //Load shaders and use the resulting shader program
    programWater = LoadShaders( "./Shaders/vshaderWater.vert", "./Shaders/fshaderWater.frag" );
    
    glUseProgram( programWater );
    
    glGenBuffers( 1, &bufferWater );
    glBindBuffer( GL_ARRAY_BUFFER, bufferWater );
    glBufferData( GL_ARRAY_BUFFER, (WaterVerticesCount * 2 * 4),WaterVertices, GL_STATIC_DRAW );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programWater, "MCVertex" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    glUniform1f(glGetUniformLocation(programWater, "ODWMS"), 1.0f / WMS);
    
    glUniform4fv(glGetUniformLocation(programWater, "SceneLight.specular"),1,glm::value_ptr(SceneLight->specular));
    
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    delete [] WaterVertices;
}


void displayWater(glm::mat4 &md,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, WaterNormalBumpMapTexture);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ReflectionTexture);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, RefractionTexture);
    
    glUseProgram(programWater);
    glBindVertexArray(vaoWater);
    
    glEnable(GL_DEPTH_TEST);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    if (wireFrame)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    
    MVP_uniformWater = glGetUniformLocation(programWater, "MVPMatrix");
    
    // Calculation of ModelView Matrix
    glm::mat4 model_mat_water =  glm::mat4();
    glm::mat4 view_mat_water = glm::lookAt(positionv,positionv + directionv,upv);
    glm::mat4 MV_mat_water = view_mat_water * model_mat_water;
    
    // Calculation of ModelViewProjection Matrix
    float aspect_water = (GLfloat)windowWidth / (GLfloat)windowHeight;
    glm::mat4 projection_mat_water = glm::perspective(FOV, aspect_water,zNear,zFar);
    glm::mat4 MVP_mat_water = projection_mat_water * MV_mat_water;
    glUniformMatrix4fv(MVP_uniformWater, 1, GL_FALSE, glm::value_ptr(MVP_mat_water));
    
    glUniform1i(glGetUniformLocation(programWater, "WaterNormalBumpMapTexture"), 0);
    glUniform1i(glGetUniformLocation(programWater, "ReflectionTexture"), 1);
    glUniform1i(glGetUniformLocation(programWater, "RefractionTexture"), 2);
    
    glUniform1f(glGetUniformLocation(programWater, "WaterLevel"),WaterLevel);
    glUniform3fv(glGetUniformLocation(programWater, "CameraPosition"),1,glm::value_ptr(pos));
    glUniformMatrix4fv(glGetUniformLocation(programWater, "ViewProjectionBiasInverse"), 1, GL_FALSE, glm::value_ptr(glm::inverse(view_mat_water) * ProjectionBiasMatrixInverse));
    
    glUniform4fv(glGetUniformLocation(programWater, "SceneLight.position"),1,glm::value_ptr(SceneLight->position));
    glUniform4fv(glGetUniformLocation(programWater, "SceneLight.ambient"),1,glm::value_ptr(SceneLight->ambient));
    glUniform4fv(glGetUniformLocation(programWater, "SceneLight.diffuse"),1,glm::value_ptr(SceneLight->diffuse));
    glUniform1f(glGetUniformLocation(programWater, "SceneLight.constantAttenuation"),SceneLight->constantAttenuation);
    glUniform1f(glGetUniformLocation(programWater, "SceneLight.linearAttenuation"),SceneLight->linearAttenuation);
    glUniform1f(glGetUniformLocation(programWater, "SceneLight.quadraticAttenuation"),SceneLight->quadraticAttenuation);
    
    glDrawArrays( GL_TRIANGLES, 0, WaterVerticesCount);
    
    glDisable(GL_DEPTH_TEST);
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void initBumpMap()
{
    //generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoBumpMap);
    glBindVertexArray(vaoBumpMap);
    
    // Load shaders and use the resulting shader program
    programBumpMap = LoadShaders( "./Shaders/vshaderBump.vert", "./Shaders/fshaderBump.frag" );
    glUseProgram( programBumpMap );
    
    // Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferBumpMap );
    glBindBuffer( GL_ARRAY_BUFFER, bufferBumpMap );
    glBufferData( GL_ARRAY_BUFFER, sizeof(QuadVertices),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(QuadVertices), QuadVertices );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programBumpMap, "MCvertex" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    std::string strONE    = "", strTWO = "", strTHREE = "", strFOUR = "", strFIVE = "";
    std::string strWa     = "Waves[";
    std::string strWSTime = "].StartTime";
    std::string strWSpeed = "].Speed";
    std::string strWMaxY  = "].MaxY";
    std::string strWfrency= "].FrequencyMPIM2";
    std::string stwWPos   = "].Position";
    
    for(int Wave = 0;Wave < MAX_WAVES; Wave++)
    {
        std::string strNum = static_cast<std::ostringstream*>( &(std::ostringstream() << Wave) )->str();
        strONE	 += strWa;
        strONE	 += strNum;
        strONE	 += strWSTime;
        
        strTWO	 += strWa;
        strTWO	 += strNum;
        strTWO	 += strWSpeed;
        
        strTHREE += strWa;
        strTHREE += strNum;
        strTHREE += strWMaxY;
        
        strFOUR += strWa;
        strFOUR += strNum;
        strFOUR += strWfrency;
        
        strFIVE += strWa;
        strFIVE += strNum;
        strFIVE += stwWPos;
        
        const GLchar *one   = strONE.c_str();
        const GLchar *two   = strTWO.c_str();
        const GLchar *three = strTHREE.c_str();
        const GLchar *four  = strFOUR.c_str();
        const GLchar *five  = strFIVE.c_str();
        
        glUniform1f(glGetUniformLocation(programBumpMap,one), Waves[Wave].StartTime);
        glUniform1f(glGetUniformLocation(programBumpMap,two), Waves[Wave].Speed);
        glUniform1f(glGetUniformLocation(programBumpMap,three), Waves[Wave].MaxY);
        glUniform1f(glGetUniformLocation(programBumpMap,four), Waves[Wave].FrequencyMPIM2);
        glUniform2fv(glGetUniformLocation(programBumpMap,five), 1,glm::value_ptr(Waves[Wave].Position));
        
        strONE   = "";
        strTWO   = "";
        strTHREE = "";
        strFOUR  = "";
        strFIVE  = "";
    }
    
    glUniform1f(glGetUniformLocation(programBumpMap, "WMS"), WMS);				// Water Mesh Size
    glUniform1f(glGetUniformLocation(programBumpMap, "WMSD2"), WMS / 2.0f);		// Water Mesh Size Divided by 2.0f
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void displayBumpMap()
{
    glUseProgram(programBumpMap);
    glBindVertexArray(vaoBumpMap);
    
    glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
#if defined(__APPLE__)
    glUniform1f(glGetUniformLocation(programBumpMap,"Time"),(float)mtime() * 0.001f);
#else
    glUniform1f(glGetUniformLocation(programBumpMap,"Time"),(float)GetTickCount() * 0.001f);
#endif
    
 
    glDrawArrays( GL_TRIANGLES, 0, 6);
    
    glPopAttrib();
    glBindVertexArray(0);
    //glUseProgram(0);
}

void initNormalMap()
{
    //generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoNormalMap);
    glBindVertexArray(vaoNormalMap);
    
    glGenTextures(1, &WaterNormalBumpMapTexture);
    
    glBindTexture(GL_TEXTURE_2D, WaterNormalBumpMapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, WNBMTR, WNBMTR, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Load shaders and use the resulting shader program
    programNormalMap = LoadShaders( "./Shaders/vshaderNormal.vert", "./Shaders/fshaderNormal.frag" );
    glUseProgram( programNormalMap );
    
    // Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferNormalMap );
    glBindBuffer( GL_ARRAY_BUFFER, bufferNormalMap );
    glBufferData( GL_ARRAY_BUFFER, sizeof(QuadVertices),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(QuadVertices), QuadVertices );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programNormalMap, "MCvertex" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    glUniform1f(glGetUniformLocation(programNormalMap, "ODWNBMTR"), 1.0f / WNBMTR);
    glUniform1f(glGetUniformLocation(programNormalMap, "WMSDWNBMTRM2"), WMS / WNBMTR * 2.0f);
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void displayNormalMap()
{
    glBindTexture(GL_TEXTURE_2D, WaterNormalBumpMapTexture);
    glUseProgram(programNormalMap);
    glBindVertexArray(vaoNormalMap);
    
    glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    glDrawArrays( GL_TRIANGLES, 0, 6);
    
    glPopAttrib();
    glBindVertexArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void init3Dmodel()
{
    
    glGenVertexArrays(1, &vao3Dmodel);
    glBindVertexArray(vao3Dmodel);
    GLuint m_Buffers[4];
    m = new Mesh();
    //m->loadMesh("./Models/Cruisership2012/Cruiser 2012.dobj");
    
    //Load shaders and use the resulting shader program
    program3Dmodel = LoadShaders( "./Shaders/vshader3D.vert", "./Shaders/fshader3D.frag" );
    
    glUseProgram( program3Dmodel );
    
    std::cout<<"this is Position size :  "<<m->Positions.size()<<" and this is Normal size : "<<m->Normals.size()<<" and this is TexCoords size : "<<m->TexCoords.size()<<std::endl;
    std::cout<<"this is numVertices "<<m->numVertices<<std::endl;
    std::cout<<"this is numSamplers "<<m->m_TextureSamplers.size()<<std::endl;
    std::cout<<"this is numTextures "<<m->m_Textures.size()<<std::endl;
    
    // Create and initialize a buffer object on the server side (GPU)
    //create the VBO
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    
    // Vertex VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m->Positions[0]) * m->Positions.size(), &m->Positions[0], GL_STATIC_DRAW);
    // Connect vertex arrays to the the shader attributes: vPosition, vNormal, vTexCoord
    GLuint vPositionMesh = glGetAttribLocation(program3Dmodel, "MCVertex");
    glEnableVertexAttribArray(vPositionMesh);
    glVertexAttribPointer(vPositionMesh, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //stop using previous VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // TEXCOORD VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m->TexCoords[0]) * m->TexCoords.size(), &m->TexCoords[0], GL_STATIC_DRAW);
    GLuint vTexCoordMesh = glGetAttribLocation(program3Dmodel, "TexCoord0");
    glEnableVertexAttribArray(vTexCoordMesh);
    glVertexAttribPointer(vTexCoordMesh, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //stop using previous VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Normal VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m->Normals[0]) * m->Normals.size(), &m->Normals[0], GL_STATIC_DRAW);
    GLuint vNormalMesh = glGetAttribLocation(program3Dmodel, "MCNormal");
    glEnableVertexAttribArray(vNormalMesh);
    glVertexAttribPointer(vNormalMesh, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //stop using previous VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Index VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m->Indices[0]) * m->Indices.size(), &m->Indices[0], GL_STATIC_DRAW);
    
    glUniform1i(glGetUniformLocation(program3Dmodel, "Texture"), 0);
    glUniform1i(glGetUniformLocation(program3Dmodel, "WaterNormalBumpMapTexture"), 1);
    glUniform1f(glGetUniformLocation(program3Dmodel, "ODWMS"), 1.0f / WMS);
    
    glUniform4fv(glGetUniformLocation(program3Dmodel, "SceneLight.position"),1,glm::value_ptr(SceneLight->position));
    
    glUniform4fv(glGetUniformLocation(program3Dmodel, "SceneLight.ambient"),1,glm::value_ptr(SceneLight->ambient));
    glUniform4fv(glGetUniformLocation(program3Dmodel, "SceneLight.diffuse"),1,glm::value_ptr(SceneLight->diffuse));
    glUniform4fv(glGetUniformLocation(program3Dmodel, "SceneLight.specular"),1,glm::value_ptr(SceneLight->specular));
    
    glUniform1f(glGetUniformLocation(program3Dmodel, "SceneLight.constantAttenuation"), SceneLight->constantAttenuation);
    glUniform1f(glGetUniformLocation(program3Dmodel, "SceneLight.linearAttenuation"), SceneLight->linearAttenuation);
    glUniform1f(glGetUniformLocation(program3Dmodel, "SceneLight.quadraticAttenuation"), SceneLight->quadraticAttenuation);
    
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void initSceneLighting()
{
    SceneLight = new Lighting();
    
    SceneLight->position = glm::vec4(-1.88f , 3.74f,  4.18f, 1.0f);
    
    SceneLight->ambient  = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    SceneLight->diffuse  = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
    SceneLight->specular = glm::vec4(1.0f , 1.0f , 1.0f , 1.0f);
    
    SceneLight->constantAttenuation  = 1.0f;
    SceneLight->linearAttenuation    = 1.0f / 128.0f;
    SceneLight->quadraticAttenuation = 1.0f / 256.0f;
}

int main (int argc, char * argv[])
{   
    glm::mat4 mat;
    
    double starting = 0.0;
    double ending   = 0.0;
    int rate = 0;
    int fr = 0;
    
    zNear = 0.1f;
    zFar  = 100.0f;
    FOV   = 45.0f;
    
    // Current time
    double time = 0;
    
    initSceneLighting();
    
    // initialise GLFW
    int running = GL_TRUE;
    
    if (!initSDL()) {
        exit(EXIT_FAILURE);
    }
    
    
    std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
    std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
    std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
    
    std::ostringstream stream1,stream2;
    
    stream1 << glGetString(GL_VENDOR);
    stream2 << glGetString(GL_RENDERER);
    
    std::string vendor ="Title : Chapter-20   Vendor : " + stream1.str() + "   Renderer : " +stream2.str();
    
    const char *tit = vendor.c_str();
	SDL_SetWindowTitle(gWindow, tit);
    
    
    glGenTextures(1, &ReflectionTexture);
    glGenTextures(1, &RefractionTexture);
    glGenTextures(1, &DepthTexture);
    
    glBindTexture(GL_TEXTURE_2D, ReflectionTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindTexture(GL_TEXTURE_2D, RefractionTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    initWater();
    
   initBumpMap();
    
     initNormalMap();
    
    glGenFramebuffersEXT(1, &FBO);
    
    GLfloat rat = 0.001f;
    
    if(SYNC == false)
    {
        rat = 0.001f;
    }
    else
    {
        rat = 0.01f;
    }
    
    // Initialize time
    time = SDL_GetTicks() / 1000.0f;
    uint32 currentTime;
    uint32 lastTime = 0U;
    
    int Frames = 0;
	double LT = SDL_GetTicks() / 1000.0f;
	starting = SDL_GetTicks() / 1000.0f;
    
#if !defined(__APPLE__)
    if(SDL_GL_SetSwapInterval(1) == 0)
    	SYNC = true;
    //setVSync(SYNC);
#endif

	FOV = initialFoV;;
    
    while (running) {
		glm::vec3 direction(cos(verticAngle) * sin(horizAngle), sin(verticAngle), cos(verticAngle) * cos(horizAngle));

		directionIntrs = direction;

		glm::vec3 right = glm::vec3(sin(horizAngle - 3.14f / 2.0f), 0, cos(horizAngle - 3.14f / 2.0f));

		glm::vec3 up = glm::cross(right, direction);

		currentTime = SDL_GetTicks();
		float dTime = float(currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_Impl_ProcessEvent(&event);
			event_handler(&event);
			
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_UP)
					pos += direction * dTime * speedo;

				if (event.key.keysym.sym == SDLK_DOWN)
					pos -= direction *  dTime * speedo;

				if (event.key.keysym.sym == SDLK_RIGHT)
					pos += right *  dTime * speedo;

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
        glViewport(0, 0, WNBMTR, WNBMTR);
        
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, WaterNormalBumpMapTexture, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
        
        displayBumpMap();
        displayNormalMap();
        
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        
        if(camera == true)
        {
			SDL_GetMouseState(&xpos, &ypos);
			SDL_WarpMouseInWindow(gWindow, windowWidth / 2, windowHeight / 2);
            
            horizAngle  += mouseSpeedo * float(windowWidth/2 - xpos );
            verticAngle += mouseSpeedo * float( windowHeight/2 - ypos );
        }     

        upIntrs = up;		
        
        
        mat = ConvertQuaternionToMatrix(Rotation, mat);
        
        glm::mat4 boattr = glm::translate(glm::mat4(),glm::vec3(0.0,0.4,2.2));
        
        glm::mat4 scaledown = glm::scale(mat,glm::vec3(0.007,0.007,0.007));
        
        scaledown = glm::rotate(scaledown,45.0f,glm::vec3(0.0,1.0,0.0));
        
        boattr *= scaledown;
        
        glViewport(0, 0, windowWidth, windowHeight);
        
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, ReflectionTexture, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, DepthTexture, 0);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 mat1, mat2;
      //  display3Dmodel(boattr,mat1,pos,direction,up,1);
        
        glm::mat4 Nboattr  = glm::scale(boattr,glm::vec3(1.0, -1.0, 1.0));
        
        glCullFace(GL_FRONT);
        
        //		display3Dmodel(boattr,Nboattr,pos,direction,up,1);
        
        glCullFace(GL_BACK);
        
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        
        // render scene / refraction texture --------------------------------------------------------------------------------------
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]); //missing
        
        //display3Dmodel(boattr,mat2,pos,direction,up,0);
        
        glBindTexture(GL_TEXTURE_2D, RefractionTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, windowWidth, windowHeight);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        displayWater(mat,pos,direction,up);
        
        fr++;
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		displayGui();
        
		ImGui::Render();
        SDL_GL_MakeCurrent(gWindow, gContext);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(gWindow);
    }
    
    //close OpenGL window and  terminate ImGui and SDL2
	close();
    
    
    exit(EXIT_SUCCESS);
}

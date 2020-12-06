//not running on MacOSX, Linux

// basic STL streams
#include <iostream>
#include <sstream>
// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>

// Here we decide which of the two versions we want to use
// If your systems supports both, choose to uncomment USE_OPENGL32
// otherwise choose to uncomment USE_OPENGL21
// GLView cna also help you decide before running this program:
//
// FOR MACOSX only, please use OPENGL32 for AntTweakBar to work properly
//
#define USE_OPENGL32

// Update 05/08/16
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>

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
#define NumOfSQ 20

// update globals
SDL_Window		*gWindow = NULL;
SDL_GLContext	gContext;
float bgColor[] = { 0.0f, 0.0f, 0.0f, 0.1f };
double FPS = 0;
int rate = 0;


// global variables
int         windowWidth = 1024 , windowHeight = 768;
int			winMapWd    = 300  , winMapHt     = 300;	  

GLuint      programPlane, program3Dmodel,programQuad , programDepth;
GLuint      vaoPlane, vao3Dmodel,vaoQuad;
GLuint      bufferPlane,buffer3Dmodel,bufferQuad;

GLuint		MVP_uniformQuad;
GLuint		View_uniformPlane , MVP_uniformPlane   , Model_uniformPlane, LightPos_uniformPlane, ShadowMatrixPlane;
GLuint		View_uniform3D    , MVP_uniform3D      , Model_uniform3D   , LightPos_uniform3D	  , ShadowMatrix3D;
GLuint      CamPos_uniform3D,CamPos_uniformPlane;
GLuint		TextureMatrix_Uniform;

int			timesc = 0;
GLuint		gSampler1,gSampler,texID,shadowMapId;

Texture	    *pTexture = NULL;
Mesh		*m		  = NULL;

const		int			NumVerticesl = 2;
const		int			NumVerticesSQ = ( (NumOfSQ) * (NumOfSQ)) * (2) * (3);
const       int         NumVerticesCube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

bool        wireFrame = false;
bool		camera    = false;

bool		SYNC	  = false;

typedef     glm::vec4   color4;
typedef     glm::vec4   point4;

int			IndexSQ = 0,IndexSQ1 = 0,IndexCube = 0;
bool		ANIMATION = true;

//Modelling arrays
glm::vec3	pointsq[NumVerticesSQ];
glm::vec3	normalsq[NumVerticesSQ];
glm::vec2	tex_coords[NumVerticesSQ];

point4      pointsCube[NumVerticesCube];
color4      colorsCube[NumVerticesCube];

glm::vec3 pos = glm::vec3( 5.0f, 1.5f , 15.5f );

float horizAngle = 3.14f;
float verticAngle = 0.0f;


float speedo = 3.0f;
float mouseSpeedo = 0.005f;

int	  xpos = 0,ypos = 0;

float zNear;
float zFar;
float FOV;
float initialFoV = 45.0f;

float Cleft   = -10.0f;
float Cright  =  10.0f;
float Cbottom = -10.0f;
float Ctop	  =  10.0f;
float Cnear   = -10.0f;
float Cfar    =  20.0f;

// Scene orientation (stored as a quaternion)
float Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };

glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 
					 0.0, 0.5, 0.0, 0.0,
					 0.0, 0.0, 0.5, 0.0,
					 0.5, 0.5, 0.5, 1.0);

//Plane
glm::vec3		planeVertices[NumVerticesSQ];

point4 QuadVertices[] = {
	point4(-1.0f, -1.0f, 0.0f,1.0f),
	point4( 1.0f, -1.0f, 0.0f,1.0f),
	point4(-1.0f,  1.0f, 0.0f,1.0f),
	point4(-1.0f,  1.0f, 0.0f,1.0f),
	point4( 1.0f, -1.0f, 0.0f,1.0f),
	point4( 1.0f,  1.0f, 0.0f,1.0f)
};
		

struct Lights
{
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];

	GLfloat position[4];
	
	GLfloat intensity;
};

struct MaterialProperties
{
   GLfloat ambient[4];   
   GLfloat diffuse[4];   
   GLfloat specular[4];

   GLfloat shininess; 
};

struct MaterialProperties *frontMaterial;

int				    maxLights = 0;                  // maximum number of dynamic lights allowed by the graphic card
Lights			   *lights = NULL;						// array of lights
MaterialProperties *materials = NULL;
int 				divideFactor = 2;

GLuint FBO;
GLuint ShadowMap;

// Update function prototypes
bool			initSDL();
bool			event_handler(SDL_Event* event);
void			close();
void			resize_window(int width, int height);
bool			initImGui();
void			displayGui();
//extra
bool initFrameBuffer();

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

#ifdef __APPLE__
		SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
#endif

		//Create Window
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

#ifdef __APPLE__
		gWindow = SDL_CreateWindow("Chapter13", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );
		divideFactor = 4;
#else
		gWindow = SDL_CreateWindow("Chapter13", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
				SDL_GetMouseState(&xpos, &ypos);
				SDL_WarpMouseInWindow(gWindow, windowWidth / divideFactor, windowHeight / divideFactor);
				SDL_ShowCursor(0);
			}
			else
			{
				camera = false;
				SDL_GetMouseState(&xpos, &ypos);
				SDL_WarpMouseInWindow(gWindow, windowWidth / divideFactor, windowHeight / divideFactor);
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

	if (width > winMapWd && winMapHt < height)
	{
		winMapWd = (int)(width / 3) - 150;
		winMapHt = (int)(height / 3);

		if (windowWidth <= 1024 && windowHeight <= 768)
		{
			winMapWd = (int)(width / 3) - 50;
			winMapHt = (int)(height / 3);
		}
	}

	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &ShadowMap);

	initFrameBuffer();
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
		ImGui::SliderFloat("X", (float*)&Rotation[0], -10.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("Y", (float*)&Rotation[1], -10.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("Z", (float*)&Rotation[2], -10.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("W", (float*)&Rotation[3], -10.0f, 10.0f, "%.2f");
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::Button("Reset View", ImVec2(40, 20)))
	{
		Rotation[0] = 0.0f; Rotation[1] = 0.0f; Rotation[2] = 0.0f; Rotation[3] = 1.0f;

		pos = glm::vec3(5.0f, 1.5f, 15.5f);

		zNear = 0.1f;
		zFar = 100.0f;
		FOV = 45.0f;

		horizAngle = 3.14f;
		verticAngle = 0.0f;

		Cleft = -10.0f;
		Cright = 10.0f;
		Cbottom = -10.0f;
		Ctop = 10.0f;
		Cnear = -10.0f;
		Cfar = 20.0f;
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
	ImGui::End();

	ImGui::Begin("Lights-Materials");
	ImGui::SetWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);
	ImGui::SetWindowPos(ImVec2(220, 10), ImGuiSetCond_Once);
	
	if (ImGui::TreeNode("Edit Lights"))
	{
		if (ImGui::TreeNode("LightSource #1"))
		{
			ImGui::ColorEdit4("Ambient", lights[0].ambient);
			ImGui::Separator();
			ImGui::ColorEdit4("Diffuse", lights[0].diffuse);
			ImGui::Separator();
			ImGui::ColorEdit4("Specular", lights[0].specular);
			ImGui::Separator();

			ImGui::SliderFloat3("Position", lights[0].position, -10.0f, 10.0f, "%.2f");
			ImGui::Separator();

			ImGui::SliderFloat("Intensity", &lights[0].intensity, 1.0, 100.0, "%.2f");
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Materials"))
	{
		if (ImGui::TreeNode("FrontMaterial"))
		{
			ImGui::ColorEdit4("Ambient", frontMaterial[0].ambient);
			ImGui::Separator();
			ImGui::ColorEdit4("Diffuse", frontMaterial[0].diffuse);
			ImGui::Separator();
			ImGui::ColorEdit4("Specular", frontMaterial[0].specular);
			ImGui::Separator();
			ImGui::SliderFloat("Shininess", &frontMaterial[0].shininess, 0.1f, 100.0f, "%.2f");
			
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Light-Ortho Proj"))
	{
		ImGui::InputFloat("Left", &Cleft);
		ImGui::Separator();
		ImGui::InputFloat("Right", &Cright);
		ImGui::Separator();
		ImGui::InputFloat("Bottom", &Cbottom);
		ImGui::Separator();
		ImGui::InputFloat("Top", &Ctop);
		ImGui::Separator();
		ImGui::InputFloat("Near", &Cnear);
		ImGui::Separator();
		ImGui::InputFloat("Far", &Cfar);
		ImGui::TreePop();
	}
	ImGui::End();
}

void displayPlaneGeometry(glm::mat4 &md,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv,GLuint program);

bool initFrameBuffer()
{
	// Create the FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	
	// Create the depth buffer
	glGenTextures(1, &ShadowMap);
	glBindTexture(GL_TEXTURE_2D, ShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, windowWidth, windowHeight+256, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowMap, 0);
	
    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
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


void initLights()
{
	lights = new Lights[1];

	lights[0].ambient[0]  = 1.0f; lights[0].ambient[1]  = 1.0f; lights[0].ambient[2]  = 1.0f; lights[0].ambient[3]  = 1.0f;
	lights[0].diffuse[0]  = 1.0f; lights[0].diffuse[1]  = 1.0f; lights[0].diffuse[2]  = 1.0f; lights[0].diffuse[3]  = 1.0f;
	lights[0].specular[0] = 1.0f; lights[0].specular[1] = 1.0f; lights[0].specular[2] = 1.0f; lights[0].specular[3] = 1.0f;
	lights[0].position[0] = 0.5f; lights[0].position[1] = 2.0f; lights[0].position[2] = 2.0f; lights[0].position[3] = 1.0f;
	lights[0].intensity   = 20.0f;
}

void initMaterials()
{
	frontMaterial = new MaterialProperties;

	frontMaterial->ambient[0] = 0.2f;frontMaterial->ambient[1] = 0.2f;frontMaterial->ambient[2] = 0.2f;frontMaterial->ambient[3] = 1.0f;
	frontMaterial->diffuse[0] = 1.0f;frontMaterial->diffuse[1] = 0.8f;frontMaterial->diffuse[2] = 0.8f;frontMaterial->diffuse[3] = 1.0f;
	frontMaterial->specular[0] = 1.0f;frontMaterial->specular[1] = 1.0f;frontMaterial->specular[2] = 1.0f;frontMaterial->specular[3] = 1.0f;
	frontMaterial->shininess = 5.0f;
}

void LoadUniforms(GLuint program)
{
	//Load Lights
	
	glUniform4fv( glGetUniformLocation(program, "lights[0].ambient"),1,lights[0].ambient);
	glUniform4fv( glGetUniformLocation(program, "lights[0].diffuse"),1,lights[0].diffuse);
	glUniform4fv( glGetUniformLocation(program, "lights[0].specular"),1,lights[0].specular);
	glUniform4fv( glGetUniformLocation(program, "lights[0].position"),1,lights[0].position);
	glUniform1f ( glGetUniformLocation(program, "lights[0].intensity"),lights[0].intensity);
	
	//Load Material Properties

	glUniform4fv( glGetUniformLocation(program, "frontMaterial.ambient"),1,frontMaterial->ambient);
	glUniform4fv( glGetUniformLocation(program, "frontMaterial.diffuse"),1,frontMaterial->diffuse);
	glUniform4fv( glGetUniformLocation(program, "frontMaterial.specular"),1,frontMaterial->specular);
	glUniform1fv( glGetUniformLocation(program, "frontMaterial.shininess"),1,&(frontMaterial->shininess));

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

float ax = (0.0f/NumOfSQ),ay = (0.0f/NumOfSQ);	// { 0.0 , 0.0 }
float bx = (0.0f/NumOfSQ),by = (1.0f/NumOfSQ);  // { 0.0 , 1.0 }
float cx = (1.0f/NumOfSQ),cy = (1.0f/NumOfSQ);  // { 1.0 , 1.0 }
float dx = (1.0f/NumOfSQ),dy = (0.0f/NumOfSQ);  // { 1.0 , 0.0 }


int counter2 = 0,counter3 = 1;

void quadSQ( int a, int b, int c, int d )
{
			//   0,     3,     2,     1
    //specify temporary vectors along each quad's edge in order to compute the face
    // normal using the cross product rule
    glm::vec3 u = (planeVertices[b]-planeVertices[a]).xyz();
    glm::vec3 v = (planeVertices[c]-planeVertices[b]).xyz();
    glm::vec3 norm = glm::cross(u, v);
    glm::vec3 normal= glm::normalize(norm);
    

	normalsq[IndexSQ]=normal;pointsq[IndexSQ] = planeVertices[a];IndexSQ++;
    normalsq[IndexSQ]=normal;pointsq[IndexSQ] = planeVertices[b];IndexSQ++;
    normalsq[IndexSQ]=normal;pointsq[IndexSQ] = planeVertices[c];IndexSQ++;
    normalsq[IndexSQ]=normal;pointsq[IndexSQ] = planeVertices[a];IndexSQ++;
    normalsq[IndexSQ]=normal;pointsq[IndexSQ] = planeVertices[c];IndexSQ++;
    normalsq[IndexSQ]=normal;pointsq[IndexSQ] = planeVertices[d];IndexSQ++;

	// Texture Coordinate Generation for the Plane

	if(counter2 != NumOfSQ)
	{
		tex_coords[IndexSQ1] = glm::vec2((bx) + (counter2 * (1.0/NumOfSQ)),(by));IndexSQ1++; //                              { 0.0 , 1.0 }
		tex_coords[IndexSQ1] = glm::vec2((cx) + (counter2 * (1.0/NumOfSQ)),(cy));IndexSQ1++; // { 1.0 , 1.0 }
        tex_coords[IndexSQ1] = glm::vec2((dx) + (counter2 * (1.0/NumOfSQ)),(dy));IndexSQ1++; //               { 1.0 , 0.0 }
        tex_coords[IndexSQ1] = glm::vec2((bx) + (counter2 * (1.0/NumOfSQ)),(by));IndexSQ1++; //                              { 0.0 , 1.0 }
        tex_coords[IndexSQ1] = glm::vec2((dx) + (counter2 * (1.0/NumOfSQ)),(dy));IndexSQ1++; //               { 1.0 , 0.0 }
        tex_coords[IndexSQ1] = glm::vec2((ax) + (counter2 * (1.0/NumOfSQ)),(ay));IndexSQ1++; // { 0.0 , 0.0 }

		counter2++;
	}
	else
	{
		ax = (ax);ay = (ay) + (counter3 * (1.0/NumOfSQ));  // { 0.0 , 0.0 }
		bx = (bx);by = (by) + (counter3 * (1.0/NumOfSQ));  // { 0.0 , 1.0 }
		cx = (cx);cy = (cy) + (counter3 * (1.0/NumOfSQ));  // { 1.0 , 1.0 }
		dx = (dx);dy = (dy) + (counter3 * (1.0/NumOfSQ));  // { 1.0 , 0.0 }

		tex_coords[IndexSQ1] = glm::vec2(bx,by);IndexSQ1++;
		tex_coords[IndexSQ1] = glm::vec2(cx,cy);IndexSQ1++;
        tex_coords[IndexSQ1] = glm::vec2(dx,dy);IndexSQ1++;
        tex_coords[IndexSQ1] = glm::vec2(bx,by);IndexSQ1++;
        tex_coords[IndexSQ1] = glm::vec2(dx,dy);IndexSQ1++;
        tex_coords[IndexSQ1] = glm::vec2(ax,ay);IndexSQ1++;

		counter2 = 1;
	}

}

void init3Dmodel()
{
	GLuint m_Buffers[4];
	m = new Mesh();
	m->loadMesh("./Models/box1/models/CargoCube01.dae");
	glGenVertexArrays(1, &vao3Dmodel);
	glBindVertexArray(vao3Dmodel);

	//create the VBO
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	//Load shaders and use the resulting shader program
	program3Dmodel = LoadShaders("./Shaders/vSceneShader.vert", "./Shaders/fSceneShader.frag");
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
	GLuint vTexCoordMesh = glGetAttribLocation(program3Dmodel, "vTexture");
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

void initQuad()
{
	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoQuad);
    glBindVertexArray(vaoQuad);

	// Load shaders and use the resulting shader program
    programQuad = LoadShaders( "./Shaders/v1pass.vert", "./Shaders/f1pass.frag" );
    glUseProgram( programQuad );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferQuad );
    glBindBuffer( GL_ARRAY_BUFFER, bufferQuad );
    glBufferData( GL_ARRAY_BUFFER, sizeof(QuadVertices),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(QuadVertices), QuadVertices );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programQuad, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void initPlane()
{
	float numX = 0.0f,numX1 = 0.5f;
	float numZ = 0.0f,numZ1 = 0.5f;

	planeVertices[0] = glm::vec3 ( numX,  0.0, numZ1);  // 0 a 
	planeVertices[1] = glm::vec3 ( numX,  0.0, numZ );   // 1 d
	planeVertices[2] = glm::vec3 ( numX1, 0.0, numZ );  // 2 c
	planeVertices[3] = glm::vec3 ( numX1, 0.0, numZ1); // 3 b

	int k = 4;
	int counter = 0;
	for(k=4;k<NumVerticesSQ;k=k+4)
	{
		numX+=0.5f; numX1+=0.5f; counter++;

		planeVertices[k]   =   glm::vec3 (numX,  0.0, numZ1 );
		planeVertices[k+1]	=	glm::vec3 (numX,  0.0, numZ );
		planeVertices[k+2]	=	glm::vec3 (numX1, 0.0, numZ );
		planeVertices[k+3]	=   glm::vec3 (numX1, 0.0, numZ1);

		if( counter == (NumOfSQ - 1) )
		{
			numX = 0.0f;numX1 = 0.5f;k+=4;
			counter = 0;

			numZ+=0.5f;numZ1+=0.5f;

			planeVertices[k]   =   glm::vec3 (numX,  0.0, numZ1 );
			planeVertices[k+1]	=	glm::vec3 (numX,  0.0, numZ );
			planeVertices[k+2]	=	glm::vec3 (numX1, 0.0, numZ );
			planeVertices[k+3]	=   glm::vec3 (numX1, 0.0, numZ1);
		}

	}

	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoPlane);
    glBindVertexArray(vaoPlane);

	pTexture = new Texture(GL_TEXTURE_2D,"./Textures/nvidia_logo.jpg");

	if (!pTexture->loadTexture()) {

		exit(EXIT_FAILURE);
    }

	int lp = 0,a,b,c,d;
	a=0,b=3,c=2,d=1;


	for(lp = 0;lp < (NumOfSQ * NumOfSQ);lp++)
	{
		quadSQ(a,b,c,d);

		a+=4;b+=4;c+=4;d+=4;
	}

	// Load shaders and use the resulting shader program
    programPlane = LoadShaders( "./Shaders/vSceneShader.vert", "./Shaders/fSceneShader.frag" );
    glUseProgram( programPlane );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferPlane );
    glBindBuffer( GL_ARRAY_BUFFER, bufferPlane );
	
	glBufferData( GL_ARRAY_BUFFER, sizeof(pointsq) + sizeof(normalsq) + sizeof(tex_coords),NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsq), pointsq );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsq), sizeof(normalsq), normalsq );
	glBufferSubData( GL_ARRAY_BUFFER,sizeof(pointsq) + sizeof(normalsq),sizeof(tex_coords),tex_coords );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programPlane, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );

	GLuint MCvertex = glGetAttribLocation( programDepth , "MCvertex" );
	glEnableVertexAttribArray( MCvertex );
    glVertexAttribPointer( MCvertex, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );

	GLuint vNormal = glGetAttribLocation( programPlane, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(pointsq)) );

	GLuint vText = glGetAttribLocation( programPlane, "vTexture" );
    glEnableVertexAttribArray( vText );
	glVertexAttribPointer( vText, 2, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(pointsq) + sizeof(normalsq)) );
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void display3Dmodel(glm::mat4 &tsl,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv,GLuint program)
{
	if(program == program3Dmodel)
	{
		glUseProgram(program3Dmodel);
		glBindVertexArray(vao3Dmodel);

		if (wireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		Model_uniform3D = glGetUniformLocation(program3Dmodel, "M_mat");
		View_uniform3D = glGetUniformLocation(program3Dmodel, "V_mat");
		MVP_uniform3D = glGetUniformLocation(program3Dmodel, "MVP_mat");
		ShadowMatrix3D = glGetUniformLocation(program3Dmodel, "ShadowMatrix");
		CamPos_uniform3D = glGetUniformLocation(program3Dmodel, "CameraPosition");
		LightPos_uniform3D = glGetUniformLocation(program3Dmodel, "LightPosition");
		

		glm::mat4 model_mat = tsl;
		glUniformMatrix4fv(Model_uniform3D,1, GL_FALSE, glm::value_ptr(model_mat));
		glm::mat4 view_mat = glm::lookAt(positionv,positionv+directionv,upv);
		glUniformMatrix4fv(View_uniform3D,1, GL_FALSE, glm::value_ptr(view_mat));
		
		glm::mat4 ModelView = view_mat * model_mat;

		//Calculation of ModelViewProjection Matrix
		float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
		glm::mat4 projection_mat = glm::perspective(FOV,aspect,zNear,zFar);
		glm::mat4 MVP_mat = projection_mat * ModelView;
		glUniformMatrix4fv(MVP_uniform3D, 1, GL_FALSE, glm::value_ptr(MVP_mat));

		glUniform3fv(CamPos_uniform3D,1,glm::value_ptr(positionv));

		// Calculation of Shadow Matrix
		glm::vec3 sLpos = glm::vec3(lights[0].position[0],lights[0].position[1],lights[0].position[2]);

		glUniform3fv(LightPos_uniform3D,1,glm::value_ptr(sLpos));

		glm::mat4 model_smat = tsl;
		glm::mat4 view_smat  = glm::lookAt(sLpos,glm::vec3(0,0,0),glm::vec3(0,1,0));
		glm::mat4 projection_mats = glm::ortho(Cleft, Cright, Cbottom, Ctop, Cnear, Cfar);

		glm::mat4 SMVP_MAP = biasMatrix * projection_mats * view_smat * model_smat;

		glUniformMatrix4fv(ShadowMatrix3D,1,GL_FALSE,glm::value_ptr(SMVP_MAP));

		gSampler1 =  glGetUniformLocationARB(program3Dmodel, "textureMap");
		glUniform1iARB(gSampler1, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ShadowMap);

		shadowMapId = glGetUniformLocationARB(program3Dmodel, "shadowMap");
		glUniform1iARB(shadowMapId, 1);

		LoadUniforms(program3Dmodel);

		m->render(); 

		if(timesc < 1)
			checkActiveUniforms();
		timesc++;

		glBindVertexArray(0);
		//glUseProgram(0);
	}
	else if(program == programDepth)
	{
		glUseProgram(programDepth);
		glBindVertexArray(vao3Dmodel);

		if (wireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		MVP_uniform3D = glGetUniformLocation(programDepth, "depthMVP");

		//Calculation of ModelView Matrix
		glm::mat4 model_mat = tsl;
		glm::vec3 sLpos = glm::vec3(lights[0].position[0],lights[0].position[1],lights[0].position[2]);
		glm::mat4 view_mat = glm::lookAt(sLpos,glm::vec3(0,0,0),glm::vec3(0,1,0));
		glm::mat4 MV_mat = view_mat * model_mat;

		//Calculation of ModelViewProjection Matrix
		float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
		glm::mat4 projection_mat = glm::ortho(Cleft, Cright, Cbottom, Ctop, Cnear, Cfar);
		glm::mat4 MVP_mat = projection_mat * MV_mat;

		glUniformMatrix4fv(MVP_uniform3D, 1, GL_FALSE, glm::value_ptr(MVP_mat));

		m->render(); 

		glBindVertexArray(0);
		//glUseProgram(0);
	}
	else
	{
		std::cout<<"\nProgram 3D Reports :  Problem to Draw Function @ : GLuint programm doesn't mach.\n";
		exit(EXIT_FAILURE);
	}
}

void displayQuad(glm::mat4 &md,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv,GLuint program)
{
	glUseProgram(programQuad);
	glBindVertexArray(vaoQuad);

	if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ShadowMap);

	texID =  glGetUniformLocationARB(programQuad, "texID");
	glUniform1i(texID, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glDrawArrays( GL_TRIANGLES, 0, 6);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glBindVertexArray(0);
	//glUseProgram(0);
}

void displayPlaneGeometry(glm::mat4 &md,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv,GLuint program)
{
	if(program == programPlane)
	{
		glUseProgram(programPlane);
		glBindVertexArray(vaoPlane);
	
		if (wireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		Model_uniformPlane = glGetUniformLocation(programPlane, "M_mat");
		View_uniformPlane = glGetUniformLocation(programPlane, "V_mat");
		MVP_uniformPlane = glGetUniformLocation(programPlane, "MVP_mat");
		ShadowMatrixPlane = glGetUniformLocation(programPlane, "ShadowMatrix");
		CamPos_uniformPlane = glGetUniformLocation(programPlane, "CameraPosition");
		LightPos_uniformPlane = glGetUniformLocation(programPlane, "LightPosition");

		// Calculation of ModelView Matrix
		glm::mat4 model_mat_plane =  md;
		glUniformMatrix4fv(Model_uniformPlane,1, GL_FALSE, glm::value_ptr(model_mat_plane));

		glm::mat4 view_mat_plane = glm::lookAt(positionv,positionv+directionv,upv);
		glUniformMatrix4fv(View_uniformPlane,1, GL_FALSE, glm::value_ptr(view_mat_plane));

		glm::mat4 MV_mat_plane = view_mat_plane * model_mat_plane;

		// Calculation of ModelViewProjection Matrix
		float aspect_plane = (GLfloat)windowWidth / (GLfloat)windowHeight;
		glm::mat4 projection_mat_plane = glm::perspective(FOV, aspect_plane,zNear,zFar);
		glm::mat4 MVP_mat_plane = projection_mat_plane * MV_mat_plane;
		glUniformMatrix4fv(MVP_uniformPlane, 1, GL_FALSE, glm::value_ptr(MVP_mat_plane));

		// Calculation of Shadow Matrix
		glm::vec3 sLpos = glm::vec3(lights[0].position[0],lights[0].position[1],lights[0].position[2]);
		glUniform3fv(LightPos_uniformPlane,1,glm::value_ptr(sLpos));

		glUniform3fv(CamPos_uniformPlane,1,glm::value_ptr(positionv));

		glm::mat4 model_smat = md;
		glm::mat4 view_smat  = glm::lookAt(sLpos,glm::vec3(0,0,0),glm::vec3(0,1,0));
		glm::mat4 projection_mats = glm::ortho(Cleft, Cright, Cbottom, Ctop, Cnear, Cfar);

		glm::mat4 SMVP_MAP = biasMatrix * projection_mats * view_smat * model_smat; 

		glUniformMatrix4fv(ShadowMatrixPlane,1,GL_FALSE,glm::value_ptr(SMVP_MAP));

		LoadUniforms(programPlane);

		pTexture->bindTexture(GL_TEXTURE0);

		gSampler =  glGetUniformLocationARB(programPlane, "textureMap");
		glUniform1iARB(gSampler, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ShadowMap);

		shadowMapId = glGetUniformLocationARB(programPlane, "shadowMap");
		glUniform1iARB(shadowMapId, 1);

		glDrawArrays( GL_TRIANGLES, 0, NumVerticesSQ );
   
		glBindVertexArray(0);
		//glUseProgram(0);
	}
	else if(program == programDepth)
	{
		glUseProgram(programDepth);
		glBindVertexArray(vaoPlane);
	
		if (wireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		MVP_uniformPlane = glGetUniformLocation(programDepth, "depthMVP");

		// Calculation of ModelView Matrix
		glm::mat4 model_mat_plane =  md;
		glm::vec3 sLpos = glm::vec3(lights[0].position[0],lights[0].position[1],lights[0].position[2]);
		glm::mat4 view_mat_plane = glm::lookAt(sLpos,glm::vec3(0,0,0),glm::vec3(0,1,0));
		glm::mat4 MV_mat_plane = view_mat_plane * model_mat_plane;

		// Calculation of ModelViewProjection Matrix
		float aspect_plane = (GLfloat)windowWidth / (GLfloat)windowHeight;
		glm::mat4 projection_mat_plane = glm::ortho(Cleft, Cright, Cbottom, Ctop, Cnear, Cfar);
		glm::mat4 MVP_mat_plane = projection_mat_plane * MV_mat_plane;

		glUniformMatrix4fv(MVP_uniformPlane, 1, GL_FALSE, glm::value_ptr(MVP_mat_plane));

		glDrawArrays( GL_TRIANGLES, 0, NumVerticesSQ );
    
		glBindVertexArray(0);
		//glUseProgram(0);
	}
	else
	{
		std::cout<<"\nProgram Plane Reports :  Problem to Draw Function @ : GLuint programm doesn't mach.\n";
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char * argv[])
{
	glm::mat4 mat;
	float axis[] = { 0.7f, 0.7f, 0.7f }; // initial model rotation
    float angle = 0.8f;

	double FT  = 0;
	

	double starting = 0.0;
	double ending   = 0.0;
	
	int fr = 0;

	zNear = 0.1f;
	zFar  = 100.0f;
	FOV   = 45.0f; 

	// Current time
	double time = 0;

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

	std::string vendor ="Title : Chapter-13   Vendor : " + stream1.str() + "   Renderer : " +stream2.str();

	const char *tit = vendor.c_str();
	SDL_SetWindowTitle(gWindow, tit);
   
	
	 // Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	if(!initFrameBuffer())
	{
		std::cout<<"\nProblem initializing the FRAMEBUFFER!\n";
		exit(EXIT_FAILURE);
	}

	programDepth = LoadShaders( "./Shaders/vDepthShader.vert", "./Shaders/fDepthShader.frag" );
	glUseProgram(programDepth);

	initQuad(); // initialize a Square

	init3Dmodel(); // initialize 3D model

	initPlane(); //initialize Plane

	

	initLights(); // initialize Lighting of the Scene
	initMaterials(); // initialize Material properties of the Objects and the Scene

	//create_Light_Material_bar();

	bool flagPlus  = false;
	bool flagMinus = true;
	GLfloat rat = 0.001f;
	float posL1 = 1.1f;
	float posL2 = 0.05f;

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
	{
		SYNC = true;
	}
	//setVSync(SYNC);
#endif

	FOV = initialFoV - 5;

#ifdef __APPLE__
	int *w = (int*)malloc(sizeof(int));
	int *h = (int*)malloc(sizeof(int));
#endif

	while (running) {
		glm::vec3 direction(cos(verticAngle) * sin(horizAngle), sin(verticAngle), cos(verticAngle) * cos(horizAngle));

		glm::vec3 right = glm::vec3(sin(horizAngle - 3.14f / 2.0f), 0, cos(horizAngle - 3.14f / 2.0f));

		glm::vec3 up = glm::cross(right, direction);

		currentTime = SDL_GetTicks();
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
					pos += direction * dTime *  speedo;

				if (event.key.keysym.sym == SDLK_DOWN)
					pos -= direction * dTime * speedo;

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

		if(camera == true)
		{
			SDL_GetMouseState(&xpos, &ypos);
			SDL_WarpMouseInWindow(gWindow, windowWidth / divideFactor, windowHeight / divideFactor);
			//SDL_ShowCursor(0);
		
			horizAngle  += mouseSpeedo * float(windowWidth/divideFactor - xpos );
			verticAngle += mouseSpeedo * float( windowHeight/divideFactor - ypos );
		}


		mat = ConvertQuaternionToMatrix(Rotation, mat);

		glm::mat4 cube,cubeL1,cubeL2;
        glm::mat4 unitMat1(1.0),unitMat2(1.0);

		glm::mat4 translateMat = glm::mat4();
		translateMat = glm::translate(translateMat,glm::vec3(5.0,3.0,4.0));

		cube  = translateMat * mat;
		///////////////////////////////////////////////////  FIRST PASS
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
		glViewport(0, 0,windowWidth,windowHeight);

		glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		display3Dmodel(cube,pos,direction,up,programDepth);
        
		displayPlaneGeometry(unitMat1,pos,direction,up,programDepth);

        
		glDisable(GL_CULL_FACE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//////////////////////////////////////////////////   SECOND PASS
		glViewport(0, 0,windowWidth,windowHeight);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//displayPlaneGeometry(glm::mat4(1.0),pos,direction,up,programPlane);
        displayPlaneGeometry(unitMat2, pos, direction, up, programPlane);
		display3Dmodel(cube,pos,direction,up,program3Dmodel);

		//glPopAttrib();

		
		glViewport(0, 0,winMapWd,winMapHt);

		
		displayQuad(mat,pos,direction,up,programQuad);

		glDisable(GL_CULL_FACE);

		fr++;
		ending = SDL_GetTicks() / 1000.0f;

		if(ending - starting >= 1)
		{
			rate = fr;
			fr = 0;
			starting = SDL_GetTicks() / 1000.0f;
		}

		double CT = SDL_GetTicks() / 1000.0f;
		Frames++;
		if(CT -LT >= 1.0)
		{
			FPS = 1000.0 / (double)Frames;
			Frames = 0;
			LT += 1.0f;
		}

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

	//close OpenGL window and  terminate SDL2 and ImGui
	close();
    
    exit(EXIT_SUCCESS);
}

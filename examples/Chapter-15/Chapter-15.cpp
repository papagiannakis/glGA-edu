// basic STL streams
#include <iostream>
#include <sstream>
// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>
// Update 05/08/16
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
#include "PerlinNoise.h"



// number of Squares for Plane
#define NumOfSQ 20

// update globals
SDL_Window		*gWindow = NULL;
SDL_GLContext	gContext;
float bgColor[] = { 0.0f, 0.0f, 0.0f, 0.1f };
double FPS;

// global variables
int         windowWidth=1024, windowHeight=768;

GLuint      programPlane, program3Dmodel;
GLuint      vao, vaoPlane, vao3Dmodel;
GLuint      bufferPlane, buffer3Dmodel;

GLuint		MV_uniformPlane   , MVP_uniformPlane   , Normal_uniformPlane;
GLuint		MV_uniform3D      , MVP_uniform3D     , Normal_uniform3D;

GLuint		RotateCloud_uniform3d;

GLuint		TextureMatrix_Uniform;
int			timesc = 0;
GLuint		gSampler1,gSampler;

Texture	    *pTexture = NULL;
Mesh		*m		  = NULL;

const		int			NumVerticesSQ = ( (NumOfSQ) * (NumOfSQ)) * (2) * (3);
const       int         NumVerticesCube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

bool        wireFrame = false;
bool		camera    = false;

bool		SYNC	  = true;

typedef     glm::vec4   color4;
typedef     glm::vec4   point4;

int			IndexSQ = 0,IndexSQ1 = 0,IndexCube = 0;

//Modelling arrays
point4		pointsq[NumVerticesSQ];
color4		colorsq[NumVerticesSQ];
glm::vec3	normalsq[NumVerticesSQ];
glm::vec4	tex_coords[NumVerticesSQ];

glm::vec3 pos = glm::vec3( 0.0f, 0.0f , 10.0f );

PerlinNoise *Noise = NULL;

glm::vec3 LightPos	 = glm::vec3(1.0, 1.0,-2.0);
glm::vec3 SkyColor	 = glm::vec3(0.0, 0.0, 0.8);
glm::vec3 CloudColor = glm::vec3(0.8, 0.8, 0.8);

int flagM = 0;
int flagP = 1;
float Scale = 1.0;

glm::mat4 rotateCloud = glm::mat4();
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
		gWindow = SDL_CreateWindow("Chapter15", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );
#else
		gWindow = SDL_CreateWindow("Chapter15", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
	ImGui::End();

	ImGui::Begin("Noise Control");
	ImGui::SetWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);
	ImGui::SetWindowPos(ImVec2(220, 10), ImGuiSetCond_Once);

	ImGui::ColorEdit3("Sky Color", (float*)&SkyColor);
	ImGui::Separator();
	ImGui::ColorEdit3("Cloud Color", (float*)&CloudColor);
	ImGui::Separator();
	ImGui::SliderFloat("Scale Factor", &Scale, -10.0, 10.0, "%.2f");
	ImGui::Separator();
	if (ImGui::TreeNode("Light Position"))
	{
		ImGui::SliderFloat3("Position", (float*)&LightPos, -10.0, 10.0, "%.2f");
		ImGui::Separator();
		ImGui::InputFloat("X", &LightPos[0], 0.1, 0.5, 2);
		ImGui::InputFloat("Y", &LightPos[1], 0.1, 0.5, 2);
		ImGui::InputFloat("Z", &LightPos[2], 0.1, 0.5, 2);
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
    

	normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[a]; pointsq[IndexSQ] = planeVertices[a];IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[b]; pointsq[IndexSQ] = planeVertices[b];IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[c]; pointsq[IndexSQ] = planeVertices[c];IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[a]; pointsq[IndexSQ] = planeVertices[a];IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[c]; pointsq[IndexSQ] = planeVertices[c];IndexSQ++;
    normalsq[IndexSQ]=normal;colorsq[IndexSQ] = planeColor[d]; pointsq[IndexSQ] = planeVertices[d];IndexSQ++;

	// Texture Coordinate Generation for the Plane

	if(counter2 != NumOfSQ)
	{
		tex_coords[IndexSQ1] = glm::vec4((bx) + (counter2 * (1.0/NumOfSQ)),(by),0.0,0.0);IndexSQ1++; //                              { 0.0 , 1.0 }
		tex_coords[IndexSQ1] = glm::vec4((cx) + (counter2 * (1.0/NumOfSQ)),(cy),0.0,0.0);IndexSQ1++; // { 1.0 , 1.0 }
        tex_coords[IndexSQ1] = glm::vec4((dx) + (counter2 * (1.0/NumOfSQ)),(dy),0.0,0.0);IndexSQ1++; //               { 1.0 , 0.0 }
        tex_coords[IndexSQ1] = glm::vec4((bx) + (counter2 * (1.0/NumOfSQ)),(by),0.0,0.0);IndexSQ1++; //                              { 0.0 , 1.0 }
        tex_coords[IndexSQ1] = glm::vec4((dx) + (counter2 * (1.0/NumOfSQ)),(dy),0.0,0.0);IndexSQ1++; //               { 1.0 , 0.0 }
        tex_coords[IndexSQ1] = glm::vec4((ax) + (counter2 * (1.0/NumOfSQ)),(ay),0.0,0.0);IndexSQ1++; // { 0.0 , 0.0 }

		counter2++;
	}
	else
	{
		ax = (ax);ay = (ay) + (counter3 * (1.0/NumOfSQ));  // { 0.0 , 0.0 }
		bx = (bx);by = (by) + (counter3 * (1.0/NumOfSQ));  // { 0.0 , 1.0 }
		cx = (cx);cy = (cy) + (counter3 * (1.0/NumOfSQ));  // { 1.0 , 1.0 }
		dx = (dx);dy = (dy) + (counter3 * (1.0/NumOfSQ));  // { 1.0 , 0.0 }

		tex_coords[IndexSQ1] = glm::vec4(bx,by,0.0,0.0);IndexSQ1++;
		tex_coords[IndexSQ1] = glm::vec4(cx,cy,0.0,0.0);IndexSQ1++;
        tex_coords[IndexSQ1] = glm::vec4(dx,dy,0.0,0.0);IndexSQ1++;
        tex_coords[IndexSQ1] = glm::vec4(bx,by,0.0,0.0);IndexSQ1++;
        tex_coords[IndexSQ1] = glm::vec4(dx,dy,0.0,0.0);IndexSQ1++;
        tex_coords[IndexSQ1] = glm::vec4(ax,ay,0.0,0.0);IndexSQ1++;

		counter2 = 1;
	}

}

void initPlane()
{
	float numX = 0.0f,numX1 = 0.5f;
	float numZ = 0.0f,numZ1 = 0.5f;

	planeVertices[0] = point4 ( numX, 0.0, numZ1, 1.0);  planeColor[0] = color4 (0.603922, 0.803922, 0.196078, 1.0); // 0 a 
	planeVertices[1] = point4 ( numX, 0.0, numZ, 1.0);   planeColor[1] = color4 (0.603922, 0.803922, 0.196078, 1.0); // 1 d
	planeVertices[2] = point4 ( numX1, 0.0, numZ, 1.0);  planeColor[2] = color4 (0.603922, 0.803922, 0.196078, 1.0); // 2 c
	planeVertices[3] = point4 ( numX1, 0.0, numZ1, 1.0); planeColor[3] = color4 (0.603922, 0.803922, 0.196078, 1.0); // 3 b

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
			numX = 0.0f;numX1 = 0.5f;k+=4;
			counter = 0;

			numZ+=0.5f;numZ1+=0.5f;

			planeVertices[k]   =   point4 (numX,  0.0, numZ1, 1.0); planeColor[k]   = color4 (0.603922, 0.803922, 0.196078, 1.0);
			planeVertices[k+1]	=	point4 (numX,  0.0, numZ, 1.0); planeColor[k+1] = color4 (0.603922, 0.803922, 0.196078, 1.0);
			planeVertices[k+2]	=	point4 (numX1, 0.0, numZ, 1.0); planeColor[k+2] = color4 (0.603922, 0.803922, 0.196078, 1.0);
			planeVertices[k+3]	=   point4 (numX1, 0.0, numZ1, 1.0); planeColor[k+3] = color4 (0.603922, 0.803922, 0.196078, 1.0);
		}

	}

	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoPlane);
    glBindVertexArray(vaoPlane);

	pTexture = new Texture(GL_TEXTURE_2D,"./Textures/nvidia_logo.jpg");
	//pTexture = new Texture(GL_TEXTURE_2D,"./Textures/NVIDIA.jpg");

	if (!pTexture->loadTexture()) {

		::exit(EXIT_FAILURE);
    }

	int lp = 0,a,b,c,d;
	a=0,b=3,c=2,d=1;


	for(lp = 0;lp < (NumOfSQ * NumOfSQ);lp++)
	{
		quadSQ(a,b,c,d);

		a+=4;b+=4;c+=4;d+=4;
	}

	// Load shaders and use the resulting shader program
    programPlane = LoadShaders( "./Shaders/vPlaneShader.vert", "./Shaders/fPlaneShader.frag" );
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

void init3Dmodel()
{

	GLuint m_Buffers[4];
	m = new Mesh();
	m->loadMesh("./Models/sphere.dae");
	//m->loadMesh("./Models/sphere.obj");

	glGenVertexArrays(1, &vao3Dmodel);
	glBindVertexArray(vao3Dmodel);

	//create the VBO
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);


	//Load shaders and use the resulting shader program
	//Load shaders and use the resulting shader program
	program3Dmodel = LoadShaders("./Shaders/vshader3D.vert", "./Shaders/fshader3D.frag");

	glUseProgram(program3Dmodel);

	std::cout << "this is Position size :  " << m->Positions.size() << " and this is Normal size : " << m->Normals.size() << " and this is TexCoords size : " << m->TexCoords.size() << std::endl;
	std::cout << "this is numVertices " << m->numVertices << std::endl;
	std::cout << "this is numSamplers " << m->m_TextureSamplers.size() << std::endl;
	std::cout << "this is numTextures " << m->m_Textures.size() << std::endl;

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
	GLuint vTexCoordMesh = glGetAttribLocation(program3Dmodel, "MCCoords");
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


	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);



glBindVertexArray(0);



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

	MV_uniform3D = glGetUniformLocation(program3Dmodel, "MVMatrix");
	MVP_uniform3D = glGetUniformLocation(program3Dmodel, "MVPMatrix");
	Normal_uniform3D = glGetUniformLocation(program3Dmodel, "NormalMatrix");
	RotateCloud_uniform3d = glGetUniformLocation(program3Dmodel, "RotateCloud");

	rotateCloud = glm::rotate(glm::mat4(),::angle,glm::vec3(1.0,1.0,1.0));
	::angle += 0.035;

	glUniformMatrix4fv(RotateCloud_uniform3d,1, GL_FALSE, glm::value_ptr(rotateCloud));

	//Calculation of ModelView Matrix
	glm::mat4 model_mat = tsl;
	glm::mat4 view_mat = glm::lookAt(positionv,positionv + directionv,upv);

	glm::mat4 MV_mat = view_mat * model_mat;
	glUniformMatrix4fv(MV_uniform3D,1, GL_FALSE, glm::value_ptr(MV_mat)); 


	if(Scale >= 2 && flagP == 1)
	{
		flagP = 0;
		flagM = 1;
	}
	else if(Scale <= 0.35 && flagM == 1)
	{
		flagM = 0;
		flagP = 1;
	}
	
	if(flagP == 1)
	{
		Scale += 0.0004;
	}
	else
	{
		Scale -= 0.0004;
	}

	glUniform3fv(glGetUniformLocation(program3Dmodel,"LightPos"),1,glm::value_ptr(LightPos));
	glUniform3fv(glGetUniformLocation(program3Dmodel,"SkyColor"),1,glm::value_ptr(SkyColor));
	glUniform3fv(glGetUniformLocation(program3Dmodel,"CloudColor"),1,glm::value_ptr(CloudColor));
	glUniform1f(glGetUniformLocation(program3Dmodel,"Scale"),Scale);

	//Calculation of Normal Matrix
	glm::mat3 Normal_mat = glm::transpose(glm::inverse(glm::mat3(MV_mat)));
	glUniformMatrix3fv(Normal_uniform3D,1, GL_FALSE, glm::value_ptr(Normal_mat));

	//Calculation of ModelViewProjection Matrix
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_mat = glm::perspective(FOV, aspect,0.1f,100.0f);
	glm::mat4 MVP_mat = projection_mat * MV_mat;
	glUniformMatrix4fv(MVP_uniform3D, 1, GL_FALSE, glm::value_ptr(MVP_mat));


	glUniform1i(glGetUniformLocation(program3Dmodel, "Noise"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, Noise->noise3DTexName);

	m->render(); 

	if(timesc < 1)
	{
		checkActiveUniforms();
	}
	timesc++;

	glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}

void displayPlane(glm::mat4 &md,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
	glUseProgram(programPlane);
	glBindVertexArray(vaoPlane);

	glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);	
	
    if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	MV_uniformPlane = glGetUniformLocation(programPlane, "MV_mat");
	MVP_uniformPlane = glGetUniformLocation(programPlane, "MVP_mat");
	Normal_uniformPlane = glGetUniformLocation(programPlane, "Normal_mat");
	TextureMatrix_Uniform = glGetUniformLocation(programPlane, "TextureMatrix");

	glm::mat4 TexMat = glm::mat4();

	glUniformMatrix4fv(TextureMatrix_Uniform,1,GL_FALSE,glm::value_ptr(TexMat));

	// Calculation of ModelView Matrix
	glm::mat4 model_mat_plane =  md;
	glm::mat4 view_mat_plane = glm::lookAt(positionv,positionv + directionv,upv);
	glm::mat4 MV_mat_plane = view_mat_plane * model_mat_plane;
	glUniformMatrix4fv(MV_uniformPlane,1, GL_FALSE, glm::value_ptr(MV_mat_plane));

	// Calculation of Normal Matrix
	glm::mat3 Normal_mat_plane = glm::transpose(glm::inverse(glm::mat3(MV_mat_plane)));
	glUniformMatrix3fv(Normal_uniformPlane,1, GL_FALSE, glm::value_ptr(Normal_mat_plane));

	// Calculation of ModelViewProjection Matrix
	float aspect_plane = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_mat_plane = glm::perspective(FOV, aspect_plane,zNear,zFar);
	glm::mat4 MVP_mat_plane = projection_mat_plane * MV_mat_plane;
	glUniformMatrix4fv(MVP_uniformPlane, 1, GL_FALSE, glm::value_ptr(MVP_mat_plane));


	gSampler =  glGetUniformLocationARB(programPlane, "gSampler");
	glUniform1iARB(gSampler, 0);
    
	pTexture->bindTexture(GL_TEXTURE0);

    glDrawArrays( GL_TRIANGLES, 0, NumVerticesSQ );
    
    glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}


int main (int argc, char * argv[])
{
	glm::mat4 mat;
	
	double FPS = 0;

	double starting = 0.0;
	double ending   = 0.0;
	int rate = 0;
	int fr = 0;

	zNear = 0.1f;
	zFar  = 100.0f;
	FOV   = 45.0f; 

	// Current time
	double time = 0;

    // initialise GLFW
    int running = GL_TRUE;
    
    if (!initSDL()) {
        ::exit(EXIT_FAILURE);
    }
    
    
    std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
    std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
    std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
   
	std::ostringstream stream1,stream2;

	stream1 << glGetString(GL_VENDOR);
	stream2 << glGetString(GL_RENDERER);

	std::string vendor ="Title : Chapter-15   Vendor : " + stream1.str() + "   Renderer : " +stream2.str();

	const char *tit = vendor.c_str();
	SDL_SetWindowTitle(gWindow, tit);

	
	 // Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	initPlane(); //initialize Plane

	Noise = new PerlinNoise(128,128,128);

	init3Dmodel();

	CloudColor = glm::vec3(0.8, 0.8, 0.8);

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

	FOV = initialFoV;

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
		lastTime = (float)currentTime;

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
					pos -= direction *dTime * speedo;

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
			SDL_WarpMouseInWindow(gWindow, windowWidth/2, windowHeight/2);
		
			horizAngle  += mouseSpeedo * float(windowWidth/2 - xpos );
			verticAngle += mouseSpeedo * float( windowHeight/2 - ypos );
		}



		mat = ConvertQuaternionToMatrix(Rotation, mat);

		//displayPlane(mat,pos,direction,up);
		mat = glm::scale(mat,glm::vec3(2.0,2.0,2.0));

		display3Dmodel(mat,pos,direction,up);

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

	//close OpenGL window and  terminate ImGui and SDL2
	close();
    
    
    ::exit(EXIT_SUCCESS);
}

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
double FPS;

// global variables
int         windowWidth=1024, windowHeight=768;

GLuint      programQuad;
GLuint      vaoQuad;
GLuint      bufferQuad;

GLuint		TextureMatrix_Uniform;
int			timesc = 0;
GLuint		gSampler1,gSampler;

Texture	    *BaseTex  = NULL;
Texture		*BlendTex = NULL;
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

point4 QuadVertices[] = {
	point4(-1.0f, -1.0f, 0.0f,1.0f),
	point4( 1.0f, -1.0f, 0.0f,1.0f),
	point4(-1.0f,  1.0f, 0.0f,1.0f),
	point4(-1.0f,  1.0f, 0.0f,1.0f),
	point4( 1.0f, -1.0f, 0.0f,1.0f),
	point4( 1.0f,  1.0f, 0.0f,1.0f)
};

struct BlendModes{

	enum BM {Normal     , Average			 , Dissolve    , Behind   , 
			 Clear      , Darken			 , Lighten     , Multiply ,
			 Screen     , Color_Burn		 , Color_Dodge , Overlay  ,
			 Soft_Light , Hard_Light         , Add         , Subtract ,
			 Difference , Inverse_Difference , Exclusion			 };
	BM blend_mode;
};

BlendModes *x;

float opacity = 1.0f;

bool Op = true;

float noiseScale = 1.0f;

glm::vec3 pos = glm::vec3( 0.0f, 0.0f , 30.0f );

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
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

		SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");

		//Create Window
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

		gWindow = SDL_CreateWindow("Chapter19", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );
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
	const char* items[] = {	"1.Normal",
							"2.Average",
							"3.Dissolve",
							"4.Behind",
							"5.Clear",
							"6.Darken",
							"7.Lighten",
							"8.Multiply",
							"9.Screen",
							"10.Color_Burn",
							"11.Color_Dogde",
							"12.Overlay",
							"13.Soft_Light",
							"14.Hard_Light",
							"15.Add",
							"16.Subtract",
							"17.Difference",
							"18.Inverse_Difference",
							"19.Exclusion"};
	static int current_item = 0;
	
	ImGui::Combo("Blend-Modes", &current_item, items, 19);
	switch (current_item)
	{
	case	0: x->blend_mode = BlendModes::Normal; break;
	case	1: x->blend_mode = BlendModes::Average; break;
	case	2: x->blend_mode = BlendModes::Dissolve; break;
	case	3: x->blend_mode = BlendModes::Behind; break;
	case	4: x->blend_mode = BlendModes::Clear; break;
	case	5: x->blend_mode = BlendModes::Darken; break;
	case	6: x->blend_mode = BlendModes::Lighten; break;
	case	7: x->blend_mode = BlendModes::Multiply; break;
	case	8: x->blend_mode = BlendModes::Screen; break;
	case	9: x->blend_mode = BlendModes::Color_Burn; break;
	case	10: x->blend_mode = BlendModes::Color_Dodge; break;
	case	11: x->blend_mode = BlendModes::Overlay; break;
	case	12: x->blend_mode = BlendModes::Soft_Light; break;
	case	13: x->blend_mode = BlendModes::Hard_Light; break;
	case	14: x->blend_mode = BlendModes::Add; break;
	case	15: x->blend_mode = BlendModes::Subtract; break;
	case	16: x->blend_mode = BlendModes::Difference; break;
	case	17: x->blend_mode = BlendModes::Inverse_Difference; break;
	case	18: x->blend_mode = BlendModes::Exclusion; break;
	}
	ImGui::Separator();
	static bool opacity_mode = false;
	if (ImGui::TreeNode("Imaging Control"))
	{
		ImGui::SliderFloat("Opacity", &opacity, 0.05f, 3.0f, "%.2f");
		if (ImGui::Checkbox("Opacity Mode", &opacity_mode))
		{
			if (opacity_mode)
				Op = true;
			else
				Op = false;
		}
		ImGui::SliderFloat("noiseScale", &noiseScale, 0.5f, 100.0f, "%.2f");
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


glm::vec4 Background = glm::vec4(0.0,0.0,0.0,1.0);


void checkActiveUniforms()
{
	GLint nUniforms, maxLen;
	glGetProgramiv( programQuad, GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLen);
	glGetProgramiv( programQuad, GL_ACTIVE_UNIFORMS,&nUniforms);
	GLchar * name = (GLchar *) malloc( maxLen );
	GLint size, location;
	GLsizei written;
	GLenum type;

	printf(" Location | Name\n");
	printf("------------------------------------------------\n");

	for( int i = 0; i < nUniforms; ++i ) {

		glGetActiveUniform( programQuad, i, maxLen, &written,&size, &type, name );
		location = glGetUniformLocation(programQuad, name);
		printf(" %-8d | %s\n", location, name);
	}
	free(name);
}

void initQuad()
{
	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoQuad);
    glBindVertexArray(vaoQuad);

	BaseTex  = new Texture(GL_TEXTURE_2D,"./Textures/Special-Summer-Sunset.jpg");
	BlendTex = new Texture(GL_TEXTURE_2D,"./Textures/sail-boat-images.jpg");

	if (!BaseTex->loadTexture() || !BlendTex->loadTexture()) {

		exit(EXIT_FAILURE);
    }

	// Load shaders and use the resulting shader program
    programQuad = LoadShaders( "./Shaders/vBlend.vert", "./Shaders/fBlend.frag" );
    glUseProgram( programQuad );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferQuad );
    glBindBuffer( GL_ARRAY_BUFFER, bufferQuad );
    glBufferData( GL_ARRAY_BUFFER, sizeof(QuadVertices),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(QuadVertices), QuadVertices );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programQuad, "MCvertex" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void updateBlendMode(BlendModes *y)
{
	for(int i = 0; i < 19 ; i++)
	{
		if(y->blend_mode == i)
		{
			glUniform1i(glGetUniformLocation(programQuad,"Mode"),i);
		}
	}
}

void displayQuad()
{
	glUseProgram(programQuad);
	glBindVertexArray(vaoQuad);

	glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniform1f(glGetUniformLocation(programQuad,"Opacity"),opacity);

	glUniform1f(glGetUniformLocation(programQuad,"noiseScale"),noiseScale);

	if(Op == true)
	{
		glUniform1i(glGetUniformLocation(programQuad,"Opacity_Mode"),1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(programQuad,"Opacity_Mode"),0);
	}

	updateBlendMode(x);

	gSampler1 =  glGetUniformLocationARB(programQuad, "BaseImage");
	glUniform1iARB(gSampler1, 0);

	BaseTex->bindTexture(GL_TEXTURE0);

	gSampler =  glGetUniformLocationARB(programQuad, "BlendImage");
	glUniform1iARB(gSampler, 1);

	BlendTex->bindTexture(GL_TEXTURE1);

    glDrawArrays( GL_TRIANGLES, 0, 6);
    
    glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}

int main (int argc, char * argv[])
{
	glm::mat4 mat;
	float axis[] = { 0.7f, 0.7f, 0.7f }; // initial model rotation
    float angle = 0.8f;

	double FT  = 0;

	double starting = 0.0;
	double ending   = 0.0;
	int rate = 0;
	int fr = 0;

	
	x = new BlendModes();
	x->blend_mode = BlendModes::Normal;

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

	std::string vendor ="Title : Chapter-19   Vendor : " + stream1.str() + "   Renderer : " +stream2.str();

	const char *tit = vendor.c_str();
	SDL_SetWindowTitle(gWindow, tit);

	
	 // Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	initQuad();

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
    time = SDL_GetTicks()/1000.0f;
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
    
	while (running) {
		currentTime = SDL_GetTicks();
		float dTime = float(currentTime - lastTime) / 1000.0f;
		lastTime = (float)currentTime;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			event_handler(&event);
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

		displayQuad();

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
    }

	//close OpenGL window and  terminate ImGui and SDL2
	close();
    
    exit(EXIT_SUCCESS);
}

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
#include "HDRI_frame.h"


//update
SDL_Window		*gWindow = NULL;
SDL_GLContext	gContext;
float bgColor[] = { 0.0f, 0.0f, 0.0f, 0.1f };
double FPS = 0;
int rate = 0;

// global variables
int         windowWidth=1024, windowHeight=768;

GLuint      program3Dmodel,program,programQuad;
GLuint      vao3Dmodel,vao,vaoQuad;
GLuint      buffer3Dmodel,buffer,bufferQuad;

GLuint		MV_uniform3D, MVP_uniform3D,Normal_uniform3D,MV_uniform ,Normal_uniform;
GLuint		MVP_uniform,MVP_uniformQuad,MVTR_uniform3D,uniformDr; 

GLuint		gSampler;
GLuint		texID;

Texture	    *pTexture = NULL;
Mesh		*m		  = NULL;

const       int         NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
int                     Index = 0;

bool        wireFrame = false;
bool		camera    = false;

typedef     glm::vec4   color4;
typedef     glm::vec4   point4;

//Modelling arrays
point4      points[NumVertices];
glm::vec2	tex_coords[NumVertices];
color4      colors[NumVertices];
glm::vec3   normals[NumVertices];

glm::vec3 pos = glm::vec3( 0.0f, 0.0f , 10.0f );

const glm::vec3 cpos = glm::vec3(0.0f,0.0f,0.0f);

float horizAngle = 3.14f;
float verticAngle = 0.0f;
glm::vec3 direction = glm::vec3(cos(verticAngle) * sin(horizAngle), sin(verticAngle), cos(verticAngle) * cos(horizAngle));

float speedo = 3.0f;
float mouseSpeedo = 0.005f;

int	  xpos = 0,ypos = 0;

glm::vec3 LightPos  = glm::vec3(2.0f,5.0f,10.0f);

glm::vec3 BaseColor = glm::vec3(0.6f,0.6f,0.6f);

float	  MixRatio  = 0.2f;
int		  divideFactor = 2;

int timesc = 0;
// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -20.5, -20.5, 20.5, 1.0 ),
    point4( -20.5,  20.5, 20.5, 1.0 ),
    point4( 20.5,  20.5, 20.5, 1.0 ),
    point4( 20.5, -20.5, 20.5, 1.0 ),
    point4( -20.5, -20.5, -20.5, 1.0 ),
    point4( -20.5,  20.5, -20.5, 1.0 ),
    point4( 20.5,  20.5, -20.5, 1.0 ),
    point4( 20.5, -20.5, -20.5, 1.0 )
};


// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

point4 QuadVertices[] = {
	point4(-1.0f, -1.0f, 0.0f,1.0f),
	point4( 1.0f, -1.0f, 0.0f,1.0f),
	point4(-1.0f,  1.0f, 0.0f,1.0f),
	point4(-1.0f,  1.0f, 0.0f,1.0f),
	point4( 1.0f, -1.0f, 0.0f,1.0f),
	point4( 1.0f,  1.0f, 0.0f,1.0f)
};

color4 QuadColors[] = {
	color4(1.0f,0.5f,1.0f,1.0f),
	color4(1.0f,0.5f,1.0f,1.0f),
	color4(1.0f,0.5f,1.0f,1.0f),
	color4(1.0f,0.5f,1.0f,1.0f),
	color4(1.0f,0.5f,1.0f,1.0f),
	color4(1.0f,0.5f,1.0f,1.0f)
};

bool initializeFBO();
void setupFBO();

float Exposure = 0.0f;
float Gamma = 1.0f/2.2f;
float Defog = 0.0f;
glm::vec3 FogColor = glm::vec3(1.0f,1.0f,1.0f);

float SpecularPercent = 1.0;
float DiffusePercent  = 1.0;

unsigned int cubeMap,diffMap;

/************** FBO ********************/
GLuint FBO;
GLuint DepthBuffer;

/**************************************/

HDRI_frame	hdrImg;

GLuint TextureIDs[3];

const char * hdr_map = "./Textures/St.Peter's_Basilica/stpeters_cross.hdr";
const char * hdr_diff_map = "./Textures/St.Peter's_Basilica/stpeters_diffuse_cross.hdr";

const char * hdr_map_fiz = "./Textures/Uffizi_Gallery/uffizi_cross.hdr";
const char * hdr_diff_map_fiz = "./Textures/Uffizi_Gallery/uffizi_cross_diffuse.hdr";

const char * hdr_map_gc = "./Textures/Grand_Canyon/Grand_Canyon.hdr";
const char * hdr_diff_map_gc = "./Textures/Grand_Canyon/Grand_Canyon_diff.hdr";

const char * hdr_map_gr = "./Textures/Washington-Gold-Room/Gold_Room.hdr";
const char * hdr_diff_map_gr = "./Textures/Washington-Gold-Room/Gold_Room_diff.hdr";

struct Map{

	enum HdrMap {st_Peter , ufizi , grandc , gr};
	HdrMap map_val;
};
//update
Map *x;

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
		gWindow = SDL_CreateWindow("Chapter12", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		divideFactor = 4;
#else
		gWindow = SDL_CreateWindow("Chapter12", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
	initializeFBO();
	setupFBO();
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
	ImGui::Begin("Editor");
	ImGui::SetWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);
	//ImGui::SetWindowPos(ImVec2(10, 10), ImGuiSetCond_Once);

	if (ImGui::TreeNode("IBL control"))
	{
		ImGui::ColorEdit3("Base Color", (float*)&BaseColor);
		ImGui::Separator();
		ImGui::SliderFloat("Specular Percent", &SpecularPercent, 0.1, 5.0, "%.2f");
		ImGui::Separator();
		ImGui::SliderFloat("Diffuse Percent", &DiffusePercent, 0.1, 5.0, "%.2f");
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Frame Rate"))
	{
		ImGui::TextColored(ImVec4(0.3f, 0.4f, 0.0f, 1.0f), "MS per 1 Frame %.2f", FPS);
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.0f, 0.4f, 0.4f, 1.0f), "Frames Per Second %d", rate);
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode("Exposure & Gamma Control"))
	{
		ImGui::SliderFloat("Exposure", &Exposure, -10.0, 10.0, "%.2f");
		ImGui::Separator();
		ImGui::SliderFloat("Gamma", &Gamma, 0.0, 1.0, "%.2f");
		ImGui::Separator();
		ImGui::SliderFloat("Defog", &Defog, 0.0, 0.1, "%.2f");
		ImGui::Separator();
		ImGui::ColorEdit3("FogColor", (float*)&FogColor);
		ImGui::TreePop();
	}
	ImGui::Separator();

	const char * items[] = {"St.Peter Basilica", "Uffizi Gallery", "Grand Canyon", "Washington G.Room"};
	static int current_item = 0;
	ImGui::Combo("Scene Management", &current_item, items, 4);
	
	if (current_item == 0)
		x->map_val = Map::st_Peter;
	else if (current_item == 1)
		x->map_val = Map::ufizi;
	else if (current_item == 2)
		x->map_val = Map::grandc;
	else
		x->map_val = Map::gr;

	ImGui::End();

	ImGui::Begin("Camera Editor");
	//ImGui::SetWindowPos(ImVec2(10, 220), ImGuiSetCond_Once);
	ImGui::SetWindowSize(ImVec2(340, 260), ImGuiSetCond_Once);

	ImGui::Checkbox("Enable Camera", &camera);
	ImGui::NewLine();
	ImGui::Separator();

	ImGui::SliderFloat("Position X", &pos.x, -30.0f, 30.0f, "%.2f");
	ImGui::SliderFloat("Position Y", &pos.y, -30.0f, 30.0f, "%.2f");
	ImGui::SliderFloat("Position Z", &pos.z, -30.0f, 30.0f, "%.2f");

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();

	ImGui::SliderFloat("Direction X", &direction.x, -10.0f, 10.0f, "%.2f");
	ImGui::SliderFloat("Direction Y", &direction.y, -10.0f, 10.0f, "%.2f");
	ImGui::SliderFloat("Direction Z", &direction.z, -10.0f, 10.0f, "%.2f");
	ImGui::End();
}

void LoadMap(const char * hpmap,GLuint tex,int i)
{
	std::cout<<"\ntex : "<<tex<<" i : "<<i<<"\n";
	hdrImg.SetTextureUnit(i);
	hdrImg.SetTextureImage(tex);
	hdrImg.ReadHDR(hpmap);
	hdrImg.CreateCubeMapFaces();
	hdrImg.BindCubeMapTexture();
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

// quad generates two triangles for each face and assigns colors
//    to the vertices
void quad( int a, int b, int c, int d )
{
    //specify temporary vectors along each quad's edge in order to compute the face
    // normal using the cross product rule
    glm::vec3 u = (vertices[b]-vertices[a]).xyz();
    glm::vec3 v = (vertices[c]-vertices[b]).xyz();
    glm::vec3 norm = glm::cross(u, v);
    glm::vec3 normal= glm::normalize(norm);
    
	normals[Index]=normal;colors[Index] = vertex_colors[a]; points[Index] = vertices[a];tex_coords[Index] = glm::vec2( 0.0 , 0.0); Index++;
    normals[Index]=normal;colors[Index] = vertex_colors[b]; points[Index] = vertices[b];tex_coords[Index] = glm::vec2( 0.0 , 0.5); Index++;
    normals[Index]=normal;colors[Index] = vertex_colors[c]; points[Index] = vertices[c];tex_coords[Index] = glm::vec2( 0.5 , 0.0); Index++;
    normals[Index]=normal;colors[Index] = vertex_colors[a]; points[Index] = vertices[a];tex_coords[Index] = glm::vec2( 0.0 , 0.5); Index++;
    normals[Index]=normal;colors[Index] = vertex_colors[c]; points[Index] = vertices[c];tex_coords[Index] = glm::vec2( 0.5 , 0.5); Index++;
    normals[Index]=normal;colors[Index] = vertex_colors[d]; points[Index] = vertices[d];tex_coords[Index] = glm::vec2( 0.5 , 0.0); Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
	
}

void initCube()
{
    //generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    colorcube();
    
    // Load shaders and use the resulting shader program
    program = LoadShaders( "./Shaders/vshaderSkybox.vert", "./Shaders/fshaderSkybox.frag" );
    glUseProgram( program );
    
    // Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals) + sizeof(tex_coords),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), sizeof(colors) , colors );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(colors) ,sizeof(tex_coords) , tex_coords );

    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );

	GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(points)) );
    
    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(points) + sizeof(normals)) );

	GLuint vText = glGetAttribLocation( program, "vTexture" );
    glEnableVertexAttribArray( vText );
	glVertexAttribPointer( vText, 2, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(points) + sizeof(normals) + sizeof(colors)) );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void displayCube(glm::mat4 &tsl,glm::vec3 constpos,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
    glUseProgram(program);
    glBindVertexArray(vao);
    
    glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	MV_uniform = glGetUniformLocation(program, "MV_mat");
	MVP_uniform = glGetUniformLocation(program, "MVP_mat");
	Normal_uniform = glGetUniformLocation(program, "Normal_mat");
	
	//Calculation of ModelView Matrix
	glm::mat4 model_mat = tsl;
	glm::mat4 view_mat = glm::lookAt(constpos,directionv,upv);
	glm::mat4 MV_mat = view_mat * model_mat;
	glUniformMatrix4fv(MV_uniform,1, GL_FALSE, glm::value_ptr(MV_mat));

	//Calculation of Normal Matrix
	glm::mat3 Normal_mat = glm::transpose(glm::inverse(glm::mat3(MV_mat)));
	glUniformMatrix3fv(Normal_uniform,1, GL_FALSE, glm::value_ptr(Normal_mat));

	//Calculation of ModelViewProjection Matrix
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_mat = glm::perspective(45.0f, aspect,0.1f,100.0f);
	glm::mat4 MVP_mat = projection_mat * MV_mat;
	glUniformMatrix4fv(MVP_uniform, 1, GL_FALSE, glm::value_ptr(MVP_mat));


	glUniform1i(glGetUniformLocation(program, "CubeMapTex"), 0);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureIDs[2]);

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );


    glPopAttrib();
    glBindVertexArray(0);
	glUseProgram(0);

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

void display3Dmodel(glm::mat4 &tsl,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv,int im)
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
	MVTR_uniform3D = glGetUniformLocation(program3Dmodel, "MV_matTR");
	MVP_uniform3D = glGetUniformLocation(program3Dmodel, "MVP_mat");
	Normal_uniform3D = glGetUniformLocation(program3Dmodel, "Normal_mat");

	//Calculation of ModelView Matrix
	glm::mat4 model_mat = tsl;
	glm::mat4 view_mat = glm::lookAt(positionv,positionv + directionv,upv);

	glm::mat4 MV_mat = view_mat * model_mat;
	glUniformMatrix4fv(MV_uniform3D,1, GL_FALSE, glm::value_ptr(MV_mat)); 

	glm::mat4 MV_matTR = glm::transpose(MV_mat);
	glUniformMatrix4fv(MVTR_uniform3D,1, GL_FALSE, glm::value_ptr(MV_matTR)); 


	glUniform3fv(glGetUniformLocation(program3Dmodel,"BaseColor"),1,glm::value_ptr(BaseColor));
	glUniform1f(glGetUniformLocation(program3Dmodel,"SpecularPercent"),SpecularPercent);
	glUniform1f(glGetUniformLocation(program3Dmodel,"DiffusePercent"),DiffusePercent);

	glUniform1i(glGetUniformLocation(program3Dmodel,"Mode"),im);

	//Calculation of Normal Matrix
	glm::mat3 Normal_mat = glm::transpose(glm::inverse(glm::mat3(MV_mat)));
	glUniformMatrix3fv(Normal_uniform3D,1, GL_FALSE, glm::value_ptr(Normal_mat));

	//Calculation of ModelViewProjection Matrix
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_mat = glm::perspective(45.0f, aspect,0.1f,100.0f);
	glm::mat4 MVP_mat = projection_mat * MV_mat;
	glUniformMatrix4fv(MVP_uniform3D, 1, GL_FALSE, glm::value_ptr(MVP_mat));


	glUniform1i(glGetUniformLocation(program3Dmodel, "SpecularEnvMap"), 0);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureIDs[0]);

	glUniform1i(glGetUniformLocation(program3Dmodel, "DiffuseEnvMap"), 1);

	glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureIDs[1]);


	m->render(); 

	if(timesc < 1)
	{
		checkActiveUniforms();
	}
	timesc++;

	glPopAttrib();
    glBindVertexArray(0);
	glUseProgram(0);
}

void initQuad()
{
	//generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoQuad);
    glBindVertexArray(vaoQuad);

	// Load shaders and use the resulting shader program
    programQuad = LoadShaders( "./Shaders/quad.vert", "./Shaders/quad.frag" );
    glUseProgram( programQuad );

	// Create and initialize a buffer object on the server side (GPU)
    //GLuint buffer;
    glGenBuffers( 1, &bufferQuad );
    glBindBuffer( GL_ARRAY_BUFFER, bufferQuad );
    glBufferData( GL_ARRAY_BUFFER, sizeof(QuadVertices) + sizeof(QuadColors),NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(QuadVertices), QuadVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(QuadVertices), sizeof(QuadColors), QuadColors );
    
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( programQuad, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
    
    GLuint vColor = glGetAttribLocation( programQuad, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(QuadVertices)) );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
    
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void displayQuad(glm::mat4 &md,glm::vec3 positionv,glm::vec3 directionv,glm::vec3 upv)
{
	glUseProgram(programQuad);
	glBindVertexArray(vaoQuad);

	glDisable(GL_CULL_FACE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (wireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	MVP_uniformQuad = glGetUniformLocation(programQuad, "MVP_mat");

	// Calculation of ModelView Matrix
	glm::mat4 model_matq = md;
	glm::mat4 view_matq = glm::lookAt(positionv,positionv + directionv,upv);

	glm::mat4 MV_matq = view_matq * model_matq;

	// Calculation of ModelViewProjection Matrix
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_matq = glm::perspective(45.0f, aspect,0.1f,100.0f);
	glm::mat4 MVP_matq = projection_matq * MV_matq;
	glUniformMatrix4fv(MVP_uniformQuad, 1, GL_FALSE, glm::value_ptr(MVP_matq));

	glUniform1f(glGetUniformLocation(programQuad,"exposure"),Exposure);
	glUniform1f(glGetUniformLocation(programQuad,"Gamma"),Gamma);
	glUniform1f(glGetUniformLocation(programQuad,"Defog"),Defog);

	glUniform3fv(glGetUniformLocation(programQuad,"FogColor"),1,glm::value_ptr(FogColor));

	texID =  glGetUniformLocationARB(programQuad, "texID");
	glUniform1i(texID, 2);

	glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, TextureIDs[2]);
    glGenerateMipmap(GL_TEXTURE_2D);

    glDrawArrays( GL_TRIANGLES, 0, 6);
    
    glPopAttrib();
    glBindVertexArray(0);
	glUseProgram(0);
}

bool initializeFBO()
{
	//generate fbo objects
  glGenFramebuffers(1, &FBO);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  //death depthbuffer
  glGenRenderbuffers(1, &DepthBuffer);

   //bind renderbuffer to depthbuffer variabler
  glBindRenderbuffer(GL_RENDERBUFFER, DepthBuffer);

  //select what type of renderbuffer to use
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);

  //attach depth to framebuffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBuffer);

  //check status of frame buffer
  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FBO error, status: 0x%x\n", Status);
        return false;
    }
  else
  {
	  return true;
  }

}

void setupFBO()
{
	glBindTexture(GL_TEXTURE_2D, TextureIDs[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureIDs[2], 0);
}

int main (int argc, char * argv[])
{
	x = new Map();
	x->map_val = Map::st_Peter;

	// Current time
	double time = 0;

	double FT = 0;
	
	int fr = 0;

	double starting = 0.0;
	double ending   = 0.0;

    // initialise GLFW
    int running = GL_TRUE;
    
    if (!initSDL()) {
        exit(EXIT_FAILURE);
    }
    

	 // Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GLU_CULLING);
	initializeFBO();

	glGenTextures(3, &TextureIDs[0]);

	LoadMap(hdr_map,TextureIDs[0],0);
	LoadMap(hdr_diff_map,TextureIDs[1],1);

	initCube();
	init3Dmodel();
	initQuad();

	setupFBO();

	int init_map = 0;
	float spee = 1.0;
	bool go = true;
	// Initialize time
    time = SDL_GetTicks() / 1000.0f;
	uint32 currentTime;
	uint32 lastTime = 0U;

	int Frames = 0;
	double LT = SDL_GetTicks() / 1000.0f;
	starting = SDL_GetTicks() / 1000.0f;

	// Initialize camera position/direction values.
	glm::vec3 right = glm::vec3(sin(horizAngle - 3.14f / 2.0f), 0, cos(horizAngle - 3.14f / 2.0f));
	glm::vec3 up = glm::cross(right, direction);

#ifdef __APPLE__
	int *w = (int*)malloc(sizeof(int));
	int *h = (int*)malloc(sizeof(int));
#endif

	while (running) {

		if(x->map_val == 0 && (init_map == 1 || init_map == 2 || init_map == 3))
		{
			std::cout<<"\nChanges applied!\n";
			LoadMap(hdr_map,TextureIDs[0],0);
			LoadMap(hdr_diff_map,TextureIDs[1],1);
			init_map = 0;
		}
		else if(x->map_val == 1 && (init_map == 0 || init_map == 2 || init_map == 3))
		{
			std::cout<<"\nChanges applied!\n";
			LoadMap(hdr_map_fiz,TextureIDs[0],0);
			LoadMap(hdr_diff_map_fiz,TextureIDs[1],1);
			init_map = 1;
		}
		else if(x->map_val == 2 && (init_map == 0 || init_map == 1 || init_map == 3))
		{
			std::cout<<"\nChanges applied!\n";
			LoadMap(hdr_map_gc,TextureIDs[0],0);
			LoadMap(hdr_diff_map_gc,TextureIDs[1],1);
			init_map = 2;
		}
		else if(x->map_val == 3 && (init_map == 0 || init_map == 1 || init_map == 2))
		{
			std::cout<<"\nChanges applied!\n";
			LoadMap(hdr_map_gr,TextureIDs[0],0);
			LoadMap(hdr_diff_map_gr,TextureIDs[1],1);
			init_map = 3;
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		if (camera)
		{
			direction = glm::vec3(cos(verticAngle) * sin(horizAngle), sin(verticAngle), cos(verticAngle) * cos(horizAngle));
			right = glm::vec3(sin(horizAngle - 3.14f / 2.0f), 0, cos(horizAngle - 3.14f / 2.0f));
			up = glm::cross(right, direction);
		}

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
					pos -= direction * dTime* speedo;

				if (event.key.keysym.sym == SDLK_RIGHT)
					pos += right *  dTime * speedo;

				if (event.key.keysym.sym == SDLK_LEFT)
					pos -= right * dTime * speedo;

				if (event.key.keysym.sym == SDLK_F1 && go == true)
					go = false;
				else
					go = true;
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
			SDL_WarpMouseInWindow(gWindow, windowWidth / divideFactor, windowHeight / divideFactor);
			SDL_ShowCursor(0);

			horizAngle  += mouseSpeedo * float(windowWidth/divideFactor - xpos );
			verticAngle += mouseSpeedo * float( windowHeight/divideFactor - ypos );
		}


		glm::mat4 cube1,cube2,cube3 = glm::mat4();
		
		displayCube(cube1,cpos,pos,direction,up);

		//cube1=glm::scale(cube1,glm::vec3(0.1,0.1,0.1));
		//cube2=glm::scale(cube2,glm::vec3(0.1,0.1,0.1));
		//cube3=glm::scale(cube3,glm::vec3(0.1,0.1,0.1));

		cube2=glm::translate(cube2,glm::vec3(3.0,0.0,-3.0));
		cube3=glm::translate(cube3,glm::vec3(-3.0,0.0,-3.0));

		//cube1=glm::rotate(cube1,spee,glm::vec3(0.0,1.0,0.0));
		//cube2=glm::rotate(cube2,spee,glm::vec3(0.0,1.0,0.0));
		//cube3=glm::rotate(cube3,spee,glm::vec3(0.0,1.0,0.0));

		if(go == true)
		{
			spee += 0.1;
		}

		display3Dmodel(cube1,pos,direction,up,0);
		display3Dmodel(cube2,pos,direction,up,1);
		display3Dmodel(cube3,pos,direction,up,2);

		//draw to screen instead of fbo
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	    glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 mat = glm::mat4();

		displayQuad(mat,pos,direction,up);
		

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		displayGui();
        
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

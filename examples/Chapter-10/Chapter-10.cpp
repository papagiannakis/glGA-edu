// basic STL streams
#include <iostream>

// GLEW lib
// http://glew.sourceforge.net/basic.html
#include <GL/glew.h>

// update 04/08/16
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




// global variables
int         windowWidth=1024, windowHeight=768;

GLuint      program3Dmodel,program;
GLuint      vao3Dmodel,vao;
GLuint      buffer3Dmodel,buffer;

GLuint		MV_uniform3D, MVP_uniform3D,Normal_uniform3D,MV_uniform ,Normal_uniform;
GLuint		MVP_uniform; 

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

glm::vec3 BaseColor = glm::vec3(1.0f,1.0f,1.0f);

float	  MixRatio  = 0.2f;


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

unsigned int cubeMap;

//update 
SDL_Window		*gWindow = NULL;
SDL_GLContext	gContext;
float bgColor[] = { 0.0f, 0.0f, 0.0f, 0.1f };

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

#ifdef __APPLE__
		SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
#endif
		//Create Window
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

#ifdef __APPLE__
		gWindow = SDL_CreateWindow("chapter10", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );
#else
		gWindow = SDL_CreateWindow("chapter10", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
	ImGui::Begin("Main");
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

	ImGui::ColorEdit3("Background color", bgColor);

	ImGui::Separator();

	if (ImGui::TreeNode("Light Position"))
	{
		ImGui::BulletText("X,\t Y,\t Z");
		ImGui::SliderFloat3("", (float*)&LightPos, -100.0f, 100.0f, "%.2f");
		ImGui::TreePop();
	}
	
	ImGui::Separator();

	ImGui::ColorEdit3("Base Color", (float*)&BaseColor);

	ImGui::Separator();

	ImGui::InputFloat("Mix Ratio", &MixRatio, 0.005, 005, 2);

	if (MixRatio < 0.0f)
		MixRatio = 0.0f;

	if (MixRatio > 0.9f)
		MixRatio = 0.9f;

	ImGui::End();

	// Camera Position/Directions editor.
	ImGui::Begin("Camera Editor");
	ImGui::SetWindowSize(ImVec2(340, 260), ImGuiSetCond_Once);
	ImGui::SetWindowPos(ImVec2(10, 220), ImGuiSetCond_Once);

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

unsigned int LoadMap(std::string* filenames)
{
	unsigned int tex;
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP,tex);

	for(int i=0;i<6;i++)
	{
		Texture	*cTexture = new Texture(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,filenames[i]);
		cTexture->loadTexture();
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP,0);
	return tex;
}

void checkActiveUniforms()
{
	GLint nUniforms, maxLen;
	glGetProgramiv( program, GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLen);
	glGetProgramiv( program, GL_ACTIVE_UNIFORMS,&nUniforms);
	GLchar * name = (GLchar *) malloc( maxLen );
	GLint size, location;
	GLsizei written;
	GLenum type;

	printf(" Location | Name\n");
	printf("------------------------------------------------\n");

	for( int i = 0; i < nUniforms; ++i ) {

		glGetActiveUniform( program, i, maxLen, &written,&size, &type, name );
		location = glGetUniformLocation(program, name);
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

	std::string filename[6];
	filename[0]="./Textures/SkyTex/right.bmp";
	filename[1]="./Textures/SkyTex/left.bmp";
	filename[2]="./Textures/SkyTex/top.bmp";
	filename[3]="./Textures/SkyTex/bottom.bmp";
	filename[4]="./Textures/SkyTex/front.bmp";
	filename[5]="./Textures/SkyTex/back.bmp";

	cubeMap=LoadMap(filename);

    colorcube();
    
    // Load shaders and use the resulting shader program
    program = LoadShaders( "vshaderC1.vert", "fshaderCube2.frag" );
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
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

   
	if(timesc < 1)
		checkActiveUniforms();
	timesc++;


    glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);

}

void init3Dmodel()
{

	

	GLuint m_Buffers[4];
	m = new Mesh();
	m->loadMesh("./Models/box1/models/CargoCube01.dae");
	//m->loadMesh("./Models/sphere.obj");

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

	MV_uniform3D = glGetUniformLocation(program3Dmodel, "MV_mat");
	MVP_uniform3D = glGetUniformLocation(program3Dmodel, "MVP_mat");
	Normal_uniform3D = glGetUniformLocation(program3Dmodel, "Normal_mat");

	//Calculation of ModelView Matrix
	glm::mat4 model_mat = tsl;
	glm::mat4 view_mat = glm::lookAt(positionv,positionv + directionv,upv);

	glm::mat4 MV_mat = view_mat * model_mat;
	glUniformMatrix4fv(MV_uniform3D,1, GL_FALSE, glm::value_ptr(MV_mat)); 

	glUniform3fv(glGetUniformLocation(program3Dmodel,"LightPosition"),1,glm::value_ptr(LightPos));
	glUniform3fv(glGetUniformLocation(program3Dmodel,"BaseColor"),1,glm::value_ptr(BaseColor));
	glUniform1f(glGetUniformLocation(program3Dmodel,"MixRatio"),MixRatio);

	//Calculation of Normal Matrix
	glm::mat3 Normal_mat = glm::transpose(glm::inverse(glm::mat3(MV_mat)));
	glUniformMatrix3fv(Normal_uniform3D,1, GL_FALSE, glm::value_ptr(Normal_mat));

	//Calculation of ModelViewProjection Matrix
	float aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
	glm::mat4 projection_mat = glm::perspective(45.0f, aspect,0.1f,100.0f);
	glm::mat4 MVP_mat = projection_mat * MV_mat;
	glUniformMatrix4fv(MVP_uniform3D, 1, GL_FALSE, glm::value_ptr(MVP_mat));


	glUniform1i(glGetUniformLocation(program3Dmodel, "CubeMapTex"), 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);


	m->render(); 

	//if(timesc < 1)
		//checkActiveUniforms();
	//timesc++;
	glPopAttrib();
    glBindVertexArray(0);
	//glUseProgram(0);
}

int main (int argc, char * argv[])
{
	// Current time
	uint32 time = 0U;

    // initialise GLFW
    int running = GL_TRUE;
    
    if (!initSDL()) {
        exit(EXIT_FAILURE);
    }


	 // Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

    glActiveTexture(GL_TEXTURE0);
    
	initCube();
	//initPyramid();
	init3Dmodel();

	// Initialize time
    time = SDL_GetTicks();
	uint32 currentTime;
	uint32 lastTime = 0U;

	// Initiliaze camera values. *direction is global*.
	glm::vec3 right = glm::vec3(sin(horizAngle - 3.14f / 2.0f), 0, cos(horizAngle - 3.14f / 2.0f));
	glm::vec3 up = glm::cross(right, direction);

#ifdef __APPLE__
	int *w = (int*)malloc(sizeof(int));
	int *h = (int*)malloc(sizeof(int));
#endif

	while (running) {
		if (camera == true)
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
					pos += direction * dTime * speedo;

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
		glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        glClearColor( bgColor[0], bgColor[1], bgColor[2], bgColor[3]); //black color

		if(camera == true)
		{
			SDL_GetMouseState(&xpos, &ypos);
			SDL_WarpMouseInWindow(gWindow, windowWidth / 2, windowHeight / 2);
			SDL_ShowCursor(0);

			horizAngle  += mouseSpeedo * float(windowWidth/2 - xpos );
			verticAngle += mouseSpeedo * float( windowHeight/2 - ypos );
		}

		glm::mat4 cube1 = glm::mat4();
		displayCube(cube1,cpos,pos,direction,up);

		//displayPyramid(pos,direction,up);
		display3Dmodel(cube1,pos,direction,up);
		
		
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

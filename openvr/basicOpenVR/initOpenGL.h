#pragma once
/*
*	Simple header.
*	description: main.cpp
*/


// Contains all includes & global variables
// In order to keep this file "clean"
#include "glob.h"



// Function Prototypes
SDL_Window* init(int width, int height, const std::string& title);

bool		event_handler(SDL_Event* event);

void 		initBuffers();

void		displayCube();

void		drawSky(int windowWidth, int windowHeight, float nearPlaneZ, float farPlaneZ, const float* cameraToWorldMatrix, const float* projectionMatrixInverse, const float* light);

void 		loadBMP(const std::string& filename, int& width, int& height, int& channels, std::vector<std::uint8_t>& data);

void		close();

GLuint		compileShaderStage(GLenum stage, const std::string& source);

GLuint		createShaderProgram(const std::string& vertexShaderSource, const std::string& pixelShaderSource);



// Function implementation

SDL_Window* init(int width, int height, const char* title)
{
	SDL_Window 		*window = NULL;

	// Basic Setup
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "SDL initialized succesfully!" << std::endl;

	// Specify basic OpenGL attributes
	// OpenGL 4.1 version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Create Window
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Window created succesfully!" << std::endl;

	// Create OpenGL context
	context = SDL_GL_CreateContext(window);
	if(context == NULL)
	{
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
	}

	// Initialize Glew
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK)
	{
		std::cout << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Glew initialized succesfully!" << std::endl;
	std::cout << "Using GLEW version: " << glewGetString(GLEW_VERSION);

	fprintf(stderr, "GPU: %s (OpenGL version %s)\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

	// Use vSync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		std::cout << "Warning: Unable to set Vsync! SDL Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "vSync set succesfully!" << std::endl;

	

	return window;
}

bool		event_handler(SDL_Event* event)
{
	static bool inDrag = false;
	
	if (bodyTranslation.y < 0.01f)
	{
		bodyTranslation.y = 0.01f;
	}
	switch (event->type)
	{
	case SDL_KEYDOWN:
	{
		//std::cout << "Buttons pressed\n";
		if (event->key.keysym.sym == SDLK_w)
		{
			//std::cout << "W was pressed\n";
			bodyTranslation += Vector3(temp * Vector4(0, 0, -cameraMoveSpeed, 0));
			return true;
		}

		if (event->key.keysym.sym == SDLK_s)
		{
			//std::cout << "S was pressed\n";
			bodyTranslation += Vector3(temp * Vector4(0, 0, +cameraMoveSpeed, 0));
			return true;
		}

		if (event->key.keysym.sym == SDLK_a)
		{
			//std::cout << "A was pressed\n";
			bodyTranslation += Vector3(temp * Vector4(-cameraMoveSpeed, 0, 0, 0));
			return true;
		}

		if (event->key.keysym.sym == SDLK_d)
		{
			//std::cout << "D was pressed\n";
			bodyTranslation += Vector3(temp * Vector4(+cameraMoveSpeed, 0, 0, 0));
			return true;
		}
	
		if (event->key.keysym.sym == SDLK_c)
		{
			//std::cout << "C was pressed\n";
			bodyTranslation.y -= cameraMoveSpeed;
			return true;
		}

		if (event->key.keysym.sym == SDLK_z)
		{
			//std::cout << "z was pressed\n";
			bodyTranslation.y += cameraMoveSpeed;
			return true;
		}
		
		if (event->key.keysym.sym == SDLK_SPACE)
		{
			//std::cout << "SPACE was pressed\n";
			bodyTranslation.y += cameraMoveSpeed;
			return true;
		}

		if (event->key.keysym.sym == SDLK_m)
		{
			//std::cout << "m was pressed\n";
			if (!camera)
				camera = true;
			else
				camera = false;
			return true;
		}
		return true;
	}
	case SDL_MOUSEMOTION:
	{
		if (camera)
		{
			//std::cout << "camera movement";
			static double startX, startY;
			double currentX, currentY;

			currentX = event->motion.x;
			currentY = event->motion.y;

			bodyRotation.y -= float(currentX - startX) * cameraTurnSpeed;
			bodyRotation.x -= float(currentY - startY) * cameraTurnSpeed;

			
			startX = currentX;
			startY = currentY;
		}
	}
	return true;
	}
}

void		initBuffers()
{
	GLuint vao; glGenVertexArrays(1, &vao); glBindVertexArray(vao);

#ifdef _VR
	assert(hmd);
#endif

	//////////////////////////////////////////////////////////////////////
    // Allocate the frame buffer. This code allocates one framebuffer per eye.
    // That requires more GPU memory, but is useful when performing temporal 
    // filtering or making render calls that can target both simultaneously.
    glGenFramebuffers(numEyes, framebuffer);

    
    glGenTextures(numEyes, colorRenderTarget);
    glGenTextures(numEyes, depthRenderTarget);
    for (int eye = 0; eye < numEyes; ++eye) {
        glBindTexture(GL_TEXTURE_2D, colorRenderTarget[eye]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebufferWidth, framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, depthRenderTarget[eye]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, framebufferWidth, framebufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[eye]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorRenderTarget[eye], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, depthRenderTarget[eye], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /////////////////////////////////////////////////////////////////
    // Load vertex array buffers
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Shape::position), Shape::position, GL_STATIC_DRAW);

    glGenBuffers(1, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Shape::texCoord), Shape::texCoord, GL_STATIC_DRAW);

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Shape::normal), Shape::normal, GL_STATIC_DRAW);

    glGenBuffers(1, &tangentBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Shape::tangent), Shape::tangent, GL_STATIC_DRAW);

    const int numVertices = sizeof(Shape::position) / sizeof(Shape::position[0]);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Shape::index), Shape::index, GL_STATIC_DRAW);
    

    /////////////////////////////////////////////////////////////////////
    // Create the main shader
    //const GLuint shader = createShaderProgram(loadTextFile("min.vrt"), loadTextFile("min.pix"));
    shader = LoadShaders("min.vert", "min.frag");

	positionAttribute = glGetAttribLocation(shader, "position");
	normalAttribute = glGetAttribLocation(shader, "normal");
	texCoordAttribute = glGetAttribLocation(shader, "texCoord");
	tangentAttribute = glGetAttribLocation(shader, "tangent");
	colorTextureUniform = glGetUniformLocation(shader, "colorTexture");

	uniformBlockIndex = glGetUniformBlockIndex(shader, "Uniform");

    // Binding points for attributes and uniforms discovered from the shader
    glUniformBlockBinding(shader, uniformBlockIndex, uniformBindingPoint);

    
    glGenBuffers(1, &uniformBlock);
    {
        // Allocate space for the uniform block buffer
        GLint uniformBlockSize;
        glGetActiveUniformBlockiv(shader, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBlock);
        glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, nullptr, GL_DYNAMIC_DRAW);
    }

	
#   ifdef _DEBUG
    {
        GLint debugNumUniforms = 0;
        glGetProgramiv(shader, GL_ACTIVE_UNIFORMS, &debugNumUniforms);
        for (GLint i = 0; i < debugNumUniforms; ++i) {
            GLchar name[1024];
            GLsizei size = 0;
            GLenum type = GL_NONE;
            glGetActiveUniform(shader, i, sizeof(name), nullptr, &size, &type, name);
            std::cout << "Uniform #" << i << ": " << name << "\n";
        }
        assert(debugNumUniforms >= numBlockUniforms);
    }
#   endif

    // Map uniform names to indices within the block
    glGetUniformIndices(shader, numBlockUniforms, uniformName, uniformIndex);
    assert(uniformIndex[0] < 10000);

    // Map indices to byte offsets
    glGetActiveUniformsiv(shader, numBlockUniforms, uniformIndex, GL_UNIFORM_OFFSET, uniformOffset);
    assert(uniformOffset[0] >= 0);

    // Load a texture map
    {
		glGenTextures(1, &colorTexture);
		pTexture = NULL;
		pTexture = new Texture(GL_TEXTURE_2D, "color.bmp");
		if (!pTexture->loadTexture())
		{
			std::cerr << " Texture could not be succesfully loaded! " << std::endl;
		}
		
    }

	/*
    {
        glGenSamplers(1, &trilinearSampler);
        glSamplerParameteri(trilinearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(trilinearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
	*/
}

void		displayCube()
{
	Matrix4x4 eyeToHead[numEyes], projectionMatrix[numEyes], headToBodyMatrix;
#   ifdef _VR
	getEyeTransformations(hmd, trackedDevicePose, nearPlaneZ, farPlaneZ, headToBodyMatrix.data, eyeToHead[0].data, eyeToHead[1].data, projectionMatrix[0].data, projectionMatrix[1].data);
#   else
	projectionMatrix[0] = Matrix4x4::perspective(float(framebufferWidth), float(framebufferHeight), nearPlaneZ, farPlaneZ, verticalFieldOfView);
#   endif

    // printf("float nearPlaneZ = %f, farPlaneZ = %f; int width = %d, height = %d;\n", nearPlaneZ, farPlaneZ, framebufferWidth, framebufferHeight);

	const Matrix4x4& bodyToWorldMatrix =
		Matrix4x4::translate(bodyTranslation) *
		Matrix4x4::roll(bodyRotation.z) *
		Matrix4x4::yaw(bodyRotation.y) *
		Matrix4x4::pitch(bodyRotation.x);

	const Matrix4x4& headToWorldMatrix = bodyToWorldMatrix * headToBodyMatrix;
	temp = headToWorldMatrix;
	

	for (int eye = 0; eye < numEyes; ++eye) {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[eye]);
		glViewport(0, 0, framebufferWidth, framebufferHeight);

		glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		const Matrix4x4& objectToWorldMatrix       = Matrix4x4::translate(0.0f, 0.5f, 0.0f) * Matrix4x4::yaw(PI / 3.0f);
		const Matrix3x3& objectToWorldNormalMatrix = Matrix3x3(objectToWorldMatrix).transpose().inverse();
		const Matrix4x4& cameraToWorldMatrix       = headToWorldMatrix * eyeToHead[eye];

		const Vector3& light = Vector3(1.0f, 0.5f, 0.2f).normalize();


        // Draw the background
		//drawSky(framebufferWidth, framebufferHeight, nearPlaneZ, farPlaneZ, cameraToWorldMatrix.data, projectionMatrix[eye].inverse().data, &light.x);

        ////////////////////////////////////////////////////////////////////////
        // Draw a mesh
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);

		glUseProgram(shader);

        // in position
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(positionAttribute);

        // in normal
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normalAttribute);

        // in tangent
		if (tangentAttribute != -1) {
            // Only bind if used
			glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
			glVertexAttribPointer(tangentAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(tangentAttribute);
		}

            // in texCoord 
		glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
		glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(texCoordAttribute);

            // indexBuffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

            // uniform colorTexture (samplers cannot be placed in blocks)
		colorTextureUnit = 0;
		pTexture->bindTexture(GL_TEXTURE0);

            // Other uniforms in the interface block
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, uniformBindingPoint, uniformBlock);

			GLubyte* ptr = (GLubyte*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
                // mat3 is passed to openGL as if it was mat4 due to padding rules.
			for (int row = 0; row < 3; ++row) {
				memcpy(ptr + uniformOffset[0] + sizeof(float) * 4 * row, objectToWorldNormalMatrix.data + row * 3, sizeof(float) * 3);
			}

			memcpy(ptr + uniformOffset[1], objectToWorldMatrix.data, sizeof(objectToWorldMatrix));

			const Matrix4x4& modelViewProjectionMatrix = projectionMatrix[eye] * cameraToWorldMatrix.inverse() * objectToWorldMatrix;
			
			memcpy(ptr + uniformOffset[2], modelViewProjectionMatrix.data, sizeof(modelViewProjectionMatrix));
			memcpy(ptr + uniformOffset[3], &light.x, sizeof(light));
			const Vector4& cameraPosition = cameraToWorldMatrix.col(3);
			memcpy(ptr + uniformOffset[4], &cameraPosition.x, sizeof(Vector3));
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);


#   ifdef _VR
		{
			const vr::Texture_t tex = { reinterpret_cast<void*>(intptr_t(colorRenderTarget[eye])), vr::API_OpenGL, vr::ColorSpace_Gamma };
			vr::VRCompositor()->Submit(vr::EVREye(eye), &tex);
		}
#   endif
    } // for each eye

        ////////////////////////////////////////////////////////////////////////
#       ifdef _VR
            // Tell the compositor to begin work immediately instead of waiting for the next WaitGetPoses() call
    vr::VRCompositor()->PostPresentHandoff();
#       endif

     // Mirror to the window
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);
}

void		close()
{
	//Clean up
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
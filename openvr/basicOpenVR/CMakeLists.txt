# project basicOpenVR
project (basicOpenVR)

# Set the executable folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/OpenVR/basicOpenVR)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/OpenVR/basicOpenVR)


set (HEADERS 	glob.h
				initOpenGL.h
				matrix.h
				teapot.h
				minimalOpenVR.h)
set (BSOURCES	main.cpp)
set (VSHADERS	min.vert)
set (FSHADERS	min.frag)

if (WIN32)
	
	source_group ("Header Files" FILES ${HEADERS})
	source_group ("Source Files" FILES ${BSOURCES})
	source_group ("Shaders" 	 FILES ${VSHADERS} ${FSHADERS})
	
	# Create our executable named basicOpenVR
	add_executable (basicOpenVR		${HEADERS}
									${BSOURCES}
									${VSHADERS}
									${FSHADERS})
									
	# Link our executable with those dependent libraries
	target_link_libraries(basicOpenVR 		${OPENGL_LIBRARIES}
											${IMGUI}
											${SDL2}
											${SDL2MAIN}
											${SDL2MIXER}
											${GLEW32}
											${ASSIMP}
											${SOIL2DEBUG}
											glGA
											glGAMath
											${OPENVR})
											

	# Creates a visual studio user preferences file
	# Which changes 1 thing! 
	# 1. Changes working directory from $(ProjectDir) to ${OutDir}
	file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user" 
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>     \
    <Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
    <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">
        <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    </PropertyGroup>
    <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">
        <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
        <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
    </PropertyGroup>
    </Project>")
									
endif ()
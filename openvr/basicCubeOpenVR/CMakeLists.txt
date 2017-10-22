# project basicCubeOpenVR
project (basicCubeOpenVR)

# Set the executable folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/OpenVR/basicCubeOpenVR)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/OpenVR/basicCubeOpenVR)

# Set source files
set (CHEADERS	minimalOpenVR.h
				Cube.h)
set (CSOURCES	main.cpp)
set (VSHADERS	vshaderCube.vert)
set (FSHADERS	fshaderCube.frag)

if (WIN32)
	
	source_group ("Header Files" FILES ${CHEADERS})
	source_group ("Source Files" FILES ${CSOURCES})
	source_group ("Shaders" 	 FILES ${VSHADERS} ${FSHADERS})
	
	# Create our executable named basicCubeOpenVR
	add_executable (basicCubeOpenVR		${CHEADERS}
										${CSOURCES}
										${VSHADERS}
										${FSHADERS})
										
	# Link our executable with those dependent libraries
	target_link_libraries(basicCubeOpenVR 		${OPENGL_LIBRARIES}
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
												
	# Removes Debug directory created by visual studio										
	add_custom_command (
    TARGET basicCubeOpenVR
    POST_BUILD
    COMMAND cmake -E remove_directory ${CMAKE_SOURCE_DIR}/OpenVR/basicCubeOpenVR/Debug)
	
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
# Project provided by OpenVR
# Link : https://github.com/ValveSoftware/openvr
# Requires Steam VR and Ocullus Runtime to run
project (hellovr CXX)

# Set the executable folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/OpenVR/hellovr)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/OpenVR/hellovr)

set (HELLO_HEADERS	shared/lodepng.h
					shared/Matrices.h
					shared/pathtools.h
					shared/lodepng.cpp
					shared/Matrices.cpp
					shared/pathtools.cpp)

set (HELLO_SOURCES  hellovr_opengl_main.cpp)


if (WIN32)
	message (STATUS "Win32")
	
	source_group ("Header Files" FILES ${HELLO_HEADERS})
	source_group ("Source Files" FILES ${HELLO_SOURCES})
	
	
	# Create our executable hellovr
	add_executable(hellovr 				${HELLO_HEADERS}
										${HELLO_SOURCES})
	
	# Link our executable with those dependent libraries
	target_link_libraries(hellovr 		${OPENGL_LIBRARIES}
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
# basicCube
project (basicCube C CXX)


# Set the executable folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/examples/basicCube)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/examples/basicCube)

# Set Source files
set (BASIC_HEADERS	Extension.h)
set (BASIC_SOURCES	basicCube.cpp)
set (BASIC_VSHADERS	vshaderCube.vert)
set (BASIC_FSHADERS fshaderCube.frag)
set (BASIC_RESOURCES resources.txt)
					
# Window Specific Options
if (WIN32)
	message (STATUS "Win32")
	
	source_group ("Header Files" FILES ${BASIC_HEADERS})
	source_group ("Source Files" FILES ${BASIC_SOURCES})
	source_group ("Resources" 	 FILES ${BASIC_RESOURCES})
	source_group ("Shaders" 	 FILES ${BASIC_VSHADERS} ${BASIC_FSHADERS})
	
	
	# Create our executable basicCube
	add_executable(basicCube 				${BASIC_HEADERS}
											${BASIC_SOURCES}
											${BASIC_VSHADERS}
											${BASIC_FSHADERS}
											${BASIC_RESOURCES})
	
	# Link our executable with those dependent libraries
	target_link_libraries(basicCube 		${OPENGL_LIBRARIES}
											${IMGUI}
											${SDL2}
											${SDL2MAIN}
											${SDL2MIXER}
											${GLEW32}
											${ASSIMP}
											${SOIL2DEBUG}
											glGAMath
											glGA)
											
	# Removes Debug directory created by visual studio										
	add_custom_command (
    TARGET basicCube
    POST_BUILD
    COMMAND cmake -E remove_directory ${CMAKE_SOURCE_DIR}/examples/basicCube/Debug)
	
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

# OSX Specific Options
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message (STATUS "OSX")
	
	source_group ("Shaders" FILES ${BASIC_VSHADERS} ${BASIC_FSHADERS})	
	
	# Build Executable and link it with dependent libraries
	add_executable(basicCube				${BASIC_HEADERS}
											${BASIC_SOURCES}
											${BASIC_VSHADERS}
											${BASIC_FSHADERS})
	
	target_link_libraries(basicCube	 		${OPENGL_LIBRARIES}
											${IMGUI}
											${SDL2_LIBRARY}
											${SDLMIXER_LIBRARY}
											${OpenCV_LIBS}
											${Boost_LIBRARIES}
											${GLEW32}
											${ASSIMP}
											${SOIL2DEBUG}
											glGAMath
											glGA)
	


endif ()

# Linux Specific Options
if (UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message (STATUS "LINUX")
	

	# Build Executable and link it with dependent libraries
	add_executable(basicCube				${BASIC_HEADERS}
											${BASIC_SOURCES}
											${BASIC_VSHADERS}
											${BASIC_FSHADERS})
	
	target_link_libraries(basicCube		 	${OPENGL_LIBRARIES}
											${IMGUI}
											${SDL2_LIBRARIES}
											${SDL2_MIXER_LIBRARIES}
											${GLEW32}
											glGAMath
											glGA
											${SOIL2}
											${ASSIMP}
											${ZLIB_LIBRARIES})
endif()

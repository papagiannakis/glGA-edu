# Chapter-10
project (Chapter-10 C CXX)


# Set the executable folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/examples/Chapter-10)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/examples/Chapter-10)

# Set Source files
set (CHAPTER_HEADERS	fake.h)

set (CHAPTER_SOURCES	Chapter-10.cpp)

set (CHAPTER_VSHADERS	vshader3D.vert
						vshaderC1.vert)
					
set (CHAPTER_FSHADERS 	fshaderCube2.frag
						fshader3D.frag)
					
set (CHAPTER_RESOURCES 	resources.txt)
					
# Window Specific Options
if (WIN32)
	message (STATUS "Win32")
	
	source_group ("Header Files" FILES ${CHAPTER_HEADERS})
	source_group ("Source Files" FILES ${CHAPTER_SOURCES})
	source_group ("Resources" 	 FILES ${CHAPTER_RESOURCES})
	source_group ("Shaders" 	 FILES ${CHAPTER_VSHADERS} ${CHAPTER_FSHADERS})
	
	
	# Create our executable Chapter-10
	add_executable(Chapter-10 			${CHAPTER_HEADERS}
										${CHAPTER_SOURCES}
										${CHAPTER_VSHADERS}
										${CHAPTER_FSHADERS}
										${CHAPTER_RESOURCES})
	
	# Link our executable with those dependent libraries
	target_link_libraries(Chapter-10 	${OPENGL_LIBRARIES}
										${IMGUI}
										${SDL2}
										${SDL2MAIN}
										${SDL2MIXER}
										${GLEW32}
										${ASSIMP}
										${SOIL2DEBUG}
										glGAMath
										glGA)
											

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
	
	source_group ("Shaders" FILES ${CHAPTER_VSHADERS} ${CHAPTER_FSHADERS})
	
	# Build Executable and link it with dependent libraries
	add_executable(Chapter-10			${CHAPTER_HEADERS}
										${CHAPTER_SOURCES}
										${CHAPTER_VSHADERS}
										${CHAPTER_FSHADERS})
	
	target_link_libraries(Chapter-10	 	${OPENGL_LIBRARIES}
										${IMGUI}
										${SDL2_LIBRARY}
										${SDLMIXER_LIBRARY}
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
	add_executable(Chapter-10			${CHAPTER_HEADERS}
										${CHAPTER_SOURCES}
										${CHAPTER_VSHADERS}
										${CHAPTER_FSHADERS})
	
	target_link_libraries(Chapter-10		${OPENGL_LIBRARIES}
										${IMGUI}
										${SDL2_LIBRARIES}
										${SDL2_MIXER_LIBRARIES}
										${GLEW32}
										glGAMath
										glGA
										${SOIL2}
										${ASSIMP_LIBRARIES}
										${ZLIB_LIBRARIES})
endif()

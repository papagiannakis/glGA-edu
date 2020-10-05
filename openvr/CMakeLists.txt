# testing , project open vr
project (OpenVR CXX)

# include header files for OpenVr
include_directories (../_thirdPartyLibs/include/OpenVR)


# Works for all platforms
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Windows Specific Options
if (WIN32)
	# Define where CMake is going to look for _thirdPartyLibs
	set (CMAKE_LIBRARY_PATH 		${CMAKE_SOURCE_DIR}/_thirdPartyLibs/lib/Windows
									${CMAKE_SOURCE_DIR}/libraries/lib)
	
	# Third Party Libraries
	
	if (MSVC_VERSION GREATER_EQUAL 1910)
		find_library (IMGUI			ImGui_vs17.lib)
		find_library (ASSIMP 		assimp-vc141-mtd.lib)
		find_library (GLEW32 		glew32.lib)		
	elseif (MSVC_VERSION EQUAL 1900)
		find_library (IMGUI			ImGui_vs15.lib)
		find_library (ASSIMP 		assimp-vc141-mtd.lib)
		find_library (GLEW32 		glew32.lib)
	else ()
		find_library (IMGUI 		ImGui.lib)
		find_library (ASSIMP 		assimpd.lib)
		find_library (GLEW32		glew32d.lib)
	endif()
	
	find_library(SDL2 				SDL2.lib)
	find_library(SDL2MAIN 			SDL2main.lib)
	find_library(SDL2MIXER			SDL2_mixer.lib)
	find_library(SOIL2DEBUG 		soil2-debug.lib)
	
	# extra
	find_library(OPENVR				openvr_api.lib)
	
	# glGA Libraries
	find_library(GLGA 				glGA.lib)
	find_library(GLGAMATH 			glGAMath.lib)


endif ()

# OSX Specific Options
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message (STATUS "OSX")
	
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
	
	# Define where CMake is going to look for _thirdPartyLibs
	set (CMAKE_LIBRARY_PATH 		${CMAKE_SOURCE_DIR}/_thirdPartyLibs/lib/OSX
									${CMAKE_SOURCE_DIR}/libraries/lib)
									
	
	find_package (SDL2 REQUIRED)
	find_package (SDL2_mixer REQUIRED)
	find_package (OpenCV REQUIRED)
	find_package (Boost REQUIRED)
	
	include_directories (${SDL2_INCLUDE_DIR})
	include_directories (${SDLMIXER_INCLUDE_DIR})
    include_directories (${Boost_INCLUDE_DIRS})
	
	# Third Party Libraries
	find_library(IMGUI 				libImGui.a)
	find_library(GLEW32 			libGLEW.a)
	find_library(ASSIMP 			libassimpd.5.0.0.dylib)
	find_library(SOIL2DEBUG 		libsoil2-debug.a)
	
	# extra
	find_library(OPENVR				libopenvr_api.dylib)
	
	# glGA libraries
	find_library(GLGA 				libglGA.a)
	find_library(GLGAMATH			libglGAMath.a)

endif ()

# Linux Specific Options
if (UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message (STATUS "LINUX")
	INCLUDE(FindPkgConfig)

	PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
	PKG_SEARCH_MODULE(ZLIB REQUIRED zlib)
	PKG_SEARCH_MODULE(SDL2_MIXER REQUIRED SDL2_mixer)
	PKG_SEARCH_MODULE(ASSIMP REQUIRED assimp)
	INCLUDE_DIRECTORIES(${ASSIMP_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${SDL2MIXER_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
	
	# Define where CMake is going to look for _thirdPartyLibs
	set (CMAKE_LIBRARY_PATH 		${CMAKE_SOURCE_DIR}/_thirdPartyLibs/lib/Linux
									${CMAKE_SOURCE_DIR}/libraries/lib/Linux)
									
	
	# Third Party Libraries
	find_library(IMGUI 				libImGui.a)
	find_library(GLEW32 			libGLEW.a)
	find_library(SOIL2	 			libsoil2.a)
	
	find_library(OPENVR				libopenvr_api.so)
	
	# glGA libraries
	find_library(GLGA 				libglGA.a)
	find_library(GLGAMATH			libglGAMath.a)

endif ()


# add project folders
add_subdirectory(hellovr)
add_subdirectory(basicOpenVR)
add_subdirectory(basicCubeOpenVR)

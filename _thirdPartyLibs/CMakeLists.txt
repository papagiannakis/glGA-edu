# Version 3.5 is chosen instead of 3.6 because there is no Linux 3.6 yet. 
# Still you can use cmake 3.6 on mac or windows to build the project files.
cmake_minimum_required (VERSION 3.3)

# Maps to a solution file (Tutorial.sln). The solution will 
# have all targets (exe, lib, dll) as projects (.vcproj)
PROJECT (ImGUI C CXX)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories (include/ImGUI)
include_directories (include/SDL2)
include_directories (include/glew)


set (HEADERS	include/ImGUI/imconfig.h
				include/ImGUI/imgui.h
				include/ImGUI/imgui_impl_sdl.h
				include/ImGUI/imgui_impl_opengl3.h
				include/ImGUI/imgui_internal.h
				include/ImGUI/imstb_rectpack.h
				include/ImGUI/imstb_textedit.h
				include/ImGUI/imstb_truetype.h
	)
	
set (SOURCES	src/ImGUI/imgui.cpp
				src/ImGUI/imgui_demo.cpp
				src/ImGUI/imgui_draw.cpp
				src/ImGUI/imgui_impl_opengl3.cpp
				src/ImGUI/imgui_impl_sdl.cpp
				src/ImGUI/imgui_widgets.cpp
	)
	
# Define where the output library is going to be placed!
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib/Windows)
	
add_library (ImGUI	STATIC 	${HEADERS}
							${SOURCES}
			)
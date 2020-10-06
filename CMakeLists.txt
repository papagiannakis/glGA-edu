# Version 3.5 is chosen instead of 3.6 because there is no Linux 3.6 yet. 
# Still you can use cmake 3.6 on mac or windows to build the project files.
cmake_minimum_required (VERSION 3.3)

# Maps to a solution file (Tutorial.sln). The solution will 
# have all targets (exe, lib, dll) as projects (.vcproj)
PROJECT (glGAframework C CXX)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_STANDARD 11)


if (WIN32)
	# Set compiler flags and options. 
	# Here it is setting the Visual Studio warning level to 3
	 set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}	/W3")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# if OSX !	
	set (SOURCE_ROOT ${CMAKE_SOURCE_DIR})
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else ()
	# if Linux !
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()


# Command to output information to the console
# Useful for displaying errors, warnings, and debugging
message ("cxx Flags: " ${CMAKE_CXX_FLAGS})


include_directories (				libraries/include
						libraries/include/glGA
						libraries/include/glGAMath
						libraries/include/KDTree
						libraries/include/PRT
						_thirdPartyLibs/include
						_thirdPartyLibs/include/boost
						_thirdPartyLibs/include/Bullet
						_thirdPartyLibs/include/Assimp
						_thirdPartyLibs/include/glew
						_thirdPartyLibs/include/glm
						_thirdPartyLibs/include/ImGUI
						_thirdPartyLibs/include/SDL2
						_thirdPartyLibs/include/SOIL2
						)

# Sub-directories where more CMakeLists.txt exist
add_subdirectory(libraries)
add_subdirectory(examples)
add_subdirectory (openvr)

# UnComment to re-build ImGui
# Remember to change output lib name from ImGUI.lib to: 
# -ImGui_vs15 for Visual Studio 2014-
# -ImGui for Visual Studio 2012-

#add_subdirectory (_thirdPartyLibs)

glGA: graphics library for Geometric Applications 
-----------------------------------------

an open framework for modern, Shader-Based OpenGL, VR and Geometric Algebra computer graphics applications
@Copyright George Papagiannakis, 2012-2017, University of Crete & FORTH,
License is provided in the license.txt

glGA is described here:
1. Papagiannakis, G., Papanikolaou, P., Greassidou, E., and Trahanias, P., "glGA: an OpenGL Geometric Application framework for a modern, shader-based computer graphics curriculum", Eurographics 2014, Education Papers, 2014.
2. Kateros, S., Georgiou, S., Papaefthymiou, M., Papagiannakis, G., Tsioumas, M., “A comparison of gamified, immersive VR curation methods for enhanced presence and human-computer interaction in digital humanities”, International Journal of Heritage in the Digital Era, vol. 4, no 2, 2015, also presented in “The 1st International Workshop on ICT for the Preservation and Transmission of Intangible Cultural Heritage”, EUROMED2014
3. Papaefthymiou, M., Feng, A., Shapiro, A., Papagiannakis, G., “A fast and robust pipeline for populating mobile AR scenes with gamified virtual characters”. ACM SIGGRAPH-ASIA 2015, Symposium On Mobile Graphics and Interactive Applications, Kobe, ACM Press, November 2015

glGA page: http://george.papagiannakis.org/?page_id=513

Main glGA contributors
-----------------------------------------
- Papagiannakis   	George,
- Papanikolaou    	Petros,
- Greassidou      	Elisavet,
- Georgiou        	Stylianos,
- Kateros         	Stavros,
- Lydatakis       	Nikolaos,
- Zikas             Paul,
- Bachlitzanakis    Vasilis,
- Papaefthymiou   	Margarita,
- Kanakis           Marios,
- Geronikolakis		Stratos

Release notes:
-----------------------------------------
- Version 5.0

Novelties include:
- new Shader types: Compute, Geometry, Tesselation shaders
- OpenVR support, ImGUI, SDL2, dual quaternion, euclidean GA and Conformal GA animation blending and skinning

This is a keep-it-simple, but powerful & versatile C++11 openGL Geometric Applications framework for GPU shader-based computer graphics development for Windows and MacOSX-Linux & iOS mobile platforms utilizing modern GPUs.

This is the Education Edition, glGA5.0 version

Dependencies on all required third-party, open-source, freely available, multi-platform libraries are included inside the framework folders.

glGA libraries and provided examples
----------------------------------
1.  glGAMath - mathematics for graphics and geometric algebra (GA) algorithms
2.  glGA - main library
3.  basicWindow - simple empty window and OpenGL context
4.  basicCube - simple 3D cube with color per vertex
5.  basicCubeGUI - previous 3D cube with a GUI using ImGUI
6.  basicTriangle - simple triangle with color per vertex
7.  basicGeometry - Geometry shader example, create new primitives inside geometry shader
8.  basicCompute - Compute shader example with simple ray-tracing (Windows only)
9.  basicTesselation - Tesselation shader example, control inner and outer Tesselation levels
10. basicCubeOpenVR - simple 3D cube in VR with OpenVR api (Windows only)


CONTENT CREATION 
----------------------------------
Open format, COLLADA/FBX 3D standard based 3D asset creation and exchange:
Content Creation Tools: Autodesk Maya free for educational purposes (http://www.autodesk.com/education/home) or Google sketchup.
COLLADA format exporters for the above tools: Open-Collada (https://github.com/KhronosGroup/OpenCOLLADA) or the FBX exporter (http://www.autodesk.com/products/fbx/overview) for COLLADA.
We employ the ASSIMP third-party 3D asset loading library so almost all standard 3D file formats are also supported.


# Building glGAframework with the use of CMake or CMake GUI.

# General Information/Requirements.
----------------------------------
Hardware: 
  * OpenGL 3.2 or higher .
Software:
  * Tested with Windows 10 , Visual Studio Community 15
  * Tested with OSX: High Sierra, Sierra, El Capitan, XCode 8, 9
  * Tested with Ubuntu 16.04 LTS
	Note: Will work with older software but it's better with new!

* CMake Minimum version required 3.5
Download the latest one for your platform from : https://cmake.org/download/

# OSX.
----------------------------------
Tested with OSX High Sierra, Sierra, El Capitan, Yosemite and Xcode 9, 8, 7 and 6.

Step 1
----------------
install Xcode 8 command line tools from terminal : xcode-select --install

Step 2 (SDL2)
-----------------
A. Download from Development Libraries -> Mac OS X -> SDL2-2.0.x.dmg  
https://www.libsdl.org/download-2.0.php#source 

B. Open the .dmg file , press command+shift+g navigate to /Library/Frameworks and copy the SDL2.framework file . 

C. Open up a terminal and navigate to : /Library/Frameworks/SDL2.framework
then sign the framework using the command: "codesign -f -s - SDL2"

Step 3 (SDL2_mixer)
-----------------
A. Download from Development Libraries -> Mac OS X -> SDL2_mixer-2.0.x.dmg
https://www.libsdl.org/projects/SDL_mixer/

B. C. Same as above! 

TIP: Sign the SDL2_mixer framework with the command: "codesign -f -s - SDL2_mixer"

Step 3 (read below only if the provided, precompiled boost library do not work for you)
----------------
Install macports (download it from : https://www.macports.org/)

then open up a terminal and type: sudo port install boost

Step 4 (OpenCV and Boost Libraries)
----------------
use macports to install OpenCV and Boost libraries
- sudo port install opencv
- sudo port install boost

Step 5
----------------
You can build the project files with CMake or CMake GUI

Build project files with CMake. 
----------------
Open a terminal and navigate to glGA
then type:
mkdir build
cd build
cmake .. -G Xcode
CMake will generate in "build" directory a glGAframework.xcworkspace which is ready for use.

Build project files with CMake GUI
----------------
Tested with version 2.8.12.1 and 3.3.2.
Open CMake GUI
Add to "where is the source code:" the path to glGA
Add to "Where to build the binaries:" the path to build the glGA (glGA-5.0/build)
Configure (Specify the generator for this project:Xcode, Select "Use default native compilers" -> Done)
Generate

CMake GUI will generate in "build" directory a glGAframework.xcworkspace which is ready for use.

Step 6
----------------
A. Product-> Scheme-> e.g. Basic Cube and similarly for all schemes:

B. At the "Arguments" tab:

In Section "Environment Variables":

Name: DYLD_LIBRARY_PATH

Value: $(SOURCE_ROOT)/_thirdPartyLibs/lib/OSX

C.In Build settings of glGA:
C++ language dialect: C++11[-std=c++11]
C++ standard library: libc++ (LLVM C++ standard library with C++11 support)

Step 7
----------------
If any of your projects can't find the needed models, 'At the "Options" tab:

Enable the 'Working directory' and add in the field the location of your models. For example for basicCubeGUI add the following:

$(SOURCE_ROOT)/examples/basicCubeGUI

Step 8 (Animation Interpolation)
----------------
Select Animation Interpolation in glGAMath.h by setting the INTERPOLATION. You can select one of the following:

// #define QUAT 0 

// #define DQUAT 1

// #define GA_GAIGEN 2

// #define CGA_GAIGEN 3

// #define GA_VERSOR 4

// #define CGA_VERSOR 5

// #define GA_GAALOP 6

// #define CGA_GAALOP 7

For example for quaternions use the following:

// #define INTERPOLATION QUAT

If you are using GA_VERSOR or CGA_VERSOR:

**A. Change the build settings of glGAMath:**

C++ language dialect: C++11[-std=c++11]
C++ standard library: libc++ (LLVM C++ standard library with C++11 support)

**B. Add to the Header Search Paths in the build settings of glGAMath and glGA the following:**

$(SOURCE_ROOT)/_thirdPartyLibs/include/gfx
$(SOURCE_ROOT)/_thirdPartyLibs/include/vsr

**C.Change the build settings of the project that runs with GA_VERSOR or CGA_VERSOR support:**

1. C++ language dialect: C++11[-std=c++11]

2. C++ standard library: libc++ (LLVM C++ standard library with C++11 support)

3. Add the following to the OtherLinkerFlags:

$(SOURCE_ROOT)/_thirdPartyLibs/lib/OSX/libvsr.a
$(SOURCE_ROOT)/_thirdPartyLibs/lib/OSX/libgfx.a
$(SOURCE_ROOT)/_thirdPartyLibs/lib/OSX/libGLV.a

4. Add to the Header Search Paths:

$(SOURCE_ROOT)/_thirdPartyLibs/include/gfx
$(SOURCE_ROOT)/_thirdPartyLibs/include/vsr

When changing animation interpolation method first build glGAMath and then glGA library.

# Windows
============================================================
Running on Visual Studio 2015

Step 1
--------------
Create a new enviroment variable named GLGA and set it to the path where glGA is located.
then update the PATH enviroment variable with the path where glGA third party libraries are located.
example:
GLGA = C:\Users\MK\Desktop\glGA
PATH = %PATH%;%GLGA%\_thirdPartyLibs\lib\Windows;

Step 2
--------------
Build with CMake GUI
Tested with version 3.7.2.
Open CMake GUI
Add to "where is the source code:" the path to glGA
Add to "Where to build the binaries:" the path to build the glGA (glGA-5.0/build)
Configure (Specify the generator for this project:Visual Studio 14 2015, Select "Use default native compilers" -> Done)
Generate

Build with CMake
--------------
Open cmd and navigate to glGA-5.0
then type:
mkdir build
cd build
cmake ../ -G "Visual Studio 14 2015" 

Open the solution glGAframework.sln in directory glGA-5.0/build

Step3 (Build Assimp in Windows  (if you need it otherwise it is included in the framework))
----------------------------
Download and install Cmake (we are going to use cmake-gui): http://www.cmake.org/cmake/resources/software.html
Download and install DirectX sdk
(http://www.microsoft.com/en-us/download/confirmation.aspx?id=6812) .
Check the environment variable DXSDK_DIR for Direct SDK (assimp needs this in windows).

Download assimp-master:  https://github.com/assimp/assimp (uparhei
epilogi dexia stin othoni Download Zip)
Unzip the zip in any directory of your choise.
Lets say i have downloaded assimp in G:\Libraries\ directory and also
i have unzipped it there.
So there is the following folder: G:\Libraries\assimp-master
Run Cmake Gui
Set "Where is the source code" with G:/Libraries/assimp-master/
Set "Where to build the binaries" with
G:/Libraries/assimp-master/cmake_build -> This is the directory where
the project files will be created.
Click Configure in Cmake-Gui and choose Visual Studio 10
Set Cmake Install Prefix: G:/Libraries/assimp-master/cmake_build
Also enable ASSIMP_BUILD_STATIC_LIB
Click Generate and go to directory: G:/Libraries/assimp-master/cmake_build
Run Assimp.sln
You can see the Projects on the left.
Build ALL_BUILD project in Release Mode.
Build INSTALL project so the libs + headers will be copied to
installation directory.

You are going to need the following files and folders:
1) G:\Libraries\assimp-master\cmake_build\lib\assimp.lib
2) G:\Libraries\assimp-master\cmake_build\contrib\zlib\Release\zlibstatic.lib
3) G:\Libraries\assimp-master\cmake_build\include\

Step 4 (Animation Interpolation)
----------------
Select Animation Interpolation in glGAMath.h by setting the INTERPOLATION. You can select one of the following:

// #define QUAT 0 

// #define DQUAT 1

// #define GA_GAIGEN 2

// #define CGA_GAIGEN 3

// #define GA_GAALOP 6

// #define CGA_GAALOP 7

For example for quaternions use the following:

// #define INTERPOLATION QUAT

For Windows GA_VERSOR and CGA_VERSOR are currently not supported!

When changing animation interpolation method first build glGAMath and then glGA library.

# Linux
==============================================================
Step 1
--------------
Open up a terminal (ctrl + alt + t). 

sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install xorg-dev
sudo apt-get install libglu1-mesa-dev
sudo apt-get install ImageMagick
sudo apt-get install libmagick++-dev
sudo apt-get install boost
sudo apt-get install xutils-dev
sudo apt-get install mesa-utils
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-mixer-dev

Step 2
---------------
Create a new enviroment variable named GLGA and set it to the path where glGA is located.
then update the PATH enviroment variable with the path where glGA third party libraries are located.
example (terminal):
nano ~/.profile
export GLGA= yourPath/glGAframework
export PATH=$PATH:$GLGA/_thirdPartyLibs/lib/Linux/

Step 3
--------------
In Ubuntu you can also cmake with the following commands:
sudo apt-get update
sudo apt-get install cmake

Build Makefiles with CMake.
open up a terminal and navigate to glGA/_projects/Linux
then type:
cmake ../.. -G "Unix Makefiles"

CMake will generate Makefiles which are ready for use.
If you hit make under the Linux directory it will build all targets.

But if you want to build a specific target you can navigate for example
to assignments and hit make which will build all assignments from 1-8 
or navigate to Assignments1 folder and hit make which will build just Assignment1.

make clean removes all executables.

* Notes:
==============================
1. All executables are build into the folders where the source files are located.
e.g. basicCube.exe is inside glGA/examples/basicCube/
2. Depending on your GPU and how powerfull it is, you may need to change multisampling in 
order to function properly. In every "main".cpp file there is an initSDL() function. 
you may need to comment those 2 lines or change the value of MULTISAMPLESAMPLES to 8/4/2 etc:
SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
3. Above note has use to all SDL_GL_SetAttribute calls!!! Change them if something seems to not work properly. You can also comment them in order to get the default values. 

-Good luck!

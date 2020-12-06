# glGA Geometric Application framework for modern Shader-Based OpenGL #

## @Copyright George Papagiannakis, 2012-2020, University of Crete & FORTH ##


### Read carefully before proceeding
* Always work on the dev branch of the repo (on the Code tab, if the main branch is selected instead, click and choose dev).
* In case you find any errors you may open an Issue, in the Issues tab. Describe the problem and the required steps to replicate it. Again make sure you are working on the dev branch!

### License is provided in the license.txt ###

## glGA is described here: 

1. Papagiannakis, G., Papanikolaou, P., Greassidou, E., and Trahanias, P., "glGA: an OpenGL Geometric Application framework for a modern, shader-based computer graphics curriculum", Eurographics 2014, Education Papers, 2014.
2. Kateros, S., Georgiou, S., Papaefthymiou, M., Papagiannakis, G., Tsioumas, M., “A comparison of gamified, immersive VR curation methods for enhanced presence and human-computer interaction in digital humanities”, International Journal of Heritage in the Digital Era, vol. 4, no 2, 2015, also presented in “The 1st International Workshop on ICT for the Preservation and Transmission of Intangible Cultural Heritage”, EUROMED2014
3. Papaefthymiou, M., Feng, A., Shapiro, A., Papagiannakis, G., “A fast and robust pipeline for populating mobile AR scenes with gamified virtual characters”. ACM SIGGRAPH-ASIA 2015, Symposium On Mobile Graphics and Interactive Applications, Kobe, ACM Press, November 2015

glGA page: (http://george.papagiannakis.org/?page_id=513)

### Main glGA contributors
-----------------------------------------
* Papagiannakis   	George
* Papanikolaou    	Petros
* Greassidou      	Elisavet
* Georgiou        	Stylianos
* Kateros         	Stavros
* Lydatakis       	Nikolaos
* Papaefthymiou   	Margarita
* Kanakis           Marios
* Geronikolakis		Stratos
* Bachlitzanakis    Vasileios
* Evangellou        Yannis
* Kartsonaki        Ioanna
* Kamarianakis      Manos
* Stefanidi         Zina
* Stefanidi         Eleni

### Release notes:
-----------------------------------------
#### Version glGA5.0.1

Novelties include:
* PRT, KDTree libraries for PRT rendering, KDTree visualization
* dual quaternion, euclidean GA and Conformal GA animation blending

This is a keep-it-simple, but powerful&versatile C++11 openGL Geometric Applications framework for GPU shader-based computer graphics development for Windows and MacOSX-Linux & iOS mobile platforms utilizing modern GPUs.

This is the Developer's Edition, glGA5.0 version

Dependencies on all required third-party, open-source, freely available, multi-platform libraries are included inside the framework folders.

glGA libraries and provided examples
----------------------------------
1. glGAMath - mathematics for graphics and geometric algebra algorithms
2. KDTree - acceleration structure for CG, used in PRT rendering
3. PRT - main library that implements the PRT (precomputed radiance transfer) algorithm
4. glGA - main library
5. basicWindow - simple empty window and OpenGL context
6. basicCube - simple 3D cube with color per vertex
7. basicCubeGUI - previous 3D cube with an AntTweakBar GUI

#### Known Issues:

1. During the first Visual Studio Code build, assimp may not be detected. In this case, simply try building the project once again.
2. There have been reports that the command "codesign -f -s - SDL2_mixer", may fail if you try to use it with the latest version of SDL2 mixer. In case that happens, please download and install an older version of SDL2 mixer (for instance: https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.0.dmg) 

#### OpenGL Shading Language, Third Edition: https://wiki.labomedia.org/images/1/10/Orange_Book_-_OpenGL_Shading_Language_2nd_Edition.pdf 
* Chapter9:  Fog simulation
* Chapter10: Reflection from a shiny surface
* Chapter11: Bump mapping
* Chapter12: Precomputed Radiance Transfer
* Chapter13: Shadow map
* Chapter14: Refraction, Diffraction example
* Chapter15: Perlin noise
* Chapter16: Particles rendering (fireworks)
* Chapter17: Antialiased Checkerboard Fragment Shader
* Chapter18: Hatching example
* Chapter19: Image processing examples


EXTRA THIRD PARTY LIBRARIES AND 3D CONTENT TO TEST WITH
--------------------------------------------------------------------------------

Inside your locally cloned "glGAFramework" folder copy the following directories (for Developers only)
- _glGA-data:			https://dl.dropboxusercontent.com/u/43768664/glGA4.0/_glGA-data.zip

CONTENT CREATION 
---------------------------------------------------------------------------
Open format, COLLADA 3D standard based 3D asset creation and exchange:
Content Creation Tools: Autodesk Maya free for educational purposes (http://www.autodesk.com/education/home) or Google sketchup
COLLADA format exporters for the above tools: Open-Collada (https://github.com/KhronosGroup/OpenCOLLADA) or the FBX exporter (http://www.autodesk.com/products/fbx/overview) for COLLADA

HOW TO EXTEND GLGA
---------------------------------------------------------------------------
Any new functionality you bring to glGA, has to be in the form of a static library that all other users can use and a simple, thin application layer that illustrates its usage. e.g.:
- glGA lib - basicWindow app 
Any third party libraries you are using place them under the _thirdPartyLibs(binaries) and _thirdPartyLibsSrc(src code).


# General Information/Requirements.

Hardware: 

* OpenGL 3.2 and up

Software:

* CMake Minimum version required 3.5
Download the latest one for your platform from : https://cmake.org/download/

* Visual Studio Code
Download Visual Studio (VS) Code from : https://code.visualstudio.com

Before compiling your examples you need to decide the OpenGL version
-Version of OpenGL (2.1 or 3.2 and later) as defined in each example:
    - uncomment the USE_OPENGL21 or USE_OPENGL32 respectively (only one of them should be active)


# How to get glGA-edu through GitHub:
You can get glGA-edu through GitHub by using Sourcetree (https://www.sourcetreeapp.com).
Simply follow the instructions shown in this video: https://elearn.uoc.gr/pluginfile.php/224876/mod_resource/content/1/HY358_glGA_installation_tutorial.mp4

Finally, after cloning the repository successfully, make sure that your are on the dev branch (as shown in the video) and then:
    1. Navigate to the Repository menu and choose Git LFS --> Fetch LFS Content.
    2. Navigate to the Repository menu and choose Git LFS --> Pull LFS Content.
    
# Installation instructions:

**Important** Follow the platform-specific step depending to your OS and then install and setup Visual Studio Code as shown in the end of this tutorial. 

**The usage of Visual Studio Code is strongly encouraged against any other method.**

## Visual Studio Code (All Platforms).

Follow the steps below, after you complete the necessary steps 
depending on your OS.

1. Run Visual Studio Code.
2. Navigate to "View" menu and then choose "Extensions".
3. Find and install the following two extensions:
    1. C++ extension for VS Code. Install the C/C++ extension by searching for ‘c++’ in the Extensions view.
    2. CMake Tools extension for VS Code. Install this extension by searching for ‘CMake tools’ in the Extensions view.
4. In VS Code, click on the Explorer button (first button on the left column) and then click “Open Folder” and navigate to the glGA folder.
5. Navigate to "View" menu and choose "Command Palette" and run “CMake: Select a Kit”. The extension will automatically scan for kits on your computer and create a list of compilers found on your system.
6. Select the compiler of your choice.
7. In the bottom bar of VS code (blue one), make sure that the project is in Debug mode (something like: Cmake:[Debug]:Ready).
8. Click the build button in the same line.
9. In the same bottom line, the build target must be [all] and the run target must be the project you wish to run.
10. Click the “Debug” button (the one with the bug).


## NECESSARY STEPS FOR OSX

Step 1
----------------
install Xcode command line tools from terminal : xcode-select --install

Step 2
----------------
Make sure CMake is installed as mentioned in "General Information/Requirements" section above.

Step 3 (SDL2)
-----------------
A. Download from Development Libraries -> Mac OS X -> SDL2-2.0.x.dmg  
https://www.libsdl.org/download-2.0.php#source 

B. Open the .dmg file , press command+shift+g navigate to /Library/Frameworks and copy the SDL2.framework file . 

C. Open up a terminal and navigate to : /Library/Frameworks/SDL2.framework
then sign the framework using the command: "codesign -f -s - SDL2"

Step 4 (SDL2_mixer)
-----------------
A. Download from Development Libraries -> Mac OS X -> SDL2_mixer-2.0.x.dmg
https://www.libsdl.org/projects/SDL_mixer/

B. C. Same as above! 

TIP: Sign the SDL2_mixer framework with the command: "codesign -f -s - SDL2_mixer"

Step 5 (read below only if the provided, precompiled boost library do not work for you)
----------------
Install macports (download it from : https://www.macports.org/)

then open up a terminal and type: sudo port install boost


## NECESSARY STEPS FOR WINDOWS

Tested with Visual Studio Code 1.49.2

Step 1
--------------
Create a new enviroment variable named GLGA and set it to the path where glGA is located.
then update the PATH enviroment variable with the path where glGA third party libraries are located.
example:
GLGA = C:\Users\MK\Desktop\glGA-5.0
PATH = %PATH%;%GLGA%\_thirdPartyLibs\lib\Windows;

*Compiler needs to be Visual Studio 2019 Community Realease - x86.*

Step 2
--------------
Make sure CMake is installed as mentioned in "General Information/Requirements" section above.


Step 3 [Optional] (Build Assimp in Windows  (if you need it otherwise it is included in the framework))
----------------------------
Download and install Cmake (we are going to use cmake-gui): http://www.cmake.org/cmake/resources/software.html
Download and install DirectX sdk
(http://www.microsoft.com/en-us/download/confirmation.aspx?id=6812) .
Check the environment variable DXSDK_DIR for Direct SDK (assimp needs this in windows).

Download assimp-master:  https://github.com/assimp/assimp 
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

Step 4 [Optional] (Precomputed Radiance Transfer)
--------------

For building glGA with PRT support:
In Solution exproler go to glGA properties then Configuration Properties->C/C++->Preprocessor. In Preprocessor Definitions at ;USE_PRT=1

Step 5 [Optional] (Animation Interpolation)
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

## NECESSARY STEPS FOR LINUX

Step 1
--------------
Open up a terminal (ctrl + alt + t). 

sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install xorg-dev
sudo apt-get install libglu1-mesa-dev
sudo apt-get install ImageMagick
sudo apt-get install libmagick++-dev
sudo apt-get install libboost-all-dev
sudo apt-get install xutils-dev
sudo apt-get install mesa-utils
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-mixer-dev
sudo apt-get install libassimp-dev
sudo apt-get install assimp-utils

Step 2
---------------
Create a new enviroment variable named GLGA and set it to the path where glGA is located.
then update the PATH enviroment variable with the path where glGA third party libraries are located.
example (terminal):
nano ~/.profile
export GLGA= yourPath/glGA-5.0
export PATH=$PATH:$GLGA/_thirdPartyLibs/lib/Linux/

Step 3 [Optional] (Without VS Code)
--------------
In Ubuntu you can also cmake with the following commands:
sudo apt-get update
sudo apt-get install cmake

Build Makefiles with CMake.
open up a terminal and navigate to glGA-5.0
then type:
mkdir build
cd build
cmake .. -G "Unix Makefiles"

CMake will generate Makefiles which are ready for use.
If you hit make under the Linux directory it will build all targets.

But if you want to build a specific target you can navigate for example
to assignments and hit make which will build all assignments from 1-8 
or navigate to Assignments1 folder and hit make which will build just Assignment1.

make clean removes all executables.

* Notes:

1. All executables are build into the folders where the source files are located.
e.g. Assignment1.exe is inside glGA/assignments/Assignment1/
2. Depending on your GPU and how powerfull it is, you may need to change multisampling in 
order to function properly. In every "main".cpp file there is an initSDL() function. 
you may need to comment those 2 lines or change the value of MULTISAMPLESAMPLES to 8/4/2 etc:
SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
3. Above note has use to all SDL_GL_SetAttribute calls!!! Change them if something seems to not work properly. You can also comment them in order to get the default values. 

Step 4 [Optional]
--------------

Installation of Image Magick (if you need it otherwise it is included in the framework):

Download from Ubuntu Software Center and use -lMagick++ in Makefiles. It is installed in usr/lib and usr/include/ImageMagick.





# Appendix

## Running glGA via Xcode 

In case you would like to run glGA through Xcode on OSX (Xcode + CMake needed). 
----------------
Build project files with CMake through a terminal
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


Inside Xcode
----------------
A. Product-> Scheme-> e.g. Basic Cube and similarly for all schemes:

B. At the "Arguments" tab:

In Section "Environment Variables":

Name: DYLD_LIBRARY_PATH

Value: $(SOURCE_ROOT)/_thirdPartyLibs/lib/OSX

C.In Build settings of glGA:
C++ language dialect: C++11[-std=c++11]
C++ standard library: libc++ (LLVM C++ standard library with C++11 support)

If any of your projects can't find the needed models, 'At the "Options" tab:

Enable the 'Working directory' and add in the field the location of your models. For example for basicCubeGUI add the following:

$(SOURCE_ROOT)/examples/basicCubeGUI

[Optional] (Precomputed Radiance Transfer Support)
----------------
For building glGA with PRT support: Build Setings->Preprocessor Macros add USE_PRT=1

[Optional] (Animation Interpolation)
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

-Good luck!

# glGA Geometric Application framework for modern Shader-Based OpenGL #

## @Copyright George Papagiannakis, 2012-2020, University of Crete & FORTH ##

### License is provided in the license.txt ###

-----------------------------------------

## What is glGA?

glGA full description can be found in [glga homepage](http://george.papagiannakis.org/?page_id=513). Some relevant papers are the following.

1. Papagiannakis, G., Papanikolaou, P., Greassidou, E., and Trahanias, P., "glGA: an OpenGL Geometric Application framework for a modern, shader-based computer graphics curriculum", Eurographics 2014, Education Papers, 2014.
2. Kateros, S., Georgiou, S., Papaefthymiou, M., Papagiannakis, G., Tsioumas, M., “A comparison of gamified, immersive VR curation methods for enhanced presence and human-computer interaction in digital humanities”, International Journal of Heritage in the Digital Era, vol. 4, no 2, 2015, also presented in “The 1st International Workshop on ICT for the Preservation and Transmission of Intangible Cultural Heritage”, EUROMED2014
3. Papaefthymiou, M., Feng, A., Shapiro, A., Papagiannakis, G., “A fast and robust pipeline for populating mobile AR scenes with gamified virtual characters”. ACM SIGGRAPH-ASIA 2015, Symposium On Mobile Graphics and Interactive Applications, Kobe, ACM Press, November 2015
 

-----------------------------------------

# Read carefully before proceeding

* Always work on the dev branch of the repo (on the Code tab, if the main or another branch is selected instead, click and choose dev).
* In case you find any errors you should open an Issue, in the Issues tab. Describe the problem and the required steps to replicate it. Again make sure you are working on the dev branch!

-----------------------------------------


## Installation instructions and hardware requirements

*Hardware Requirements*: OpenGL 3.2 and up

1. Clone this repo locally and pull LFS content. You can follow [these instructions](https://github.com/papagiannakis/glGA-edu/wiki/How-to-get-glGA5.0-through-GitHub) if you are not sure how to do it. Make sure you are on the **dev** branch. 
1. Follow the basic installation instructions in the wiki, depending on your OS: [Windows](https://github.com/papagiannakis/glGA-edu/wiki/Basic-Windows-Installation-Instructions), [Mac](https://github.com/papagiannakis/glGA-edu/wiki/Basic-Mac-Installation-Instruction) or [Linux](https://github.com/papagiannakis/glGA-edu/wiki/Basic-Linux-Installation-Instructions). 
2. Download and setup Visual Studio to build and run each project. Instructions to do so are found [here](https://github.com/papagiannakis/glGA-edu/wiki/Basic-Visual-Studio-Code-usage-for-all-OS).


*The usage of Visual Studio Code is strongly encouraged against any other method.* For other build and run methods, you may find information [here](https://github.com/papagiannakis/glGA-edu/wiki/Older-Installation-Instructions-for-all-OS).





-----------------------------------------

## Main glGA contributors
* Papagiannakis     George
* Papanikolaou      Petros
* Greassidou        Elisavet
* Georgiou          Stylianos
* Kateros           Stavros
* Lydatakis         Nikolaos
* Papaefthymiou     Margarita
* Kanakis           Marios
* Geronikolakis       Stratos
* Bachlitzanakis    Vasileios
* Evangellou        Yannis
* Kartsonaki        Ioanna
* Kamarianakis      Manos
* Stefanidi         Zina
* Stefanidi         Eleni




# Release notes:

### Version glGA5.0.1

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


#### OpenGL Shading Language, Third Edition 
[Download the book here](https://wiki.labomedia.org/images/1/10/Orange_Book_-_OpenGL_Shading_Language_2nd_Edition.pdf) 
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

------------------------------------------------------------------------

#### Known Issues:

1. During the first Visual Studio Code build, assimp may not be detected. In this case, simply try building the project once again.


-----------------------------------------------------------------------------

## How to extend glga

Any new functionality you bring to glGA has to be in the form of a static library that all other users can use and a simple, thin application layer that illustrates its usage. e.g.:

- glGA lib - basicWindow app 

Any third party libraries you are using place them under the _thirdPartyLibs(binaries) and _thirdPartyLibsSrc(src code).

You may check some extra libraries and content in [this wiki page](https://github.com/papagiannakis/glGA-edu/wiki/Extra-3rd-party-libraries-and-content,-3D-content-creation).





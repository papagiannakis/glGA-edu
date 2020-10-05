# CMakeLists.txt for -> Libraries  
# Including : glGAMath, glGA, PRT, KDTree

# Define glGAMath source files
set (GLGAMATH_HEADERS 
	include/glGAMath/c3ga.h
	include/glGAMath/c3ga_util.h
	include/glGAMath/common.h
	include/glGAMath/e3ga.h
	include/glGAMath/e3ga_util.h
	include/glGAMath/gabits.h
	include/glGAMath/glGAMath.h
	include/glGAMath/mvtypebase.h)

set (GLGAMATH_SOURCES
	src/glGAMath/c3ga.cpp
	src/glGAMath/c3ga_util.cpp
	src/glGAMath/e3ga.cpp
	src/glGAMath/e3ga_util.cpp
	src/glGAMath/glGAMath.cpp)


# Define glGA source files
set (GLGA_HEADERS
	include/glGA/glGAHelper.h
	include/glGA/glGAMesh.h
	include/glGA/glGAMeshEntry.h
	include/glGA/glGAMeshPRT.h
	include/glGA/glGARigMesh.h
	include/glGA/glGARigMeshPRT.h
	include/glGA/PlatformWrapper.h)
	
set (GLGA_SOURCES
	src/glGA/glGAHelper.cpp
	src/glGA/glGAMesh.cpp
	src/glGA/glGAMeshEntry.cpp
	src/glGA/glGAMeshPRT.cpp
	src/glGA/glGARigMesh.cpp
	src/glGA/glGARigMeshPRT.cpp
	src/glGA/PlatformWrapper.cpp
	src/glGA/glGARigMeshGA.cpp)
	

# Define PRT source files
set (PRT_HEADERS
	include/PRT/CGA_SH.hpp
	include/PRT/CGA_SH_Rotations.h
	include/PRT/Diffuse.h
	include/PRT/FunctionLight.h
	include/PRT/HDRI_frame.h
	include/PRT/HDRLight.h
	include/PRT/HDRloader.h
	include/PRT/Illumination.h
	include/PRT/Light.h
	include/PRT/MonteCarlo.h
	include/PRT/Sample.h
	include/PRT/Shadow.h
	include/PRT/Shadowed.h
	include/PRT/ShadowedDiffuse.h
	include/PRT/ShadowedMonteCarlo.h
	include/PRT/SHRotation.h
	include/PRT/SphericalHarmonics.h
	include/PRT/Split.h
	include/PRT/Structs.h
	include/PRT/Unshadowed.h
	include/PRT/UnshadowedDiffuse.h
	include/PRT/UnshadowedMonteCarlo.h
	include/PRT/Utils.h
	
	)

set (PRT_SOURCES
	src/PRT/CGA_SH.cpp
	src/PRT/CGA_SH_Rotations.cpp
	src/PRT/Diffuse.cpp
	src/PRT/FunctionLight.cpp
	src/PRT/HDRI_frame.cpp
	src/PRT/HDRLight.cpp
	src/PRT/HDRloader.cpp
	src/PRT/Light.cpp
	src/PRT/MonteCarlo.cpp
	src/PRT/Sample.cpp
	src/PRT/Shadow.cpp
	src/PRT/Shadowed.cpp
	src/PRT/ShadowedDiffuse.cpp
	src/PRT/ShadowedMonteCarlo.cpp
	src/PRT/SHRotation.cpp
	src/PRT/SphericalHarmonics.cpp
	src/PRT/Split.cpp
	src/PRT/Structs.cpp
	src/PRT/Unshadowed.cpp
	src/PRT/UnshadowedDiffuse.cpp
	src/PRT/UnshadowedMonteCarlo.cpp
	src/PRT/Utils.cpp
	)
	

# Define KDTree source files
set (KDT_HEADERS
	include/KDTree/error.h
	include/KDTree/geometry.h
    include/KDTree/KDTreeVisulization.hpp
	include/KDTree/kdtree.h
	include/KDTree/kdtreeaccel.h
	include/KDTree/memory.h
	include/KDTree/parallel.h
	include/KDTree/pbrt.h
	include/KDTree/PrimitiveTriangle.h
	include/KDTree/probes.h
	include/KDTree/stdafx.h)
	
set (KDT_SOURCES
	src/KDTree/geometry.cpp
	src/KDTree/kdtree.cpp
    src/KDTree/KDTreeVisulization.cpp
	src/KDTree/memory.cpp
	src/KDTree/PrimitiveTriangle.cpp
	src/KDTree/stdafx.cpp)


# Define where the output library is going to be placed!
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/libraries/lib/)

if (WIN32)
	link_directories ( ${CMAKE_SOURCE_DIR}/_thirdPartyLibs/lib/Windows/)
	message (STATUS "WIN32")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	link_directories ( ${CMAKE_SOURCE_DIR}/_thirdPartyLibs/lib/OSX/)
	message (STATUS "OSX")
endif ()

if (UNIX AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY 	${CMAKE_SOURCE_DIR}/libraries/lib/Linux/)
	link_directories ( ${CMAKE_SOURCE_DIR}/_thirdPartyLibs/lib/Linux/)
	message (STATUS "LINUX")
endif ()

# Build Libraries
add_library(glGAMath 	STATIC		${GLGAMATH_HEADERS}
									${GLGAMATH_SOURCES})
add_library(PRT			STATIC		${PRT_HEADERS}
									${PRT_SOURCES})
add_library(KDTree 		STATIC		${KDT_HEADERS}
									${KDT_SOURCES})
add_library(glGA 		STATIC		${GLGA_HEADERS}
									${GLGA_SOURCES})

# Link glGA with glGAMath and PRT
TARGET_LINK_LIBRARIES(glGA	glGAMath PRT)

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

add_library(glGA 		STATIC		${GLGA_HEADERS}
									${GLGA_SOURCES})

# Link glGA with glGAMath
TARGET_LINK_LIBRARIES(glGA	glGAMath)

# source files.
SRC =  	glGAHelper.cpp glGAMesh.cpp glGARigMesh.cpp PlatformWrapper.cpp glGAMeshEntry.o
 
OBJ = $(SRC:.cpp=.o)
 
OUT = ../../lib/Linux/libglGA.a
 
# include directories
INCLUDES =  -I../../include -I../../../_thirdPartyLibs/include -I../../../_thirdPartyLibs/include/glm -I../../../_thirdPartyLibs/include/glew -I../../../_thirdPartyLibs/include/glm -I/usr/include/assimp -I../../../_thirdPartyLibs/include/SOIL2/
 
# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -g 
 
# compiler
CCC = g++
 
# library paths
LIBS = -l../../_thirdPartyLibs/lib/Linux/libGLEW.a -l../../_thirdPartyLibs/lib/Linux/libsoil2.a -lsoil2 -lstdc++ -lGL -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lm -lrt -lXcursor -lassimp
 
# compile flags
LDFLAGS = -g -std=c++11
 
.SUFFIXES: .cpp
 
#default: dep $(OUT)
 
.cpp.o:
	$(CCC) $(LDFLAGS) $(INCLUDES) $(CCFLAGS) -c $< -o $@
 
$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)
 
depend: dep
 
dep:
	makedepend -f Makefile.linux64 -- $(CFLAGS) -- $(INCLUDES) $(SRC)
 
clean:
	rm -f $(OBJ) $(OUT) Makefile.linux64.bak



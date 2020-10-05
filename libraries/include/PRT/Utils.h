#ifndef UTILS_H

#include "glGA/PlatformWrapper.h"
#include <string>
#include <glm/glm.hpp>
#include <assimp/types.h>
//#include <time.h>


#ifndef __APPLE__
#include <glGA/glGAHelper.h>
#else
#include <sys/time.h>
#endif  // __APPLE__

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

#define BUFFER_OFFSET( offset ) ((GLvoid*) (offset))
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define INVALID_MATERIAL 0xFFFFFFFF

#define GLExitIfError()                                                         \
{																				\
    GLenum Error = glGetError();                                                \
    if (Error != GL_NO_ERROR) {                                                 \
        printf("OpenGL error in %s:%d: 0x%x\n", __FILE__, __LINE__, Error);			\
        exit(0);																	\
    }                                                                           \
}

#define GLCheckError() (glGetError() == GL_NO_ERROR)

glm::vec3 fromSphericalToCartesian(float phi, float theta, float r);
unsigned int getNextPowerOf2(unsigned int n);
float calculateRunningTime(const timeval& startTime);//clock_t& startTime);
bool readFile(const char* filename, std::string& shader);

#endif

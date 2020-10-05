#include "PRT/Utils.h"
#include "glGA/PlatformWrapper.h"

#include <iostream>
#include <iostream>
#include <fstream>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 fromSphericalToCartesian(float phi, float theta, float r)
{
    float cartesian_z = cos(phi) * sin(theta) * r;	//spherical x
    float cartesian_x = sin(phi) * sin(theta) * r;	//spherical y
    float cartesian_y = cos(theta) * r;				//spherical z
    
    return glm::vec3(cartesian_x,cartesian_y,cartesian_z);
}

unsigned int getNextPowerOf2(unsigned int n)
{
    for (int i = 0; i < sizeof(n) * 8; ++i)
    {
        if ((1U << i) >= n) {
            return 1U << i;
        }
    }
    return 0;
}

float calculateRunningTime(const timeval& startTime)
{
    timeval currentTime;
    gettimeofday(&currentTime, NULL);
    float runningTime = (float)(currentTime.tv_sec - startTime.tv_sec);
    runningTime += ((float)(currentTime.tv_usec - startTime.tv_usec)) / 1000000.0f;
    return runningTime;
}

bool readFile(const char* filename, std::string& shader) {
    std::ifstream fileStream(filename, std::ios::in);
    if(fileStream.is_open()) {
        std::string line;
        while(getline(fileStream, line)) {
            shader += "\n" + line;
        }
        fileStream.close();
    } else {
        std::cout << "Unable to open file" << filename << std::endl;
        return false;
    }
    return true;
}

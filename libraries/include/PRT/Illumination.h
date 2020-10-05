//
//  Illumination.hpp
//  PRT
//
//  Created by Margrarita Papaefthimiou on 05/07/2017.
//
//

#ifndef Illumination_hpp
#define Illumination_hpp

#include "stdio.h"
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "KDTree/kdtreeaccel.h"
#include "PRT/Structs.h"

using namespace std;
using namespace StructDef;

class Illumination
{
    public:
        virtual void coefficients(glm::mat4& coefficients, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, std::string name, bool shadow, KdTreeAccel *kdTree) = 0;

        virtual void writeToFile(const char *, int coeffSize) = 0;

        virtual void readFile(const char *c, int coeffSize) = 0;

        virtual double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient, KdTreeAccel *kdTree) = 0;
    
        virtual std::vector<glm::vec3> computeColors(int numVertices) = 0;
    
        virtual void setRotation(glm::mat4 rotation) = 0;
};

#endif /* Illumination_h */

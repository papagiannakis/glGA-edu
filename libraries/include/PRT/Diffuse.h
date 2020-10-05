//
//  Diffuse.h
//  PRT
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef __MetaioOpenGLES__Diffuse__
#define __MetaioOpenGLES__Diffuse__

#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include "Structs.h"
#include "KDTree/kdtreeaccel.h"
#include "PRT/Illumination.h"

using namespace StructDef;

class Diffuse: public Illumination
{
private:
    int numCoeff;
    double* vertCoeff;
    glm::mat4 rotation;
    
public:
    glm::vec3* lightCoeffs;
    Diffuse(int numCoeff);
    
    void coefficients(glm::mat4& coefficients, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, std::string name, bool shadow, KdTreeAccel *kdTree);
    
    void writeToFile(const char *, int coeffSize);
    
    void readFile(const char *c, int coeffSize);
    
    virtual double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient, KdTreeAccel *kdTree) = 0;
    
    std::vector<glm::vec3> computeColors(int numVertices);
    
    void setLightCoeffs(glm::vec3* lightCoeffs);
    
    void setRotation(glm::mat4 rotation);
};

#endif /* Diffuse.h */

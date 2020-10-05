//
//  Unshadowed.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/Unshadowed.h"

double Unshadowed::computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient,  KdTreeAccel *kdTree)
{
    vec3 dir(samplesCoefficient.vectorCoord.x, samplesCoefficient.vectorCoord.y, samplesCoefficient.vectorCoord.z);
    
    double H = glm::dot(dir, normal);
    
    return H;
};
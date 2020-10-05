//
//  HDRLight.h
//  PRT
//
//  Created by margarita on 3/10/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef __MetaioOpenGLES__HDRLight__
#define __MetaioOpenGLES__HDRLight__

#include "Light.h"
#include "Structs.h"

using namespace StructDef;
using namespace glm;
using namespace std;

class HDRLight : public Light
{
    
public:
    int imgSz;
    StructDef::rgb *pixels;
    HDRLight(std::string img);
    
    // get light on angle theta,phi
    vec3 getLight(SHSample samplesCoefficients);
    int diretionToImageSpace(SHSample samplesCoefficients);
    void computeSHImage(glm::vec3* lightCoeffs, string name);
};

#endif

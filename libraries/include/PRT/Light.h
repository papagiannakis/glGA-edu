//
//  Light.h
//  PRT
//
//  Created by margarita on 3/3/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef __MetaioOpenGLES__Light__
#define __MetaioOpenGLES__Light__

#include <stdio.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Structs.h"
#include <vector>

using namespace glm;
using namespace std;
using namespace StructDef;

class Light
{
private:
    vec3* lightCoeffs;
public:
    // set light coeffs
    vec3* setLightCoeffs(vector<SHSample> samplesCoefficients, int samples, int numCoeff);
    
    // get light on angle theta,phi
    virtual vec3 getLight(SHSample samplesCoefficients) = 0;
};
#endif /* defined(__MetaioOpenGLES__Light__) */

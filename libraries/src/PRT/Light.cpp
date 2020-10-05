//
//  Light.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/3/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/Light.h"
#ifndef __APPLE__
#define _USE_MATH_DEFINES 
#include <math.h>
#endif  // __APPLE__

vec3* Light::setLightCoeffs(vector<SHSample> samplesCoefficients,int numSamples, int numCoeff)
{
    // compute light coefficients
    lightCoeffs = new vec3[numCoeff];
    
    for(int i = 0; i < numCoeff; i++)
    {
        lightCoeffs[i] = vec3(0.0,0.0,0.0);
        
        for(int j = 0; j < numSamples; j++)
        {
            vec3 light = this->getLight(samplesCoefficients[j]);
            
            double curC = samplesCoefficients.at(j).coeff[i];
            
            lightCoeffs[i] += vec3(light[0] * curC,light[1] * curC,light[2]* curC);
        }
        lightCoeffs[i] *= (4.0f * M_PI / numSamples);
    }
    
    return lightCoeffs;
}

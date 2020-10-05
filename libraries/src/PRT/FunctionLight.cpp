//
//  FunctionLight.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/10/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/FunctionLight.h"

vec3 FunctionLight::getLight(SHSample samplesCoefficients)
{
    // light function
    //double light = fmax(0.0, 5 * cos(samplesCoefficients.sphereCoord.x)-4) + fmax(0.0,-4 * sin(samplesCoefficients.sphereCoord.x - M_PI)*cos(samplesCoefficients.sphereCoord.y - 2.5) - 3);
    
    //if (samplesCoefficients.sphereCoord.z < 0)
      //  light = 1;
    //light=(theta<M_PI/6) ? 1 : 0;
    float light=0;
    if (samplesCoefficients.vectorCoord.x<-0.5f)
        light=1;
    
    //light = (theta<M_PI/4) ? 1 : 0;
    return vec3(light, light, light);
};

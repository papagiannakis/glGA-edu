//
//  FunctionLight.h
//  PRT
//
//  Created by margarita on 3/10/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef __MetaioOpenGLES__FunctionLight__
#define __MetaioOpenGLES__FunctionLight__

#include <stdio.h>

#include "Light.h"
#include "Structs.h"

class FunctionLight : public Light
{
    // get light on angle theta,phi (Green Light)
    vec3 getLight(SHSample samplesCoefficients);
};

#endif /* FunctionLight.h */

//
//  Unshadowed.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/UnshadowedDiffuse.h"
#include "PRT/Shadow.h"

UnshadowedDiffuse::UnshadowedDiffuse(int numCoeff):Diffuse(numCoeff)
{
}

double UnshadowedDiffuse::computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient,  KdTreeAccel *kdTree)
{
    return Shadow::computeBrightness(normal, position, samplesCoefficient);
}

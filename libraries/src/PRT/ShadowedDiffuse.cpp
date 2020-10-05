//
//  Shadowed.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/ShadowedDiffuse.h"
#include "PRT/Shadow.h"

ShadowedDiffuse::ShadowedDiffuse(int numCoeff):Diffuse(numCoeff)
{
}

double ShadowedDiffuse::computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient, KdTreeAccel *kdTree)
{
     return Shadow::computeBrightness(normal, position, samplesCoefficient, kdTree);
}

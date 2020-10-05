//
//  Shadowed.h
//  PRT
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef __MetaioOpenGLES__Shadowed__
#define __MetaioOpenGLES__Shadowed__

#include <stdio.h>
#include "PRT/Diffuse.h"
#include "PRT/Structs.h"
#include "KDTree/geometry.h"

using namespace StructDef;

class ShadowedDiffuse : public Diffuse
{
public:
    ShadowedDiffuse(int numCoeff);
    double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient, KdTreeAccel *kdTree);
};

#endif /* ShadowedDiffuse.h */

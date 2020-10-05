//
//  Shadow.h
//  PRT
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef SHADOW_hpp
#define SHADOW_hpp

#include <stdio.h>
#include "PRT/Structs.h"
#include "KDTree/geometry.h"
#include "KDTree/kdtreeaccel.h"

using namespace StructDef;

class Shadow
{
public:
    static double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient);
    static double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient, KdTreeAccel *kdTree);
};

#endif /* Shadow.h*/

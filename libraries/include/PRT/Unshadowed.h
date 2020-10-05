//
//  Unshadowed.h
//  PRT
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef Unshadowed_h
#define Unshadowed_h

#include <stdio.h>

#include "KDTree/geometry.h"
#include "KDTree/kdtreeaccel.h"
#include "Structs.h"

using namespace StructDef;

class Unshadowed
{
public:
    double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient,  KdTreeAccel *kdTree);
};

#endif /* Unshadowed.sh */

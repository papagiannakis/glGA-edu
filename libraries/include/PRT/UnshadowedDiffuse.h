//
//  UnshadowedDiffuse.h
//  PRT
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef UNSHADOWEDDIFFUSE_H
#define UNSHADOWEDDIFFUSE_H

#include <stdio.h>
#include "KDTree/geometry.h"
#include "Diffuse.h"
#include "Structs.h"

class UnshadowedDiffuse : public Diffuse
{
public:
    UnshadowedDiffuse(int numCoeff);
    double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient,  KdTreeAccel *kdTree);
};

#endif /* UnshadowedDiffuse.h */

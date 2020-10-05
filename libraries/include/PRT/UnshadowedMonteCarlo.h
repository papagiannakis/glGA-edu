//
//  UnshadowedMonteCarlo.h
//  PRT
//
//  Created by Margrarita Papaefthimiou on 05/07/2017.
//

#ifndef UnshadowedMonteCarlo_h
#define UnshadowedMonteCarlo_h

#include <stdio.h>
#include "PRT/MonteCarlo.h"
#include "PRT/Unshadowed.h"

class UnshadowedMonteCarlo : public MonteCarlo
{
public:
    double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient,  KdTreeAccel *kdTree);
};

#endif /* UnshadowedMonteCarlo.h*/

//
//  ShadowedMonteCarlo.hpp
//  PRT
//
//  Created by Margrarita Papaefthimiou on 05/07/2017.
//
//

#ifndef ShadowedMonteCarlo_hpp
#define ShadowedMonteCarlo_hpp

#include <stdio.h>
#include "PRT/MonteCarlo.h"
#include "PRT/Shadow.h"

class ShadowedMonteCarlo: public MonteCarlo
{
public:
    double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient,  KdTreeAccel *kdTree);
};

#endif /* ShadowedMonteCarlo_h */

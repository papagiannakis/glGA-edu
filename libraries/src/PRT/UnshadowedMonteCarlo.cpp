//
//  UnshadowedMonteCarlo.cpp
//  glGAframework
//
//  Created by Margrarita Papaefthimiou on 05/07/2017.
//
//

#include "PRT/UnshadowedMonteCarlo.h"
#include "PRT/Shadow.h"

double UnshadowedMonteCarlo::computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient,  KdTreeAccel *kdTree)
{
    return Shadow::computeBrightness(normal, position, samplesCoefficient,kdTree);
}

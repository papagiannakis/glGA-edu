//
//  ShadowedMonteCarlo.cpp
//  glGAframework
//
//  Created by Margrarita Papaefthimiou on 05/07/2017.
//
//

#include "PRT/ShadowedMonteCarlo.h"
#include "PRT/Shadow.h"

double ShadowedMonteCarlo::computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient,  KdTreeAccel *kdTree)
{
    return Shadow::computeBrightness(normal, position, samplesCoefficient,kdTree);
}

//
//  CGA_SH_Rotations.h
//  PRT
//
//  Created by Margrarita Papaefthimiou on 16/10/16.
//  Copyright © 2016 George Papagiannakis. All rights reserved.
//

#ifndef CGA_SH_Rotations_hpp
#define CGA_SH_Rotations_hpp

#include <stdio.h>
#include "PRT/CGA_SH.hpp"

#define GLM_SWIZZLE
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "CGA_SH.hpp"

using namespace glm;

void initializeRot(int bands);
void printLightCoefficients(glm::vec3 *lightCoeffs, int bands);
float* getSHRotation(vec4 rotor, int bands,  vec3* lines);
glm::vec3 rotateline(glm::vec4 R, float x, float y, float z);

#endif /* CGA_SH_Rotations_hpp */

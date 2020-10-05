//
//  CGA_SH_Rotations.cpp
//  glGACharacterApp
//
//  Created by Margrarita Papaefthimiou on 16/10/16.
//  Copyright © 2016 George Papagiannakis. All rights reserved.
//

#include "CGA_SH_Rotations.h"
#include <math.h>
#include <stdio.h>
#include <iostream>

// GLM lib
// http://glm.g-truc.net/api/modules.html
#define GLM_SWIZZLE
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include "SphericalHarmonics.h"
#include <PRT/HDRLight.h>

using namespace glm;
vec3* pos;
float* val;

void printLightCoefficients(glm::vec3 *lightCoeffs, int bands)
{
    int num=bands*bands;
    for (int i=0; i<num; i++)
    {
        std::cout << i << " " << lightCoeffs[i].x << " " << lightCoeffs[i].y << " " << lightCoeffs[i].z << std::endl;
    }
    std::cout << std::endl;
}

vec3 rotateline(vec4 R, float x, float y, float z)
{
    float val1=((-(R[1] * x)) + (-(R[2] * y)) + (-(R[3] * z)));
    float val2=(R[0] * x + (-(R[2] * z)) + R[3] * y);
    float val3=R[0] * z + (-(R[1] * y)) + R[2] * x;
    float val4=R[0] * y + R[1] * z + (-(R[3] * x));
    
    float x1 = val1 * (-R[1]) + val2 * R[0] + (-((val4) * (-R[3]))) + (val3) * (-R[2]); // e1 ^ e2
    float y1 = val1 * (-R[2]) + val2 * (-R[3]) + (val4) * R[0] + (-((val3) * (-R[1]))); // e1 ^ e3
    float z1 = val1 * (-R[3]) + (-(val2 * (-R[2]))) + (val4) * (-R[1]) + (val3) * R[0]; // e2 ^ e3
    
    return vec3(x1, y1, z1);
}

void initializeRot(int bands)
{
    pos= new vec3[bands*bands];
    vec2* anglesMax= new vec2[bands*bands];
    float* max= new float[bands*bands];
    val = new float[bands*bands];
    
    for (int i=0; i<bands*bands; i++)
        max[i]=0;
    
    float sqrtSamples2=500;
    
    for (int i=0; i<sqrtSamples2; i++)
    {
        for (int j=0; j<sqrtSamples2; j++)
        {
            double x = (i + 0)/ (double)sqrtSamples2;
            double y = (j + 0)/(double)sqrtSamples2;
            
            double theta = 2.0f * acos(std::sqrt(1.0f - x));
            double phi = 2.0f * M_PI * y;
            
            SHSample cur;
            
            cur.sphereCoord = vector3(theta,phi,1.0) ;
            
            float X = sin(theta)*cos(phi);
            float Y = sin(theta)*sin(phi);
            float Z = cos(theta);
            
            cur.vectorCoord = vector3(X,Y,Z);
            cur.coeff = new double[bands*bands];
            
            for(int l=0; l<bands; l++)
            {
                for(int m=-l; m<=l; m++)
                {
                    int index = l*(l+1)+m;
                    cur.coeff[index] = computeSH1(l, m, X, Y, Z);
                    if (cur.coeff[index]>max[index])
                    {
                        max[index]=cur.coeff[index];
                        anglesMax[index]=vec2(theta,phi);
                    }
                }
            }
        }
    }
    
    for (int i=0; i<bands*bands; i++)
    {
        float X = sin(anglesMax[i][0])*cos(anglesMax[i][1]);
        float Y = sin(anglesMax[i][0])*sin(anglesMax[i][1]);
        float Z = cos(anglesMax[i][0]);
        
        pos[i] = vec3(X, Y, Z);
    }
    
    for(int l=0; l<bands; l++)
    {
        for(int m=-l; m<=l; m++)
        {
            int index = l*(l+1)+m;
            val[index]=computeSH1(l, m, pos[index][0], pos[index][1], pos[index][2]);
        }
    }
}

float* getSHRotation(vec4 rotor, int bands,  vec3* lines)
{
    float* rotatedLightCoeffs = new float[bands*bands];
    
    rotatedLightCoeffs[0]=1;
    
    lines[0]=  vec3(0, 0, 1);
    
    for(int l=1; l<bands; l++)
    {
        for(int m=-l; m<=l; m++)
        {
            int index = l*(l+1)+m;
            vec3 rot1_1 = rotateline(rotor, pos[index][0], pos[index][1], pos[index][2]);
            lines[index]=  vec3(rot1_1[0], rot1_1[1], rot1_1[2]);
            rotatedLightCoeffs[index]=computeSH1(l,m,rot1_1[0],rot1_1[1],rot1_1[2])/val[index];
        }
    }
    return rotatedLightCoeffs;
}

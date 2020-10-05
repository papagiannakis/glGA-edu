//
//  Sampling.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/2/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/Sample.h"
#include "PRT/Utils.h"
#include "PRT/CGA_SH.hpp"

#ifndef __APPLE__
#define _USE_MATH_DEFINES 
#include <math.h>
#endif  // __APPLE__

#include "SphericalHarmonics.h"

Sample::Sample(int samples, int bands)
{
    this->samples = samples;
    this->bands = bands;
}

vector<SHSample> Sample::setCoefficients()
{
    int sqrtSamples = std::sqrt(samples);
    
    for (int i=0; i<sqrtSamples; i++)
    {
        for (int j=0; j<sqrtSamples; j++)
        {
            // generate samples
            double x = (i + (double)rand()/RAND_MAX)/ (double)sqrtSamples;
            double y = (j + (double)rand()/RAND_MAX)/(double)sqrtSamples;
            
            double theta = 2.0f * acos(std::sqrt(1.0f - x));
            double phi = 2.0f * M_PI * y;
            
            SHSample cur;
            
            cur.sphereCoord = vector3(theta,phi,1.0) ;
            
            //for function light
            float X = sin(theta)*cos(phi);
            float Y = sin(theta)*sin(phi);
            float Z = cos(theta);
            
            cur.vectorCoord = vector3(X,Y,Z);
            
            cur.coeff = new double[bands*bands];
            
            // set SH coefficients
            for(int l=0; l<bands; l++)
            {
                for(int m=-l; m<=l; m++)
                {
                    int index = l*(l+1)+m;
                    SphericalHarmonics yy;
                    cur.coeff[index] = yy.y(l, m, theta, phi);
                }
            }
            samplesCoefficients.push_back(cur);
        }
    }
    return samplesCoefficients;
}

vector<SHSample> Sample::setCoefficientsCGA(vector<SHSample>  samplesCoefficients)
{
    int sqrtSamples = std::sqrt(samples);
    
    int index=0;
    for (int i=0; i<sqrtSamples; i++)
    {
        for (int j=0; j<sqrtSamples; j++)
        {
            SHSample cur;
            
            cur.sphereCoord = samplesCoefficients[index].sphereCoord;
            
            cur.vectorCoord = samplesCoefficients[index].vectorCoord;
            
            cur.coeff = new double[bands*bands];
            
            // set SH coefficients
            for(int l=0; l<bands; l++)
            {
                for(int m=-l; m<=l; m++)
                {
                    int index = l*(l+1)+m;
                    cur.coeff[index] = computeSH(l, m, cur.vectorCoord.x, cur.vectorCoord.y,cur.vectorCoord.z);
                }
            }
            samplesCoefficients[index]=cur;
            index++;
        }
    }
    return samplesCoefficients;
}

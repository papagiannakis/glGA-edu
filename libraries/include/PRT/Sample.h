//
//  Sample.h
//  PRT
//
//  Created by margarita on 3/2/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef SAMPLE_hpp
#define SAMPLE_hpp

#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/math/special_functions/spherical_harmonic.hpp>
#include "Structs.h"

using namespace glm;
using namespace boost::math;
using namespace std;
using namespace StructDef;

class Sample
{
    
private:
    int samples;
    int bands;
    vector<SHSample> samplesCoefficients;
    
public:
    
    Sample(int numSamples, int numBands);
    
    // sampling, SH coeffs
    vector<SHSample> setCoefficients();
    // sampling, CGA coeffs
    vector<SHSample> setCoefficientsCGA(vector<SHSample> samplesCoefficients);
    
    // setters
    void setSamples(int samples)
    {
        this->samples = samples;
    }
    
    void setBands(int bands)
    {
        this->bands = bands;
    }
    
    // getters
    int getSamples()
    {
        return samples;
    }
    
    int getBands()
    {
        return bands;
    }
    
    vector<SHSample> getSamplesCoefficients()
    {
        return samplesCoefficients;
    }
};

#endif /* defined(__MetaioOpenGLES__Sampling__) */

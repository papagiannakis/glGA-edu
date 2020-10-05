//
//  glGAMeshPRT.h
//  glGA
//
//  Created by Margrarita Papaefthimiou on 9/14/15.
//  Copyright (c) 2015 George Papagiannakis. All rights reserved.
//

#ifndef __glGA__glGARigMeshPRT__
#define __glGA__glGARigMeshPRT__

// to use PRT uncomment
//#define USE_PRT

#include <stdio.h>
#include <glGA/glGARigMesh.h>

#define USE_PRT

#if defined USE_PRT || defined USE_MC
#include "PRT/Diffuse.h"
#include "PRT/ShadowedDiffuse.h"
#include "PRT/UnshadowedDiffuse.h"
#include "PRT/ShadowedMonteCarlo.h"
#include "PRT/UnshadowedMonteCarlo.h"
#endif

#if defined USE_PRT || defined USE_MC
class RigMeshPRT: public RigMesh
{
public:
    static vector<SHSample> samplesCoefficients;
    static int numCoeff;
    
    std::vector<glm::vec3>          Colors;
    std::string filename;
    Illumination *diffuse;
    bool shadow;
    
    bool loadRigMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale);
    bool loadRigMeshMC(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale);
    bool loadRigMeshCGA(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale);
    void initializeColors(KdTreeAccel *kdTree);
    void initializeColors(KdTreeAccel *kdTree, vector<SHSample> samplesCoefficientsCGA);
    void computeColors();
    std::vector<glm::vec3> getColors();
    void updatePRTLight(glm::vec3* lightCoeffs);
    void setColors(std::vector<glm::vec3> Colors);
    bool loadRigMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyoptimised, Illumination *illuminationType);
    void setLight(Light *l);
    
};
#endif
#endif /* glGAMeshPRT.h */

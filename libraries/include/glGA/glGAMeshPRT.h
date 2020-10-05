//
//  glGAMesh.h
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//
//  References: 
//      http://ogldev.atspace.co.uk/, Etay Meiri

#ifndef glGACharacterApp_glGAMeshPRT_h
#define glGACharacterApp_glGAMeshPRT_h

// to use PRT uncomment
//#define USE_PRT

#include "glGAMesh.h"

#define USE_PRT

#if defined USE_PRT || defined USE_MC
#include "PRT/Diffuse.h"
#include "PRT/ShadowedDiffuse.h"
#include "PRT/UnshadowedDiffuse.h"
#include "PRT/Illumination.h"
#include "PRT/UnshadowedMonteCarlo.h"
#include "PRT/ShadowedMonteCarlo.h"
#endif

#if defined USE_PRT || defined USE_MC
class MeshPRT: public Mesh
{

public:
    static std::vector<SHSample> samplesCoefficients;
    static int numCoeff;
    std::string filename;
    std::vector<glm::vec3> Colors;
    Illumination *diffuse;
    bool shadow;
    
    bool loadMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyOptimised);
    bool loadMeshMC(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyOptimised);
    bool loadMeshCGA(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyoptimised);
    void initializeColors(KdTreeAccel *kdTree);
    void initializeColors(KdTreeAccel *kdTree, vector<SHSample> samplesCoefficientsCGA);
    void computeShadow(KdTreeAccel *kdTree, std::vector<glm::vec3> Positions2, Light *light1);
    void initializeColors(KdTreeAccel *kdTree, glm::mat4 quatToMat1, std::vector<glm::vec3> Positions2, Light *light1);
    void computeColors();
    std::vector<glm::vec3> getColors();
    void updatePRTLight(glm::vec3* lightCoeffs);
    void setLight(Light* l);
    void setColors(std::vector<glm::vec3> Colors);
    bool getShadowed();
};
#endif
#endif /* glGAMeshPRT_h */

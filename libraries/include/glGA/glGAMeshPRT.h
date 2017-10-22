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

#if defined USE_PRT
#include "PRT/Diffuse.h"
#include "PRT/Shadowed.h"
#include "PRT/Unshadowed.h"
#endif

#if defined USE_PRT
class MeshPRT: public Mesh
{

public:
    static vector<SHSample> samplesCoefficients;
    static int numCoeff;
    static glm::vec3* lightCoeffs;
    
    std::vector<glm::vec3> Colors;
    Diffuse *diffuse;
    std::string filename;
    
    bool loadMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyOptimised);
    void initializeColors(KdTreeAccel *kdTree);
    void computeColors();
    
private:
    mat4 vNormalTransformation;
    bool shadowed;
    
    
    void computeNormalTransformation(glm::vec3 rotation);
    void initMesh(const aiMesh* paiMesh,
                  std::vector<glm::vec3>& positions,
                  std::vector<glm::vec3>& normals,
                  std::vector<glm::vec2>& texCoords,
                  std::vector<unsigned int>& indices,
                  std::vector<glm::vec3>& tangents);
};
#endif
#endif //glGACharacterApp_glGAMeshPRT_h

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

#if defined USE_PRT
#include "PRT/Diffuse.h"
#include "PRT/Shadowed.h"
#include "PRT/Unshadowed.h"
#endif

#if defined USE_PRT
class RigMeshPRT: public RigMesh
{
public:
    static vector<SHSample> samplesCoefficients;
    static int numCoeff;
    static glm::vec3* lightCoeffs;
    
    std::vector<glm::vec3>          Colors;
    Diffuse *diffuse;
    std::string filename;
    
    bool loadRigMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale);
    void initializeColors(KdTreeAccel *kdTree);
    void computeColors();
    
private:
    mat4 vNormalTransformation;
    bool shadowed;
    
    
    void computeNormalTransformation(glm::vec3 rotation);
    void initRigMesh(uint RigMeshIndex,
                     const aiMesh* paiMesh,
                     std::vector<glm::vec3>& positions,
                     std::vector<glm::vec3>& normals,
                     std::vector<glm::vec2>& texCoords,
                     std::vector<VertexBoneData>& bones,
                     std::vector<unsigned int>& indices
                     );
};
#endif
#endif /* defined(__glGA__glGAMeshPRT__) */

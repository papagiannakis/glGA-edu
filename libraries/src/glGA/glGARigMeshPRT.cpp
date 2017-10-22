//
//  glGAMeshPRT.cpp
//  glGA
//
//  Created by Margrarita Papaefthimiou on 9/14/15.
//  Copyright (c) 2015 George Papagiannakis. All rights reserved.
//
#ifndef __APPLE__
#include "glGA/glGARigMeshPRT.h"
#else
#include "glGARigMeshPRT.h"
#endif  // __APPLE__

#if defined USE_PRT
vector<SHSample> RigMeshPRT::samplesCoefficients;
int RigMeshPRT::numCoeff;
glm::vec3* RigMeshPRT::lightCoeffs;

bool RigMeshPRT::loadRigMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale)
{
    bool loaded = RigMesh::loadRigMesh(filename, rotation, position, scale);
    
    // compute normals base on rotation
    computeNormalTransformation(rotation);
    
    this->filename=filename+"D";
    
    this->shadowed=shadowed;
    
    if (shadowed==true)
    {
        this->diffuse = new Shadowed();
    }
    else
    {
        this->diffuse = new Unshadowed();
    }
    
    return loaded;
}

void RigMeshPRT::computeNormalTransformation(glm::vec3 rotation)
{
    glm::mat4 rotateX = rotate(mat4(1.0), glm::degrees(rotation.x), vec3(1.0, 0.0, 0.0));
    glm::mat4 rotateY = rotate(rotateX, glm::degrees(rotation.y), vec3(0.0, 1.0, 0.0));
    mat4 rotateZ = rotate(rotateY, glm::degrees(rotation.z), vec3(0.0, 0.0, 1.0));
    
    vNormalTransformation = rotateZ;
}

void RigMeshPRT::initRigMesh(uint RigMeshIndex,
                          const aiMesh* paiMesh,
                          std::vector<glm::vec3>& positions,
                          std::vector<glm::vec3>& normals,
                          std::vector<glm::vec2>& texCoords,
                          std::vector<VertexBoneData>& bones,
                          std::vector<unsigned int>& indices
                          )
{
    const aiVector3D    zero3D(0.0f,0.0f,0.0f);
    
    // Populate the vertex attribute vectors
    for (unsigned int i=0; i < paiMesh->mNumVertices; ++i)
    {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &zero3D;
        
        positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        
        if(pNormal!=NULL)
        {
            vec4 normal(pNormal->x, pNormal->y, pNormal->z, 1.0);
            normal = vNormalTransformation * normal;
            normals.push_back(glm::vec3(normal[0], normal[1], normal[2]));
        }
        
        texCoords.push_back(glm::vec2(pTexCoord->x,pTexCoord->y));
    }
    
    loadBones(RigMeshIndex, paiMesh, bones);
    
    // Populate the index buffer
    for (unsigned int i=0; i < paiMesh->mNumFaces; ++i)
    {
        const aiFace& face = paiMesh->mFaces[i];
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
    
    GLExitIfError();
}//end initFromScene()


void RigMeshPRT::computeColors()
{
    Colors.resize(Positions.size());
    for (int i=0; i<Positions.size(); i++)
    {
        glm::vec3 curColor(0.0,0.0,0.0);
        for (int j=0; j<numCoeff; j++)
        {
            glm::vec3 b = lightCoeffs[j];
            
            double a = diffuse->vertCoeff[i*numCoeff + j];
            
            glm::vec3 color = vec3(b[0]*a,b[1]*a, b[2]*a);
            curColor +=  color;
        }
        glm::vec3 finalColor(curColor[0]/M_PI,curColor[1]/M_PI,curColor[2]/M_PI);
        Colors[i] = finalColor;
    }
}

void RigMeshPRT::initializeColors(KdTreeAccel *kdTree)
{
    diffuse->coefficients(initialTransformation, Normals, Positions, samplesCoefficients, numCoeff, filename, shadowed, kdTree);
    
    computeColors();
}
#endif
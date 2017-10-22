//
//  glGAMesh.cpp
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//
//  References: 
//      http://ogldev.atspace.co.uk/, Etay Meiri

#include "glGA/glGAMeshPRT.h"

#if defined USE_PRT
vector<SHSample> MeshPRT::samplesCoefficients;
int MeshPRT::numCoeff;
glm::vec3* MeshPRT::lightCoeffs;

void MeshPRT::computeNormalTransformation(glm::vec3 rotation)
{
    glm::mat4 rotateX = rotate(mat4(1.0), glm::degrees(rotation.x), vec3(1.0, 0.0, 0.0));
    glm::mat4 rotateY = rotate(rotateX, glm::degrees(rotation.y), vec3(0.0, 1.0, 0.0));
    mat4 rotateZ = rotate(rotateY, glm::degrees(rotation.z), vec3(0.0, 0.0, 1.0));
    
    vNormalTransformation = rotateZ;
}

void MeshPRT::computeColors()
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

void MeshPRT::initializeColors(KdTreeAccel *kdTree)
{
    diffuse->coefficients(initialTransformation, Normals, Positions, samplesCoefficients, numCoeff, filename, shadowed, kdTree);
    
    computeColors();
}

bool MeshPRT::loadMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyoptimised)
{
    bool loaded = Mesh::loadMesh(filename, rotation, position, scale, false);
    
    // compute normals base on rotation
    computeNormalTransformation(rotation);
    
    this->filename=filename+"S";
    
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

void MeshPRT::initMesh(const aiMesh* paiMesh,
                    std::vector<glm::vec3>& positions,
                    std::vector<glm::vec3>& normals,
                    std::vector<glm::vec2>& texCoords,
                    std::vector<unsigned int>& indices,
                    std::vector<glm::vec3>& tangents
                    )
{
    const aiVector3D    zero3D(0.0f,0.0f,0.0f);
    
    // Populate the vertex attribute vectors
    for (unsigned int i=0; i < paiMesh->mNumVertices; ++i)
    {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &zero3D;
        const aiVector3D* pTangent  = &(paiMesh->mTangents[i]);
        
        positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        if (paiMesh->HasNormals())
        {
            if(pNormal!=NULL)
            {
                vec4 normal(pNormal->x, pNormal->y, pNormal->z, 1.0);
                normal = vNormalTransformation * normal;
                normals.push_back(glm::vec3(normal[0], normal[1], normal[2]));
            }
        }
        
        texCoords.push_back(glm::vec2(pTexCoord->x,pTexCoord->y));
        
        if (paiMesh->HasTangentsAndBitangents())
        {
            if (pTangent!=NULL)
                tangents.push_back(glm::vec3(pTangent->x, pTangent->y, pTangent->z));
        }
    }
    
    // Populate the index buffer
    for (unsigned int i=0; i < paiMesh->mNumFaces; ++i)
    {
        const aiFace& face = paiMesh->mFaces[i];
        //assert(face.mNumIndices == 3);
        if(face.mNumIndices == 3)
        {
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }
        else if (face.mNumIndices == 2)
        {
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
        }
    }
    
}//end initFromScene()

#endif





























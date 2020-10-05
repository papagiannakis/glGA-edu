//
//  glGAMeshPRT.cpp
//  glGA
//
//  Created by Margrarita Papaefthimiou on 9/14/15.
//  Copyright (c) 2015 George Papagiannakis. All rights reserved.
//

#define USE_PRT 

#ifndef __APPLE__
#include "glGA/glGARigMeshPRT.h"
#else
#include "glGARigMeshPRT.h"
#endif  // __APPLE__

#if defined USE_PRT || defined USE_MC
vector<SHSample> RigMeshPRT::samplesCoefficients;
int RigMeshPRT::numCoeff;

bool RigMeshPRT::loadRigMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale)
{
    bool loaded = RigMesh::loadRigMesh(filename, rotation, position, scale);
    
    // compute normals base on rotation
    computeNormalTransformation(rotation);
    
    this->shadow=shadowed;
    
    if (shadowed)
    {
        this->filename=filename+"D_S_";
        this->diffuse = new ShadowedDiffuse(RigMeshPRT::numCoeff);
    }
    else
    {
        this->filename=filename+"D_S_";
        this->diffuse = new UnshadowedDiffuse(RigMeshPRT::numCoeff);
    }
    
    this->diffuse->setRotation(vNormalTransformation);
    
    return loaded;
}

bool RigMeshPRT::loadRigMeshCGA(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale)
{
    bool loaded = loadRigMesh(filename, shadowed, rotation, position, scale);
    
    this->filename=this->filename+"CGA_";
    
    return loaded;
}

bool RigMeshPRT::loadRigMeshMC(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale)
{
    bool loaded = RigMesh::loadRigMesh(filename, rotation, position, scale);
    
    // rotate normals
    computeNormalTransformation(rotation);
    
    this->shadow=shadowed;
    
    if (shadowed)
    {
        this->filename=filename+"D_S_MC";
        this->diffuse = new ShadowedMonteCarlo();
    }
    else
    {
        this->filename=filename+"D_U_MC";
        this->diffuse = new UnshadowedMonteCarlo();
    }
    
    this->diffuse->setRotation(vNormalTransformation);
    
    return loaded;
}

void RigMeshPRT::setLight(Light *l)
{
    if (shadow)
    {   ShadowedMonteCarlo* b = dynamic_cast<ShadowedMonteCarlo*>(this->diffuse);
        b->setLight(l);
    }
    else
    {
        UnshadowedMonteCarlo* b = dynamic_cast<UnshadowedMonteCarlo*>(this->diffuse);
        b->setLight(l);
    }
}

void RigMeshPRT::updatePRTLight(glm::vec3* lightCoeffs)
{
    if (shadow)
    {   ShadowedDiffuse* b = dynamic_cast<ShadowedDiffuse*>(this->diffuse);
        b->setLightCoeffs(lightCoeffs);
    }
    else
    {
        UnshadowedDiffuse* b = dynamic_cast<UnshadowedDiffuse*>(this->diffuse);
        b->setLightCoeffs(lightCoeffs);
    }
}

std::vector<glm::vec3> RigMeshPRT::getColors()
{
    return Colors;
}

void RigMeshPRT::initializeColors(KdTreeAccel *kdTree)
{
    diffuse->coefficients(initialTransformation, Normals, Positions, samplesCoefficients, filename, shadow, kdTree);
    
    computeColors();
}

void RigMeshPRT::initializeColors(KdTreeAccel *kdTree, vector<SHSample> samplesCoefficientsCGA)
{
    diffuse->coefficients(initialTransformation, Normals, Positions, samplesCoefficientsCGA, filename, shadow, kdTree);
    
    computeColors();
}

void RigMeshPRT::computeColors()
{
    Colors = diffuse->computeColors(Positions.size());
}

void RigMeshPRT::setColors(std::vector<glm::vec3> Colors)
{
    this->Colors = Colors;
}
#endif

//
//  glGAMesh.cpp
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//
//  References: 
//      http://ogldev.atspace.co.uk/, Etay Meiri

#define USE_PRT 

#if defined USE_PRT || defined USE_MC
#include "glGA/glGAMeshPRT.h"
#include "PRT/UnshadowedDiffuse.h"
#include "PRT/ShadowedDiffuse.h"
#endif

#if defined USE_PRT || defined USE_MC
int MeshPRT::numCoeff;
vector<SHSample> MeshPRT::samplesCoefficients;

void MeshPRT::computeColors()
{
    Colors = diffuse->computeColors(Positions.size());
}

std::vector<glm::vec3> MeshPRT::getColors()
{
    return Colors;
}

bool MeshPRT::loadMeshMC(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyoptimised)
{
    bool loaded = Mesh::loadMesh(filename, rotation, position, scale, false);
    
    this->shadow=shadowed;
    
    if (shadowed)
    {
        this->filename=filename+"S_S_MC";
        this->diffuse = new ShadowedMonteCarlo();
    }
    else
    {
        this->filename=filename+"S_U_MC";
        this->diffuse = new UnshadowedMonteCarlo();
    }
    
    this->diffuse->setRotation(vNormalTransformation);
    
    return loaded;
}

bool MeshPRT::loadMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyoptimised)
{
    bool loaded = Mesh::loadMesh(filename, rotation, position, scale, false);
    
    this->shadow=shadowed;
    
    if (shadowed)
    {
        this->filename=filename+"S_S_";
        this->diffuse = new ShadowedDiffuse(MeshPRT::numCoeff);
    }
    else
    {
        this->filename=filename+"S_U_";
        this->diffuse = new UnshadowedDiffuse(MeshPRT::numCoeff);
    }
    
    this->diffuse->setRotation(vNormalTransformation);
    
    return loaded;
}

bool MeshPRT::loadMeshCGA(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyoptimised)
{
    bool loaded = loadMesh(filename, shadowed, rotation, position, scale, fullyoptimised);
    
    this->filename=this->filename+"CGA_";
    
    return loaded;
}

void MeshPRT::initializeColors(KdTreeAccel *kdTree)
{
    diffuse->coefficients(initialTransformation, Normals, Positions, samplesCoefficients, filename, shadow, kdTree);
    
    computeColors();
}

void MeshPRT::initializeColors(KdTreeAccel *kdTree, vector<SHSample> samplesCoefficientsCGA)
{
    diffuse->coefficients(initialTransformation, Normals, Positions, samplesCoefficientsCGA, filename, shadow, kdTree);
    
    computeColors();
}

void MeshPRT::setColors(std::vector<glm::vec3> Colors)
{
    this->Colors=Colors;
}

void MeshPRT::setLight(Light *l)
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

void MeshPRT::updatePRTLight(glm::vec3* lightCoeffs)
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

bool MeshPRT::getShadowed()
{
    return this->shadow;
}

void MeshPRT::computeShadow(KdTreeAccel *kdTree, std::vector<glm::vec3> Positions2, Light *light1)
{
    if (shadow)
    {   ShadowedMonteCarlo* mc = dynamic_cast<ShadowedMonteCarlo*>(this->diffuse);
         mc->computeShadow(initialTransformation, Normals, Positions2, samplesCoefficients, numCoeff, filename, shadow, kdTree, light1);
    }
    else
    {
        UnshadowedMonteCarlo* mc = dynamic_cast<UnshadowedMonteCarlo*>(this->diffuse);
         mc->computeShadow(initialTransformation, Normals, Positions2, samplesCoefficients, numCoeff, filename, shadow, kdTree, light1);
    }
}

void MeshPRT::initializeColors(KdTreeAccel *kdTree, glm::mat4 quatToMat1, std::vector<glm::vec3> Positions2, Light *light)
{
    if (shadow)
    {   ShadowedMonteCarlo* mc = dynamic_cast<ShadowedMonteCarlo*>(this->diffuse);
        mc->coefficients(initialTransformation, Normals, Positions2, samplesCoefficients, numCoeff, filename, shadow, kdTree, quatToMat1, light);
    }
    else
    {
        UnshadowedMonteCarlo* mc = dynamic_cast<UnshadowedMonteCarlo*>(this->diffuse);
        mc->coefficients(initialTransformation, Normals, Positions2, samplesCoefficients, numCoeff, filename, shadow, kdTree, quatToMat1, light);
    }
    
    computeColors();
}
#endif

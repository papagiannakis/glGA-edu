//
//  glGARigMesh.cpp
//
//
//  Created by George Papagiannakis
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//
//  References:
//      http://ogldev.atspace.co.uk/, Etay Meiri

#include <iostream>
#include <cassert>
#include <cmath>
#include <glm/gtx/dual_quaternion.hpp>
#include "glGA/glGARigMesh.h"
#if INTERPOLATION==GA_VERSOR || INTERPOLATION==CGA_VERSOR
#include "space/vsr_cga3D_op.h"
#include "detail/vsr_generic_op.h"
#endif

#include <glGAMath/e3ga.h>
#include <glGAMath/e3ga_util.h>

using namespace std::chrono;

//#define USE_GA to employ glGAMath and Geometric Algebra
// or define it in project as C++ Preprocessor
// do not use it if you are not sure what it does!

// GLM
#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

/*
 This utility function finds a free slot in the VertexBoneData structure and places the bone ID and weight in it. Some vertices will be influenced by less than four bones but since the weight of a non existing bone remains zero (see the constructor of VertexBoneData) it means that we can use the same weight calculation for any number of bones.
 */

RotorDef getRotorDef(float* finalmotor)
{
    RotorDef result(finalmotor[0], finalmotor[6], finalmotor[7], finalmotor[8], finalmotor[10], finalmotor[11], finalmotor[13], finalmotor[26]);
    return result;
}

RotorDefScale getRotorDef2(float* finalmotor)
{
    RotorDefScale result(finalmotor[0], finalmotor[6], finalmotor[7], finalmotor[8], finalmotor[10], finalmotor[11], finalmotor[13], finalmotor[26], finalmotor[15], finalmotor[28], finalmotor[29], finalmotor[30]);
    return result;
}

#if INTERPOLATION==CGA_GAALOP
float RigMesh::IdentityVersor[32]={1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif

#if INTERPOLATION==GA_GAIGEN
void RigMesh::boneTransformGA_Gaigen(float TimeInSeconds, std::vector<glm::mat4>& transforms)
{
    std::vector<RotorDef> transformsVersors;
    //    std::cout<<"@@@ entered RigMesh::boneTransform()"<< std::endl;
    glm::mat4 Identity(1.0);
    
    if (m_pScene->HasAnimations()) {
        
        //std::cout<<"found "<<m_pScene->mNumAnimations<<" animations in loaded Scene!"<<std::endl;
        assert(m_pScene!=NULL);
        assert( (m_pScene->mAnimations[0]) !=NULL);
        const aiAnimation* basicAnim = m_pScene->mAnimations[0];
        //std::cout<<"found basicAnim with name: "<< basicAnim->mName.C_Str()<<std::endl;
        
        assert(basicAnim !=NULL);
        //float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
        float TicksPerSecond;
        if ( basicAnim->mTicksPerSecond != 0)
            TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond;
        else
            TicksPerSecond = 25.0f;;
        //std::cout<<"found TicksPerSecond in animation: "<< TicksPerSecond<<std::endl;
        
        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float Duration = m_pScene->mAnimations[0]->mDuration;
        float animationTime = fmod(TimeInTicks,Duration);
        
        readNodeHierarchyGA_Gaigen(animationTime, m_pScene->mRootNode, Identity);
        
        transforms.resize(m_NumBones);
        
        for (uint i = 0 ; i < m_NumBones ; i++)
        {
            transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
        
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;
}
void RigMesh::readNodeHierarchyGA_Gaigen(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    glm::mat4 NodeTransformation;
    aiMat2glmMat( &(pNode->mTransformation), NodeTransformation);
    
    const glGA::aiNodeAnimglGA* pNodeAnim = findNodeAnim(aiAnim, NodeName);
    
    if (pNodeAnim) {
        high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        glm::mat4 rotationMatrix1 = calcInterpolatedRotationGA_GAIGEN(animationTime, pNodeAnim);
        
        aiVector3D translation1;
        calcInterpolatedPosition(translation1, animationTime, pNodeAnim->assimpAiNode);
        glm::mat4 translationMatrix1 = glm::translate(glm::mat4(1.0), glm::vec3(translation1.x, translation1.y, translation1.z));
        
        NodeTransformation = translationMatrix1 * rotationMatrix1;
        high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        duration<double, std::milli> time_span = duration_cast<duration<double, std::milli> >(end - start);
        total += time_span.count();
        i++;
        avgTime = total/i;
    }
    
    glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchyGA_Gaigen(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

float RigMesh::calcInterpolatedScalingGA(float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim)
{
    if (pNodeAnim->assimpAiNode->mNumScalingKeys == 1)
    {
        aiVector3D Out = pNodeAnim->assimpAiNode->mScalingKeys[0].mValue;
        float Dilator1 = (1.0 - Out.y) * (1.0 + Out.y) / ((1.0 + Out.y) * (1.0 + Out.y));
        return Dilator1;
    }
    
    uint ScalingIndex = findScaling(animationTime, pNodeAnim->assimpAiNode);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->assimpAiNode->mNumScalingKeys);
    float DeltaTime = pNodeAnim->assimpAiNode->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->assimpAiNode->mScalingKeys[ScalingIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->assimpAiNode->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    // GPTODO CHECK THIS ASSERT
    //    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->assimpAiNode->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->assimpAiNode->mScalingKeys[NextScalingIndex].mValue;
    
    float d1=Start.x;
    float d2=End.y;
    
    float Dilator1 = (1.0 - d1) * (1.0 + d1) / ((1.0 + d1) * (1.0 + d1)); // einf ^ e0
    float Dilator2 = (1.0 - d2) * (1.0 + d2) / ((1.0 + d2) * (1.0 + d2)); // einf ^ e0
    float finalD = Dilator1 + (Dilator2 - Dilator1) * Factor; // einf ^ e0
    
    
    return finalD;
}

glm::mat4 RigMesh::calcInterpolatedRotationGA_GAIGEN(float animationTime, const glGA::aiNodeAnimglGA* nodeAnim)
{
    // we need at least two values to interpolate...
    if (nodeAnim->assimpAiNode->mNumRotationKeys == 1)
    {
        aiQuaternion out;
        out = nodeAnim->assimpAiNode->mRotationKeys[0].mValue;
        aiMatrix4x4 assimpRotationMatrix1(out.GetMatrix());
        glm::mat4 rotationMatrix;
        aiMat2glmMat(&assimpRotationMatrix1, rotationMatrix);
        return rotationMatrix;
    }
    
    unsigned int rotationIndex = findRotation(animationTime, nodeAnim->assimpAiNode);
    unsigned int nextRotationIndex = (rotationIndex + 1);
    assert(nextRotationIndex < nodeAnim->assimpAiNode->mNumRotationKeys);
    float deltaTime = (float)(nodeAnim->assimpAiNode->mRotationKeys[nextRotationIndex].mTime - nodeAnim->assimpAiNode->mRotationKeys[rotationIndex].mTime);
    float factor = (animationTime - (float)nodeAnim->assimpAiNode->mRotationKeys[rotationIndex].mTime) / deltaTime;
    
    const aiQuaternion& startRotationQ = nodeAnim->assimpAiNode->mRotationKeys[rotationIndex].mValue;
    const glm::quat start(startRotationQ.w, startRotationQ.x, startRotationQ.y, startRotationQ.z);
    
    const e3ga::rotor StartQuat = nodeAnim->rotorGaigen2[rotationIndex];
    const e3ga::rotor EndQuat   = nodeAnim->rotorGaigen2[nextRotationIndex];
    
    return interpolateGA_GAIGEN(StartQuat, EndQuat, factor,m_GAfactorDenominator, start);
}
#endif

#if INTERPOLATION==CGA_GAIGEN
void RigMesh::boneTransformCGA_Gaigen(float TimeInSeconds, std::vector<glm::mat4>& transforms)
{
    std::vector<RotorDef> transformsVersors;
    //    std::cout<<"@@@ entered RigMesh::boneTransform()"<< std::endl;
    glm::mat4 Identity(1.0);
    
    if (m_pScene->HasAnimations()) {
        
        //std::cout<<"found "<<m_pScene->mNumAnimations<<" animations in loaded Scene!"<<std::endl;
        assert(m_pScene!=NULL);
        assert( (m_pScene->mAnimations[0]) !=NULL);
        const aiAnimation* basicAnim = m_pScene->mAnimations[0];
        //std::cout<<"found basicAnim with name: "<< basicAnim->mName.C_Str()<<std::endl;
        
        assert(basicAnim !=NULL);
        //float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
        float TicksPerSecond;
        if ( basicAnim->mTicksPerSecond != 0)
            TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond;
        else
            TicksPerSecond = 25.0f;;
        //std::cout<<"found TicksPerSecond in animation: "<< TicksPerSecond<<std::endl;
        
        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float Duration = m_pScene->mAnimations[0]->mDuration;
        float animationTime = fmod(TimeInTicks,Duration);
        
        c3ga::TRSversor IdentityVersor = c3ga::matrix4x4ToVersorPS(value_ptr(Identity), 1);
        
        readNodeHierarchyCGA_Gaigen(animationTime, rootNode, IdentityVersor);
        
        transforms.resize(m_NumBones);
        
        for (uint i = 0 ; i < m_NumBones ; i++)
        {
            c3ga::TRSversor versor = m_BoneInfo[i].FinalTransformationGaigen;
            c3ga::TRSversor iVersor = inverse(versor);
            
            transforms[i] = makeMat4(_flatPoint(versor * c3ga::e1ni * iVersor),
                                     _flatPoint(versor * c3ga::e2ni * iVersor),
                                     _flatPoint(versor * c3ga::e3ni * iVersor),
                                     _flatPoint(versor * c3ga::noni * iVersor));
        }
        
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;
}

void RigMesh::readNodeHierarchyCGA_Gaigen(float animationTime, const glGA::aiNodeglGA* pNode, const c3ga::TRSversor& parentTransform)
{
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    c3ga::TRSversor NodeTransformation = pNode->versorGaigen;
    
    glGA::aiNodeAnimglGA* pNodeAnim = findNodeAnim(aiAnim, NodeName);
    
    if (pNodeAnim) {
        high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        NodeTransformation = calcInterpolatedRotationTranslationCGA_GAIGEN(animationTime, pNodeAnim);
        
        high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        duration<double, std::milli> time_span = duration_cast<duration<double, std::milli> >(end - start);
        total += time_span.count();
        i++;
        avgTime = total/i;
    }
    
    c3ga::TRSversor GlobalTransformation = parentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformationGaigen = m_GlobalInverseTransformGaigen * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffsetGaigen;
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchyCGA_Gaigen(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}
c3ga::TRSversor RigMesh::calcInterpolatedRotationTranslationCGA_GAIGEN(float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim)
{
    c3ga::TRSversor finalRotTrans;
    
    if (pNodeAnim->assimpAiNode->mNumPositionKeys == 1 && pNodeAnim->assimpAiNode->mNumRotationKeys == 1)
    {
        aiVector3D StartPos = pNodeAnim->assimpAiNode->mPositionKeys[0].mValue;
        c3ga::rotor StartQuat = pNodeAnim->rotorGaigen[0];
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        finalRotTrans = trRotGaigen(StartPos2, StartQuat);
    }
    else if (pNodeAnim->assimpAiNode->mNumPositionKeys == 1 && pNodeAnim->assimpAiNode->mNumRotationKeys > 1)
    {
        c3ga::rotor StartQuat, EndQuat;
        float Factor = getStartEndRotorGaigen(StartQuat, EndQuat, animationTime, pNodeAnim);
        
        aiVector3D StartPos = pNodeAnim->assimpAiNode->mPositionKeys[0].mValue;
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        
        finalRotTrans = interpolateCGA1Trs2Rot(StartPos2, StartQuat, EndQuat, Factor, m_GAfactorDenominator);
    }
    else if (pNodeAnim->assimpAiNode->mNumPositionKeys > 1 && pNodeAnim->assimpAiNode->mNumRotationKeys > 1)
    {
        c3ga::rotor StartQuat, EndQuat;
        getStartEndRotorGaigen(StartQuat, EndQuat, animationTime, pNodeAnim);        aiVector3D StartPos, EndPos;
        
        float Factor = getStartEndPos(StartPos, EndPos, animationTime, pNodeAnim->assimpAiNode);
        
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        glm::vec3 EndPos2(EndPos.x, EndPos.y, EndPos.z);
        
        finalRotTrans = interpolateCGA(StartPos2, EndPos2, StartQuat, EndQuat, Factor, m_GAfactorDenominator);
    }
    else // >1 ==1
    {
        aiVector3D StartPos, EndPos;
        float Factor = getStartEndPos(StartPos, EndPos, animationTime, pNodeAnim->assimpAiNode);
        c3ga::rotor StartQuat = pNodeAnim->rotorGaigen[0];
        
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        glm::vec3 EndPos2(EndPos.x, EndPos.y, EndPos.z);
        
        finalRotTrans = interpolateCGA2Trs1Rot(StartPos2, EndPos2, StartQuat, Factor, m_GAfactorDenominator);
    }
    
    return finalRotTrans;
}
float RigMesh::getStartEndRotorGaigen(c3ga::rotor& Start, c3ga::rotor& End, float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim)
{
    uint RotationIndex = findRotation(animationTime, pNodeAnim->assimpAiNode);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->assimpAiNode->mNumRotationKeys);
    
    Start = pNodeAnim->rotorGaigen[RotationIndex];
    End = pNodeAnim->rotorGaigen[NextRotationIndex];
    
    float DeltaTime = pNodeAnim->assimpAiNode->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->assimpAiNode->mRotationKeys[RotationIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->assimpAiNode->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    
    return Factor;
}
#endif

#if INTERPOLATION==GA_VERSOR || INTERPOLATION==CGA_VERSOR
float RigMesh::getStartEndQuat(vsr::cga::Rot& Start, vsr::cga::Rot& End, float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim)
{
    uint RotationIndex = findRotation(animationTime, pNodeAnim->assimpAiNode);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->assimpAiNode->mNumRotationKeys);
    
    Start = pNodeAnim->rotorVersor[RotationIndex];
    End = pNodeAnim->rotorVersor[NextRotationIndex];
    
    float DeltaTime = pNodeAnim->assimpAiNode->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->assimpAiNode->mRotationKeys[RotationIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->assimpAiNode->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    
    return Factor;
}
#endif

#if INTERPOLATION==GA_VERSOR
void RigMesh::boneTransformGA_Versor(float TimeInSeconds, std::vector<glm::mat4>& transforms)
{
    std::vector<RotorDef> transformsVersors;
    //    std::cout<<"@@@ entered RigMesh::boneTransform()"<< std::endl;
    glm::mat4 Identity(1.0);
    
    if (m_pScene->HasAnimations()) {
        
        //std::cout<<"found "<<m_pScene->mNumAnimations<<" animations in loaded Scene!"<<std::endl;
        assert(m_pScene!=NULL);
        assert( (m_pScene->mAnimations[0]) !=NULL);
        const aiAnimation* basicAnim = m_pScene->mAnimations[0];
        //std::cout<<"found basicAnim with name: "<< basicAnim->mName.C_Str()<<std::endl;
        
        assert(basicAnim !=NULL);
        //float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
        float TicksPerSecond;
        if ( basicAnim->mTicksPerSecond != 0)
            TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond;
        else
            TicksPerSecond = 25.0f;;
        //std::cout<<"found TicksPerSecond in animation: "<< TicksPerSecond<<std::endl;
        
        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float Duration = m_pScene->mAnimations[0]->mDuration;
        float animationTime = fmod(TimeInTicks,Duration);
        
        readNodeHierarchyGA_Versor(animationTime, m_pScene->mRootNode, Identity);
        
        transforms.resize(m_NumBones);
        
        for (uint i = 0 ; i < m_NumBones ; i++)
        {
            transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
        
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;
}

void RigMesh::readNodeHierarchyGA_Versor(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
    //std::cout<<"@@@GPTEMP: inside readNodeHierarchy()"<<std::endl;
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    glm::mat4 NodeTransformation;
    aiMat2glmMat( &(pNode->mTransformation), NodeTransformation);
    
    //std::cout<<"@@@GPTEMP: inside readNodeHierarchy() before findNodeAnim()"<<std::endl;
    const glGA::aiNodeAnimglGA* pNodeAnim = findNodeAnim(aiAnim, NodeName);
    
    if (pNodeAnim)
    {
        using namespace std::chrono;
        
        high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        glm::mat4 rotationMatrix1 = calcInterpolatedRotationGAVersor(animationTime, pNodeAnim);
        
        aiVector3D translation1;
        calcInterpolatedPosition(translation1, animationTime, pNodeAnim->assimpAiNode);
        glm::mat4 translationMatrix1 = glm::translate(glm::mat4(1.0), glm::vec3(translation1.x, translation1.y, translation1.z));
        
        NodeTransformation = translationMatrix1 * rotationMatrix1;
        
        high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        duration<double, std::milli> time_span = duration_cast<duration<double, std::milli> >(end - start);
        total += time_span.count();
        i++;
        avgTime = total/i;
    }
    
    glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchyGA_Versor(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

glm::mat4 RigMesh::calcInterpolatedRotationGAVersor(float animationTime, const glGA::aiNodeAnimglGA* nodeAnim)
{
    // we need at least two values to interpolate...
    if (nodeAnim->assimpAiNode->mNumRotationKeys == 1)
    {
        aiQuaternion out = nodeAnim->assimpAiNode->mRotationKeys[0].mValue;
        glm::quat glmQuat(out.w, out.x, out.y, out.z);
        
        return glm::mat4_cast(glmQuat);
    }
    
    unsigned int rotationIndex = findRotation(animationTime, nodeAnim->assimpAiNode);
    unsigned int nextRotationIndex = (rotationIndex + 1);
    assert(nextRotationIndex < nodeAnim->assimpAiNode->mNumRotationKeys);
    float deltaTime = (float)(nodeAnim->assimpAiNode->mRotationKeys[nextRotationIndex].mTime - nodeAnim->assimpAiNode->mRotationKeys[rotationIndex].mTime);
    float factor = (animationTime - (float)nodeAnim->assimpAiNode->mRotationKeys[rotationIndex].mTime) / deltaTime;
    //assert(factor >= 0.0f && factor <= 1.0f);
    const vsr::cga::Rot& startRotationQ = nodeAnim->rotorVersor[rotationIndex];
    const vsr::cga::Rot& endRotationQ   = nodeAnim->rotorVersor[nextRotationIndex];
    
    return interpolateGAVersor(startRotationQ, endRotationQ, factor,m_GAfactorDenominator);
}
#endif


#if INTERPOLATION==CGA_VERSOR
void RigMesh::readNodeHierarchyCGA_Versor(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
    //std::cout<<"@@@GPTEMP: inside readNodeHierarchy()"<<std::endl;
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    glm::mat4 NodeTransformation;
    aiMat2glmMat( &(pNode->mTransformation), NodeTransformation);
    
    const glGA::aiNodeAnimglGA* pNodeAnim = findNodeAnim(aiAnim, NodeName);
    
    if (pNodeAnim)
    {
        vsr::cga::Mot node;
        using namespace std::chrono;
        
        high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        node = calcInterpolatedRotationTranslationCGA_Versor(animationTime, pNodeAnim);
        
        high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        duration<double, std::milli> time_span = duration_cast<duration<double, std::milli> >(end - start);
        total += time_span.count();
        i++;
        avgTime = total/i;
        
        NodeTransformation = getMat4Versor(node);
    }
    
    glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchyCGA_Versor(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

vsr::cga::Mot RigMesh::calcInterpolatedRotationTranslationCGA_Versor(float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim)
{
    vsr::cga::Mot finalRotTrans;
    
    if (pNodeAnim->assimpAiNode->mNumPositionKeys == 1 && pNodeAnim->assimpAiNode->mNumRotationKeys == 1)
    {
        aiVector3D StartPos = pNodeAnim->assimpAiNode->mPositionKeys[0].mValue;
        vsr::cga::Rot StartQuat = pNodeAnim->rotorVersor[0];
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        
        finalRotTrans = interpolateCGAVersor11(StartPos2, StartQuat);
    }
    else if (pNodeAnim->assimpAiNode->mNumPositionKeys == 1 && pNodeAnim->assimpAiNode->mNumRotationKeys > 1)
    {
        vsr::cga::Rot StartQuat, EndQuat;
        float Factor = getStartEndQuat(StartQuat, EndQuat, animationTime, pNodeAnim);
        aiVector3D StartPos = pNodeAnim->assimpAiNode->mPositionKeys[0].mValue;
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        
        finalRotTrans = interpolateCGAVersor1Pos2Rot(StartPos2, StartQuat, EndQuat, Factor, m_GAfactorDenominator);
    }
    else if (pNodeAnim->assimpAiNode->mNumPositionKeys > 1 && pNodeAnim->assimpAiNode->mNumRotationKeys > 1)
    {
        vsr::cga::Rot StartQuat, EndQuat;
        getStartEndQuat(StartQuat, EndQuat, animationTime, pNodeAnim);
        aiVector3D StartPos, EndPos;
        float Factor = getStartEndPos(StartPos, EndPos, animationTime, pNodeAnim->assimpAiNode);
        
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        glm::vec3 EndPos2(EndPos.x, EndPos.y, EndPos.z);
        
        finalRotTrans = interpolateCGAVersor(StartPos2, EndPos2, StartQuat, EndQuat, Factor, m_GAfactorDenominator);
    }
    else // >1 ==1
    {
        aiVector3D StartPos, EndPos;
        float Factor = getStartEndPos(StartPos, EndPos, animationTime, pNodeAnim->assimpAiNode);
        vsr::cga::Rot  StartQuat = pNodeAnim->rotorVersor[0];
        
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        glm::vec3 EndPos2(EndPos.x, EndPos.y, EndPos.z);
        
        finalRotTrans = interpolateCGAVersor1Rot(StartPos2, EndPos2, StartQuat, Factor, m_GAfactorDenominator);
    }
    
    return finalRotTrans;
}
void RigMesh::boneTransformCGA_Versor(float TimeInSeconds, std::vector<glm::mat4>& transforms)
{
    std::vector<RotorDef> transformsVersors;
    //    std::cout<<"@@@ entered RigMesh::boneTransform()"<< std::endl;
    glm::mat4 Identity(1.0);
    
    if (m_pScene->HasAnimations()) {
        
        //std::cout<<"found "<<m_pScene->mNumAnimations<<" animations in loaded Scene!"<<std::endl;
        assert(m_pScene!=NULL);
        assert( (m_pScene->mAnimations[0]) !=NULL);
        const aiAnimation* basicAnim = m_pScene->mAnimations[0];
        //std::cout<<"found basicAnim with name: "<< basicAnim->mName.C_Str()<<std::endl;
        
        assert(basicAnim !=NULL);
        //float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
        float TicksPerSecond;
        if ( basicAnim->mTicksPerSecond != 0)
            TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond;
        else
            TicksPerSecond = 25.0f;;
        //std::cout<<"found TicksPerSecond in animation: "<< TicksPerSecond<<std::endl;
        
        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float Duration = m_pScene->mAnimations[0]->mDuration;
        float animationTime = fmod(TimeInTicks,Duration);
        
        readNodeHierarchyCGA_Versor(animationTime, m_pScene->mRootNode, Identity);
        
        transforms.resize(m_NumBones);
        
        for (uint i = 0 ; i < m_NumBones ; i++)
        {
            transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
        
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;
}
#endif

#if INTERPOLATION==GA_GAALOP || INTERPOLATION==CGA_GAALOP
float RigMesh::calcInterpolatedScalingGA(float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        aiVector3D Out = pNodeAnim->mScalingKeys[0].mValue;
        float Dilator1 = (1.0 - Out.y) * (1.0 + Out.y) / ((1.0 + Out.y) * (1.0 + Out.y));
        return Dilator1;
    }
    
    uint ScalingIndex = findScaling(animationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    // GPTODO CHECK THIS ASSERT
    //    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    
    float d1=Start.x;
    float d2=End.y;
    
    float Dilator1 = (1.0 - d1) * (1.0 + d1) / ((1.0 + d1) * (1.0 + d1)); // einf ^ e0
    float Dilator2 = (1.0 - d2) * (1.0 + d2) / ((1.0 + d2) * (1.0 + d2)); // einf ^ e0
    float finalD = Dilator1 + (Dilator2 - Dilator1) * Factor; // einf ^ e0
    
    
    return finalD;
}

void RigMesh::calcInterpolatedRotationGA_GAALOP(aiQuaternion& out, float animationTime, const glGA::aiNodeAnimglGA* nodeAnim)
{
    // we need at least two values to interpolate...
    if (nodeAnim->assimpAiNode->mNumRotationKeys == 1)
    {
        out = nodeAnim->assimpAiNode->mRotationKeys[0].mValue;
        return;
    }
    
    unsigned int rotationIndex = findRotation(animationTime, nodeAnim->assimpAiNode);
    unsigned int nextRotationIndex = (rotationIndex + 1);
    assert(nextRotationIndex < nodeAnim->assimpAiNode->mNumRotationKeys);
    float deltaTime = (float)(nodeAnim->assimpAiNode->mRotationKeys[nextRotationIndex].mTime - nodeAnim->assimpAiNode->mRotationKeys[rotationIndex].mTime);
    float factor = (animationTime - (float)nodeAnim->assimpAiNode->mRotationKeys[rotationIndex].mTime) / deltaTime;
    //assert(factor >= 0.0f && factor <= 1.0f);
    const aiRotor& startRotationQ = nodeAnim->rotorGA_GAALOP[rotationIndex];
    const aiRotor& endRotationQ   = nodeAnim->rotorGA_GAALOP[nextRotationIndex];
    
    interpolateGA_GAALOP(out, startRotationQ, endRotationQ, factor);
    out = out.Normalize();
}
#endif

#if INTERPOLATION==GA_GAALOP
aiRotor RigMesh::calculateGA_GAALOP_Rot(float srcAngle, float srcX, float srcY, float srcZ)
{
    float R[8];
    R[0] = ((0.0026041667442768812 * pow(srcAngle,4.0) - 0.125 * srcAngle * srcAngle + 1.0) * pow(srcZ,6.0) + ((0.0078125 * pow(srcAngle,4.0) - 0.375 * srcAngle * srcAngle + 3.0) * srcY * srcY + (0.0078125 * pow(srcAngle,4.0) - 0.375 * srcAngle * srcAngle + 3.0) * srcX * srcX) * pow(srcZ,4.0) + ((0.0078125 * pow(srcAngle,4.0) - 0.375 * srcAngle * srcAngle + 3.0) * pow(srcY,4.0) + (0.015625 * pow(srcAngle,4.0) - 0.75 * srcAngle * srcAngle + 6.0) * srcX * srcX * srcY * srcY + (0.0078125 * pow(srcAngle,4.0) - 0.375 * srcAngle * srcAngle + 3.0) * pow(srcX,4.0)) * srcZ * srcZ + (0.0026041667442768812 * pow(srcAngle,4.0) - 0.125 * srcAngle * srcAngle + 1.0) * pow(srcY,6.0) + (0.0078125 * pow(srcAngle,4.0) - 0.375 * srcAngle * srcAngle + 3.0) * srcX * srcX * pow(srcY,4.0) + (0.0078125 * pow(srcAngle,4.0) - 0.375 * srcAngle * srcAngle + 3.0) * pow(srcX,4.0) * srcY * srcY + (0.0026041667442768812 * pow(srcAngle,4.0) - 0.125 * srcAngle * srcAngle + 1.0) * pow(srcX,6.0)) / (pow(srcZ,6.0) + (3.0 * srcY * srcY + 3.0 * srcX * srcX) * pow(srcZ,4.0) + (3.0 * pow(srcY,4.0) + 6.0 * srcX * srcX * srcY * srcY + 3.0 * pow(srcX,4.0)) * srcZ * srcZ + pow(srcY,6.0) + 3.0 * srcX * srcX * pow(srcY,4.0) + 3.0 * pow(srcX,4.0) * srcY * srcY + pow(srcX,6.0)); // 1.0
    R[4] = (-((sqrtf(srcZ * srcZ + srcY * srcY + srcX * srcX) * ((0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcZ * srcZ * srcZ + ((0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcY * srcY + (0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcX * srcX) * srcZ)) / (pow(srcZ,4.0) + (2.0 * srcY * srcY + 2.0 * srcX * srcX) * srcZ * srcZ + pow(srcY,4.0) + 2.0 * srcX * srcX * srcY * srcY + pow(srcX,4.0)))); // e1 ^ e2
    R[5] = (sqrtf(srcZ * srcZ + srcY * srcY + srcX * srcX) * ((0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcY * srcZ * srcZ + (0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcY * srcY * srcY + (0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcX * srcX * srcY)) / (pow(srcZ,4.0) + (2.0 * srcY * srcY + 2.0 * srcX * srcX) * srcZ * srcZ + pow(srcY,4.0) + 2.0 * srcX * srcX * srcY * srcY + pow(srcX,4.0)); // e1 ^ e3
    R[6] = (-((sqrtf(srcZ * srcZ + srcY * srcY + srcX * srcX) * ((0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcX * srcZ * srcZ + (0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcX * srcY * srcY + (0.02083333395421505 * srcAngle * srcAngle * srcAngle - srcAngle / 2.0) * srcX * srcX * srcX)) / (pow(srcZ,4.0) + (2.0 * srcY * srcY + 2.0 * srcX * srcX) * srcZ * srcZ + pow(srcY,4.0) + 2.0 * srcX * srcX * srcY * srcY + pow(srcX,4.0)))); // e2 ^ e3
    
    return aiRotor(R[0], R[4], R[5], R[6]);
}

void RigMesh::boneTransformGA_GAALOP(float TimeInSeconds, std::vector<glm::mat4>& transforms)
{
    std::vector<RotorDef> transformsVersors;
    //    std::cout<<"@@@ entered RigMesh::boneTransform()"<< std::endl;
    glm::mat4 Identity(1.0);
    
    if (m_pScene->HasAnimations()) {
        
        //std::cout<<"found "<<m_pScene->mNumAnimations<<" animations in loaded Scene!"<<std::endl;
        assert(m_pScene!=NULL);
        assert( (m_pScene->mAnimations[0]) !=NULL);
        const aiAnimation* basicAnim = m_pScene->mAnimations[0];
        //std::cout<<"found basicAnim with name: "<< basicAnim->mName.C_Str()<<std::endl;
        
        assert(basicAnim !=NULL);
        //float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
        float TicksPerSecond;
        if ( basicAnim->mTicksPerSecond != 0)
            TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond;
        else
            TicksPerSecond = 25.0f;;
        //std::cout<<"found TicksPerSecond in animation: "<< TicksPerSecond<<std::endl;
        
        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float Duration = m_pScene->mAnimations[0]->mDuration;
        float animationTime = fmod(TimeInTicks,Duration);
        
        readNodeHierarchyGA_GAALOP(animationTime, m_pScene->mRootNode, Identity);
        
        transforms.resize(m_NumBones);
        
        for (uint i = 0 ; i < m_NumBones ; i++)
        {
            transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
        
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;
}

void RigMesh::readNodeHierarchyGA_GAALOP(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];
    
    glm::mat4 NodeTransformation;
    aiMat2glmMat( &(pNode->mTransformation), NodeTransformation);
    
    const glGA::aiNodeAnimglGA* pNodeAnim = findNodeAnim(aiAnim, NodeName);
    
    if (pNodeAnim) {
        high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        aiQuaternion rotationQuat1;
        calcInterpolatedRotationGA_GAALOP(rotationQuat1, animationTime, pNodeAnim);
        aiMatrix4x4 assimpRotationMatrix1(rotationQuat1.GetMatrix());
        glm::mat4 rotationMatrix1;
        aiMat2glmMat(&assimpRotationMatrix1, rotationMatrix1);
        
        aiVector3D translation1;
        calcInterpolatedPosition(translation1, animationTime, pNodeAnim->assimpAiNode);
        glm::mat4 translationMatrix1 = glm::translate(glm::mat4(1.0), glm::vec3(translation1.x, translation1.y, translation1.z));
        
        NodeTransformation = translationMatrix1 * rotationMatrix1;
        
        high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        duration<double, std::milli> time_span = duration_cast<duration<double, std::milli> >(end - start);
        total += time_span.count();
        i++;
        avgTime = total/i;
    }
    glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
        
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchyGA_GAALOP(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}
#endif

#if INTERPOLATION==CGA_GAALOP
RotorDef RigMesh::calcInterpolatedRotationTranslationCGA_GAALOP(float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim)
{
    RotorDef finalRotTrans;
    
    if (pNodeAnim->assimpAiNode->mNumPositionKeys == 1 && pNodeAnim->assimpAiNode->mNumRotationKeys == 1)
    {
        aiVector3D StartPos = pNodeAnim->assimpAiNode->mPositionKeys[0].mValue;
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        
        aiRotor StartQuat = pNodeAnim->rotor[0];
        finalRotTrans = TrRotToVersor(StartPos2, StartQuat);
    }
    else if (pNodeAnim->assimpAiNode->mNumPositionKeys == 1 && pNodeAnim->assimpAiNode->mNumRotationKeys > 1)
    {
        aiVector3D StartPos = pNodeAnim->assimpAiNode->mPositionKeys[0].mValue;
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        
        aiRotor StartQuat1, EndQuat1;
        float Factor = getStartEndQuat(StartQuat1, EndQuat1, animationTime, pNodeAnim);
        finalRotTrans = interpolateCGA_GAALOP(StartPos2, StartPos2, StartQuat1, EndQuat1, Factor);
    }
    else if (pNodeAnim->assimpAiNode->mNumPositionKeys > 1 && pNodeAnim->assimpAiNode->mNumRotationKeys > 1)
    {
        aiRotor StartQuat, EndQuat;
        getStartEndQuat(StartQuat, EndQuat, animationTime, pNodeAnim);
        aiVector3D StartPos, EndPos;
        float Factor = getStartEndPos(StartPos, EndPos, animationTime, pNodeAnim->assimpAiNode);
        
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        glm::vec3 EndPos2(EndPos.x, EndPos.y, EndPos.z);
        
        finalRotTrans = interpolateCGA_GAALOP(StartPos2, EndPos2, StartQuat, EndQuat, Factor);
    }
    else // >1 ==1
    {
        aiVector3D StartPos, EndPos;
        float Factor = getStartEndPos(StartPos, EndPos, animationTime, pNodeAnim->assimpAiNode);
        aiRotor StartQuat = pNodeAnim->rotor[0];
        
        glm::vec3 StartPos2(StartPos.x, StartPos.y, StartPos.z);
        glm::vec3 EndPos2(EndPos.x, EndPos.y, EndPos.z);
        finalRotTrans = rotationInterpolateTranslationCGA_GAALOP(StartPos2, EndPos2, StartQuat, Factor);
    }
    
    return finalRotTrans;
}

std::vector<RotorDefScale> RigMesh::boneTransformCGA_GAALOP(float TimeInSeconds)
{
    std::vector<RotorDefScale> transformsVersors;
    //std::cout<<"@@@ entered RigMesh::boneTransform()"<< std::endl;
    glm::mat4 Identity(1.0);
    
    if (m_pScene->HasAnimations()) {
        
        //std::cout<<"found "<<m_pScene->mNumAnimations<<" animations in loaded Scene!"<<std::endl;
        assert(m_pScene!=NULL);
        assert( (m_pScene->mAnimations[0]) !=NULL);
        const aiAnimation* basicAnim = m_pScene->mAnimations[0];
        //std::cout<<"found basicAnim with name: "<< basicAnim->mName.C_Str()<<std::endl;
        
        assert(basicAnim !=NULL);
        //float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
        float TicksPerSecond;
        if ( basicAnim->mTicksPerSecond != 0)
            TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond;
        else
            TicksPerSecond = 25.0f;;
        //std::cout<<"found TicksPerSecond in animation: "<< TicksPerSecond<<std::endl;
        
        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float Duration = m_pScene->mAnimations[0]->mDuration;
        float animationTime = fmod(TimeInTicks,Duration);
        
        readNodeHierarchyCGA_GAALOP(animationTime, rootNode, IdentityVersor);
        transformsVersors.resize(m_NumBones);
        
        for (uint i = 0 ; i < m_NumBones ; i++)
        {
            transformsVersors[i] = getRotorDef2(m_BoneInfo[i].FinalTransformationVersor);
        }
        
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;
    
    return  transformsVersors;
}

float RigMesh::getStartEndQuat(aiRotor& Start, aiRotor& End, float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim)
{
    uint RotationIndex = findRotation(animationTime, pNodeAnim->assimpAiNode);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->assimpAiNode->mNumRotationKeys);
    
    Start = pNodeAnim->rotor[RotationIndex];
    End = pNodeAnim->rotor[NextRotationIndex];
    
    float DeltaTime = pNodeAnim->assimpAiNode->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->assimpAiNode->mRotationKeys[RotationIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->assimpAiNode->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    
    return Factor;
}

float RigMesh::getStartEndQuat(aiRotor& Start, aiRotor& End, float animationTime, const aiNodeAnim* pNodeAnim)
{
    uint RotationIndex = findRotation(animationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    
    //Start = pNodeAnim->mRotationKeys[RotationIndex].rotor;
    //End = pNodeAnim->mRotationKeys[NextRotationIndex].rotor;
    
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    
    return Factor;
}

void RigMesh::readNodeHierarchyCGA_GAALOP(float animationTime, const glGA::aiNodeglGA* pNode, float* parentTransform)
{
    //std::cout<<"@@@GPTEMP: inside readNodeHierarchy()"<<std::endl;
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    float* NodeTransformation = pNode->rotor;
    
    glGA::aiNodeAnimglGA* pNodeAnim = findNodeAnim(aiAnim, NodeName);
    RotorDef x;
    
    if (pNodeAnim) {
        high_resolution_clock::time_point start = high_resolution_clock::now();
        
        float interpolatedScaling = calcInterpolatedScalingGA(animationTime, pNodeAnim->assimpAiNode);
        
        x = calcInterpolatedRotationTranslationCGA_GAALOP(animationTime, pNodeAnim);
        
        RotorDefScale scale = gpMotorDilator(x, interpolatedScaling);
        
        high_resolution_clock::time_point end = high_resolution_clock::now();
        duration<double, std::milli> time_span = duration_cast<duration<double, std::milli> >(end - start);
        
        total += time_span.count();
        i++;
        avgTime = total/i;
        
        NodeTransformation[0] = scale.scalar; // 1.0
        NodeTransformation[6] = scale.e1e2; // e1 ^ e2
        NodeTransformation[7] = scale.e1e3; // e1 ^ e3
        NodeTransformation[8] = scale.e1einf; // e1 ^ einf
        NodeTransformation[10] = scale.e2e3; // e2 ^ e3
        NodeTransformation[11] = scale.e2einf; // e2 ^ einf
        NodeTransformation[13] = scale.e3einf; // e3 ^ einf
        NodeTransformation[26] = scale.e2e3einf; // e1 ^ (e2 ^ (e3 ^ einf))
        
        NodeTransformation[15] = scale.e15;
        NodeTransformation[28] = scale.e28;
        NodeTransformation[29] = scale.e29;
        NodeTransformation[30] = scale.e30;
    }
    
    float* GlobalTransformation = mulVersors(parentTransform, NodeTransformation);
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        
        m_BoneInfo[BoneIndex].FinalTransformationVersor = mulVersors(mulVersors(m_GlobalInverseTransformVersor, GlobalTransformation), m_BoneInfo[BoneIndex].BoneOffsetVersor);
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchyCGA_GAALOP(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

aiRotor RigMesh::calculate(float angle, float axis1X, float axis1Y, float axis1Z)
{
    float axis[32]; float axiss[32]; float R[32];
    
    axiss[7] = (-axis1Y); // e1 ^ e3
    axis[6] = axis1Z * sqrtf(fabs((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X))))) / sqrtf(fabs(((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X)))) * ((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X)))))); // e1 ^ e2
    axis[7] = axiss[7] * sqrtf(fabs((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X))))) / sqrtf(fabs(((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X)))) * ((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X)))))); // e1 ^ e3
    axis[10] = axis1X * sqrtf(fabs((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X))))) / sqrtf(fabs(((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X)))) * ((-(axis1Z * (-axis1Z))) + (-(axiss[7] * (-axiss[7]))) + (-(axis1X * (-axis1X)))))); // e2 ^ e3
    R[0] = 1.0 + ((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) / 2.0 + ((-(((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[6])) + (-(((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[7])) + (-(((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[10]))) * 0.16666666666666666 + ((-((((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[6] + (-(((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[10])) + ((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[6])) + (-((((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[7] + ((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[10] + (-(((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[7])) + (-((((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[10] + (-(((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[7])) + ((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[10]))) * 0.041666666666666664; // 1.0
    R[6] = (-angle) / 2.0 * axis[6] + ((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) / 2.0 + (((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[6] + (-(((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[10])) + ((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[7]) * 0.16666666666666666 + (((-(((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[6])) + (-(((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[7])) + (-(((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[6] + (-((((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[7] + ((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[10] + (-(((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[10])) + (((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[10] + (-(((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[7])) + ((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[7]) * 0.041666666666666664; // e1 ^ e2
    R[7] = (-angle) / 2.0 * axis[7] + ((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) / 2.0 + (((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[7] + ((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[10] + (-(((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[6]))) * 0.16666666666666666 + (((-(((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[6])) + (-(((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[7])) + (-(((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[7] + (((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[6] + (-(((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[10])) + ((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[10] + (-((((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[10] + (-(((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[7])) + ((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[6]))) * 0.041666666666666664; // e1 ^ e3
    R[10] = (-angle) / 2.0 * axis[10] + ((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) / 2.0 + (((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[10] + (-(((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[7])) + ((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[6]) * 0.16666666666666666 + (((-(((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[6])) + (-(((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[7])) + (-(((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[10] + (-((((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[6] + (-(((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[10] + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[10])) + ((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[7])) + (((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[6])) + (-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[7])) + (-((-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[10]))) * (-angle) / 2.0 * axis[7] + ((-((-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[10])) + (-angle) / 2.0 * axis[10] * (-angle) / 2.0 * axis[7]) * (-angle) / 2.0 * axis[10] + (-(((-((-angle) / 2.0 * axis[6] * (-angle) / 2.0 * axis[7])) + (-angle) / 2.0 * axis[7] * (-angle) / 2.0 * axis[6]) * (-angle) / 2.0 * axis[6]))) * (-angle) / 2.0 * axis[6]) * 0.041666666666666664; // e2 ^ e3
    return aiRotor(R[0], R[6], R[7], R[10]);
}
#endif

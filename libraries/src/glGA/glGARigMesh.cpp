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

using namespace std::chrono;

float RigMesh::getAnimationInterpolationTime()
{
    return avgTime;
}

void RigMesh::VertexBoneData::addBoneData(uint boneID, float weight)
{
    for (uint i=0; i< ARRAY_SIZE_IN_ELEMENTS(IDs); ++i){
        if (Weights[i] == 0.0) {
            IDs[i]  = boneID;
            Weights[i] = weight;
            return;
        }
    }//end for
    
    // should not reach here - means we have more bones than allocated space
    assert(0);
}

RigMesh::RigMesh()
{
    m_NumBones = 0;
    m_pScene = NULL;
    m_Textures.clear();
    m_Textures.resize(0);
    m_GAfactorDenominator = 1.0;
    initialTransformation = mat4(1.0);
}

RigMesh::~RigMesh()
{
    clear();
}

void RigMesh::clear()
{
    for (unsigned int i=0; i<m_Textures.size(); ++i) {
        SAFE_DELETE(m_Textures[i]);
    }
    m_Textures.clear();
    m_Textures.resize(0);
    m_pScene = NULL;
    //delete m_pScene;
    m_Importer = NULL;
    delete m_Importer;
}//end clear()

bool RigMesh::loadRigMesh(const std::string& filename )
{
    //release a previously loaded RigMesh (if it exists)
    clear();
    
    GLExitIfError();
    
    bool Ret = false;
    //create an instance of the importer class, very important to create it on heap and NOT on stack, so that it persists after execution leaves this class and might be accessed later, after load is complete
    Assimp::Importer* importer = new Assimp::Importer();;
    
    Assimp::DefaultLogger::create( ASSIMP_DEFAULT_LOG_NAME,
                                  Assimp::Logger::VERBOSE,
                                  aiDefaultLogStream_DEBUGGER|aiDefaultLogStream_FILE,
                                  NULL );
    
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example:
    // aiProcess_CalcTangentSpace       |
    // aiProcess_Triangulate            |
    // aiProcess_JoinIdenticalVertices  |
    // aiProcess_SortByPType |
    // aiProcess_FlipUVs
    /*
     m_pScene = importer->ReadFile( filename.c_str(),
     aiProcess_Triangulate   | aiProcess_GenSmoothNormals );
     */
    ///*
    m_pScene = importer->ReadFile( filename.c_str(),
                                  aiProcess_Triangulate  | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_SortByPType | aiProcess_LimitBoneWeights | aiProcess_SplitByBoneCount | aiProcess_JoinIdenticalVertices | aiProcess_FindInvalidData | aiProcess_FindDegenerates | aiProcess_RemoveRedundantMaterials | aiProcess_ImproveCacheLocality | aiProcess_SplitLargeMeshes |  aiProcess_ValidateDataStructure);
    //*/
    if (m_pScene) {
        //m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
        aiMat2glmMat(&(m_pScene->mRootNode->mTransformation),m_GlobalInverseTransform);
        m_GlobalInverseTransform=glm::inverse(m_GlobalInverseTransform);
        Ret = initFromScene(m_pScene, filename);
        
#if INTERPOLATION==GA_GAALOP || INTERPOLATION==CGA_GAALOP || INTERPOLATION==GA_GAIGEN || INTERPOLATION==CGA_GAIGEN
        c3ga::TRSversor v = c3ga::matrix4x4ToVersorPS(value_ptr(m_GlobalInverseTransform), 1);
#endif

#if INTERPOLATION==GA_GAALOP || INTERPOLATION==CGA_GAALOP
        m_GlobalInverseTransformVersor = new float[32];
        
        
        
        for (int i=0; i<32; i++)
            m_GlobalInverseTransformVersor[i] = 0;
        
        m_GlobalInverseTransformVersor[0] = v.m_c[0]; // 1.0
        m_GlobalInverseTransformVersor[6] = v.m_c[1]; // e1 ^ e2
        m_GlobalInverseTransformVersor[7] =  v.m_c[2]; // e1 ^ e3
        m_GlobalInverseTransformVersor[8] = v.m_c[4]; // e1 ^ einf
        m_GlobalInverseTransformVersor[10] =v.m_c[3]; // e2 ^ e3
        m_GlobalInverseTransformVersor[11] = v.m_c[5]; // e2 ^ einf
        m_GlobalInverseTransformVersor[13] = v.m_c[6]; // e3 ^ einf
        m_GlobalInverseTransformVersor[26] = v.m_c[11];  // e1 ^ (e2 ^ (e3 ^ einf))
#endif
        
#if INTERPOLATION==GA_GAIGEN || INTERPOLATION==CGA_GAIGEN
        m_GlobalInverseTransformGaigen = v;
#endif
        
        glm::quat x = glm::quat(m_GlobalInverseTransform);
        glm::vec3 translation = glm::vec3(m_GlobalInverseTransform[3][0], m_GlobalInverseTransform[3][1], m_GlobalInverseTransform[3][2]);
        m_GlobalInverseTransformDQ = glm::dualquat(x, translation);
        
        rootNode = copyAiNode(m_pScene->mRootNode, NULL);
        copyAnimationData();
        QuatToRotors(m_pScene->mRootNode);
    }
    else
    {
        std::cout<<"Error parsing "<< filename.c_str() <<": "<<importer->GetErrorString() <<std::endl;
    }
    
    return Ret;
}//end loadRigMesh()

void RigMesh::copyAnimationData()
{
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];
    aiAnim = new glGA::aiAnimationglGA();
    aiAnim->mNumChannels =pAnimation->mNumChannels;
    cout << aiAnim->mNumChannels;
    aiAnim->mChannels = new glGA::aiNodeAnimglGA*[aiAnim->mNumChannels];
    
    for (int i=0; i<aiAnim->mNumChannels; i++)
    {
        aiAnim->mChannels[i] = new glGA::aiNodeAnimglGA();
        aiAnim->mChannels[i]->assimpAiNode=new aiNodeAnim();
        aiAnim->mChannels[i]->assimpAiNode=pAnimation->mChannels[i];
    }
}

void RigMesh::QuatToRotors(aiNode* pNode)
{
    assert(pNode!=NULL);
    
    std::string NodeName(pNode->mName.data);
    
    glGA::aiNodeAnimglGA* pNodeAnim = findNodeAnim(aiAnim, NodeName);
    if (pNodeAnim)
    {
#if INTERPOLATION==GA_GAALOP
        pNodeAnim->rotorGA_GAALOP = new aiRotor[pNodeAnim->assimpAiNode->mNumRotationKeys];
#endif
        
#if INTERPOLATION==CGA_GAALOP
        pNodeAnim->rotor = new aiRotor[pNodeAnim->assimpAiNode->mNumRotationKeys];
#endif
      
#if INTERPOLATION==GA_GAIGEN
        pNodeAnim->rotorGaigen2 = new e3ga::rotor[pNodeAnim->assimpAiNode->mNumRotationKeys];
#endif
        
#if INTERPOLATION==CGA_GAIGEN
        pNodeAnim->rotorGaigen = new c3ga::rotor[pNodeAnim->assimpAiNode->mNumRotationKeys];
#endif

#if INTERPOLATION==GA_VERSOR || INTERPOLATION==CGA_VERSOR
        pNodeAnim->rotorVersor = new vsr::cga::Rot[pNodeAnim->assimpAiNode->mNumRotationKeys];
#endif

        for (int i = 0 ; i < pNodeAnim->assimpAiNode->mNumRotationKeys ; i++)
        {
            aiQuaternion quat = pNodeAnim->assimpAiNode->mRotationKeys[i].mValue;
            glm::quat glmQuat(quat.w, quat.x, quat.y, quat.z);
            
            glmQuat = glm::normalize(glmQuat);
            float angle = glm::angle(glmQuat);
            glm::vec3 axis = glm::axis(glmQuat);
            
#if INTERPOLATION==GA_GAALOP
            pNodeAnim->rotorGA_GAALOP[i] = calculateGA_GAALOP_Rot(angle, axis.x, axis.y, axis.z);
#endif
            
#if INTERPOLATION==CGA_GAALOP
            pNodeAnim->rotor[i] = calculate(angle, axis.x, axis.y, axis.z);
#endif
            
#if INTERPOLATION==CGA_GAIGEN
            c3ga::mv uC = c3ga::unit_e(axis.x*c3ga::e1+axis.y*c3ga::e2+axis.z*c3ga::e3);
            pNodeAnim->rotorGaigen[i] = _rotor(c3ga::exp(angle/2.0 * (-c3ga::I3 * (uC) )));;
#endif
            
#if INTERPOLATION==GA_GAIGEN
            e3ga::mv u = e3ga::unit_e(axis.x*e3ga::e1+axis.y*e3ga::e2+axis.z*e3ga::e3);
            pNodeAnim->rotorGaigen2[i] = _rotor( e3ga::exp( _bivector(angle/2.0 * (-e3ga::I3 * (u) )) ));
#endif
            
#if INTERPOLATION==GA_VERSOR || INTERPOLATION==CGA_VERSOR
            vsr::cga::Vec srcAxis(axis.x, axis.y, axis.z);
            vsr::cga::Line lin1 = vsr::cga::Vec(0,0,0).null() ^ srcAxis.null() ^ vsr::cga::Inf(1);
            
            vsr::cga::Biv dll = lin1.dual()/lin1.dual().norm();
            
            pNodeAnim->rotorVersor[i] = vsr::cga::Gen::rot(dll*angle/2.0);
            #endif
        }
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++)
    {
        QuatToRotors(pNode->mChildren[i]);
    }
}


glGA::aiNodeglGA* RigMesh::copyAiNode(aiNode* node, glGA::aiNodeglGA* parent)
{
    glGA::aiNodeglGA* newRoot;
    if (node!=NULL)
    {
        newRoot = new glGA::aiNodeglGA();
        newRoot->mName = aiString(node->mName);
        std::string NodeName(newRoot->mName.data);
        newRoot->mParent=parent;
        newRoot->mNumChildren = node->mNumChildren;
        newRoot->mChildren=NULL;
        newRoot->mChildren = new glGA::aiNodeglGA*[node->mNumChildren];
        
        glm::mat4 NodeTransformation;
        aiMat2glmMat( &(node->mTransformation), NodeTransformation);
        
        c3ga::TRSversor v = c3ga::matrix4x4ToVersorPS(value_ptr(NodeTransformation), 1);
        
        #if INTERPOLATION==CGA_GAALOP
        /************ Gaalop *************/
        for (int i=0; i<32; i++)
            newRoot->rotor[i] = 0;
        
        newRoot->rotor[0] = v.m_c[0]; // 1.0
        newRoot->rotor[6] = v.m_c[1]; // e1 ^ e2
        newRoot->rotor[7] =  v.m_c[2]; // e1 ^ e3
        newRoot->rotor[8] = v.m_c[4]; // e1 ^ einf
        newRoot->rotor[10] =v.m_c[3]; // e2 ^ e3
        newRoot->rotor[11] = v.m_c[5]; // e2 ^ einf
        newRoot->rotor[13] = v.m_c[6]; // e3 ^ einf
        newRoot->rotor[26] = v.m_c[11];  // e1 ^ (e2 ^ (e3 ^ einf))
        /*********************************/
#endif
        
        #if INTERPOLATION==GA_GAIGEN || INTERPOLATION==CGA_GAIGEN
        /************ Gaigen ************/
        newRoot->versorGaigen = v;
        /*********************************/
        #endif
        
        for (uint i = 0 ; i < node->mNumChildren ; i++)
        {
            newRoot->mChildren[i] = copyAiNode(node->mChildren[i], newRoot);
        }
    }else return NULL;
    
    return newRoot;
}

bool RigMesh::loadRigMesh(const std::string& filename, glm::vec3 rotation, glm::vec3 position, float scale)
{
    this->rotation = rotation;
    this->position = position;
    this->scale = scale;
    
    float Ret = loadRigMesh(filename);
    
    mat4 tr = translate(mat4(), position);
    mat4 rotateX = rotate(tr, rotation.x, vec3(1.0, 0.0, 0.0));
    mat4 rotateY = rotate(rotateX, rotation.y, vec3(0.0, 1.0, 0.0));
    mat4 rotateZ = rotate(rotateY, rotation.z, vec3(0.0, 0.0, 1.0));
    mat4 initialTransformation = glm::scale(rotateZ, vec3(scale, scale, scale));
    this->initialTransformation = initialTransformation;
    
    return Ret;
}//end loadRigMesh()

/*
 Called by: RigMesh::loadRigMesh
 Creates:
 - one RigMeshEntry for every Mesh in the Scene
 - each RigMeshEntry stores the numIndices, baseVertex, baseIndex and materialIndex
 - one Texture for every material in the Scene
 - aiMesh* paiMesh for every mesh in the scene
 Calls:
 - initRigMesh() for every mesh in the scene
 - initMaterials()
 */

bool RigMesh::initFromScene(const aiScene* pScene, const std::string& filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);
    
    uint numVertices = 0;
    uint numIndices = 0;
    
    // count the num of vertices and indices
    for (unsigned int i=0; i<m_Entries.size(); ++i)
    {
        m_Entries[i].materialIndex  = pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].numIndices     = pScene->mMeshes[i]->mNumFaces *3;
        m_Entries[i].baseVertex     = numVertices;
        m_Entries[i].baseIndex      = numIndices;
        
        numVertices += pScene->mMeshes[i]->mNumVertices;
        numIndices += m_Entries[i].numIndices;
    }
    
    // Reserve space in the vectors for the vertex attributes and indixes
    Positions.reserve(numVertices);
    Normals.reserve(numVertices);
    TexCoords.reserve(numVertices);
    Bones.resize(numVertices);
    Indices.reserve(numIndices);
    
    GLExitIfError();
    
    // initialize the scene meshes one by one
    for (unsigned int i=0; i<m_Entries.size(); ++i) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        this->initRigMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
    }
    
    if (!initMaterials(pScene, filename)) {
        return false;
    }
    // @@@GPTODO: Colors are missing
    
    bool bGLError = GLCheckError();
    return  bGLError;
    
}//end initFromScene()

/*
 Called by: initFromScene()
 Initialises and populates all main vectors:
 - positions, normals, texcoords, indices and bones (VertexBoneData)
 Calls:
 - loadBones (populate for every Mesh and vertex the VertexBoneData with addBoneData() )
 */
void RigMesh::initRigMesh(uint RigMeshIndex,
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

/*
 This function loads the vertex bone information for a single aiMesh object.
 Called from RigMesh::InitRigMesh().
 In addition to populating the VertexBoneData structure this function also updates a map between bone names and bone IDs (a running index managed by this function) and stores the offset matrix in a vector based on the bone ID. Note how the vertex ID is calculated. Since vertex IDs are relevant to a single mesh and we store all meshes in a single vector we add the base vertex ID of the current aiMesh to vertex ID from the mWeights array to get the absolute vertex ID.
 */
void RigMesh::loadBones(uint meshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& bones)
{
    for (uint i=0; i < paiMesh->mNumBones; ++i) {
        uint boneIndex = 0;
        std::string boneName(paiMesh->mBones[i]->mName.data);
        
        if (m_BoneMapping.find(boneName) == m_BoneMapping.end()) {
            //allocate an index for a new bone
            boneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
            glm::mat4 bo;
            aiMat2glmMat( &(paiMesh->mBones[i]->mOffsetMatrix), bo);
            m_BoneInfo[boneIndex].BoneOffset = bo;
            
            c3ga::TRSversor v = c3ga::matrix4x4ToVersorPS(value_ptr(bo), 1);
            
            #if INTERPOLATION==CGA_GAALOP
            m_BoneInfo[boneIndex].BoneOffsetVersor = new float[32];
            for (int i=0; i<32; i++)
                m_BoneInfo[boneIndex].BoneOffsetVersor[i] = 0;
            
            m_BoneInfo[boneIndex].BoneOffsetVersor[0] = v.m_c[0]; // 1.0
            m_BoneInfo[boneIndex].BoneOffsetVersor[6] = v.m_c[1]; // e1 ^ e2
            m_BoneInfo[boneIndex].BoneOffsetVersor[7] =  v.m_c[2]; // e1 ^ e3
            m_BoneInfo[boneIndex].BoneOffsetVersor[8] = v.m_c[4]; // e1 ^ einf
            m_BoneInfo[boneIndex].BoneOffsetVersor[10] =v.m_c[3]; // e2 ^ e3
            m_BoneInfo[boneIndex].BoneOffsetVersor[11] = v.m_c[5]; // e2 ^ einf
            m_BoneInfo[boneIndex].BoneOffsetVersor[13] = v.m_c[6]; // e3 ^ einf
            m_BoneInfo[boneIndex].BoneOffsetVersor[26] = v.m_c[11];  // e1 ^ (e2 ^ (e3 ^ einf))*/
            

            #endif
            #if INTERPOLATION==CGA_GAIGEN
            m_BoneInfo[boneIndex].BoneOffsetGaigen = v;
            #endif
            m_BoneMapping[boneName] = boneIndex;
            
            glm::quat x = glm::quat(bo);
            glm::vec3 translation = glm::vec3(bo[3][0], bo[3][1], bo[3][2]);
            m_BoneInfo[boneIndex].BoneOffsetDQ = glm::dualquat(x, translation);
        } //end if
        else {
            boneIndex = m_BoneMapping[boneName];
        }
        for (uint j=0; j < paiMesh->mBones[i]->mNumWeights; ++j) {
            uint vertexID = m_Entries[meshIndex].baseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
            float weight = paiMesh->mBones[i]->mWeights[j].mWeight;
            bones[vertexID].addBoneData(boneIndex, weight);
        }
    }//end for i
    
    //DEBUG m_BoneMapping:
    for (std::map<std::string, uint>::iterator j=m_BoneMapping.begin(); j!=m_BoneMapping.end(); ++j) {
        std::cout<<"Bone: "<< (*j).first <<" with index: "<<(*j).second<<std::endl;
    }
    
}//end loadBones()

/*
 Called by: initFromScene()
 Calls:
 - initialises all materials in Scene
 - loads all Textures in vector m_Textures
 */
bool RigMesh::initMaterials(const aiScene* pScene, const std::string& filename)
{
    // Extract the directory part from the file name
    std::string::size_type  slashIndex = filename.find_last_of("/");
    std::string             dir;
    
    if (slashIndex == std::string::npos) {
        dir = ".";
    }
    else if (slashIndex == 0)
    {
        dir = "/";
    }
    else
    {
        dir = filename.substr(0, slashIndex);
    }
    
    bool Ret = true;
    
    // initialize the materials
    for (unsigned int i=0; i < pScene->mNumMaterials; ++i)
    {
        const aiMaterial* pMaterial = pScene->mMaterials[i];
        
        m_Textures[i] = NULL;
        
        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) >0)
        {
            aiString path;
            
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS )
            {
                std::string fullPath = dir + "/" + path.data;
                m_Textures[i] = new Texture(GL_TEXTURE_2D, fullPath.c_str());
                
                
                if (!m_Textures[i]->loadTexture()) {
                    std::cout<<"Error Loading texture: "<<fullPath.c_str() <<std::endl;
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else
                {
                    std::cout<<"Loaded texture: "<<fullPath.c_str() <<std::endl;
                }
            }//end if pMaterial->getTexture
        }//end if pMaterial->GetTextureCount
    }//end for()
    
    return Ret;
    
}//end initMaterials()

/*
 Main RigMesh method called from external render/display callback
 Calls for every Mesh in the scene (RigMeshEntry):
 - glDrawElementsBaseVertex()
 - bindTexture for every texture
 */
void RigMesh::render()
{
    for (unsigned int i=0; i < m_Entries.size(); ++i)
    {
        const unsigned int materialIndex = m_Entries[i].materialIndex;
        
        assert(materialIndex < m_Textures.size());
        
        if (m_Textures[materialIndex]) {
            m_Textures[materialIndex]->bindTexture(GL_TEXTURE0);
        }
        
        // render primitives from array data with a per-element offset
        /*
         glDrawElementsBaseVertex behaves identically to glDrawElements except that the ith element transferred by the corresponding draw call will be taken from element indices[i] + basevertex of each enabled array:
         
         void glDrawElementsBaseVertex(	GLenum  	mode,
         GLsizei  	count,
         GLenum  	type,
         GLvoid * 	indices,
         GLint  	basevertex);
         */
        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 m_Entries[i].numIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(unsigned int) * m_Entries[i].baseIndex),
                                 m_Entries[i].baseVertex);
        
    }//end for
    
}//end render()


uint RigMesh::findPosition(float animationTime, const aiNodeAnim* pNodeAnim)
{
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);
}

/*
 This utility method finds the key rotation which is immediately before the animation time. If we have N key rotations the result can be 0 to N-2. The animation time is always contained inside the duration of the channel so the last key (N-1) can never be a valid result.
 */
unsigned int RigMesh::findRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
    assert(nodeAnim->mNumRotationKeys > 0);
    
    for (unsigned int i = 0 ; i < nodeAnim->mNumRotationKeys - 1 ; i++)
    {
        if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }
    assert(0);
    
}


uint RigMesh::findScaling(float animationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);
}


void RigMesh::calcInterpolatedPosition(aiVector3D& Out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
    
    uint PositionIndex = findPosition(animationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    // GPTODO CHECK THIS ASSERT
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

/*
 This method interpolates the rotation quaternion of the specified channel based on the animation time (remember that the channel contains an
 ￼
 array of key quaternions). First we find the index of the key quaternion which is just before the required animation time. We calculate the ratio between the distance from the animation time to the key before it and the distance between that key and the next. We need to interpolate between these two keys using that factor. We use an Assimp code to do the interpolation and normalize the result. The corresponding methods for position and scaling are very similar so they are not quoted here.
 */
void RigMesh::calcInterpolatedRotation(aiQuaternion& Out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
    uint RotationIndex = findRotation(animationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    // GPTODO CHECK THIS ASSERT
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    //const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    //const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    StartRotationQ = StartRotationQ.Normalize();
    EndRotationQ = EndRotationQ.Normalize();
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void RigMesh::calcInterpolatedScaling(aiVector3D& Out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
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
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void RigMesh::readNodeHierarchyQuat(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];
    
    glm::mat4 NodeTransformation;
    aiMat2glmMat( &(pNode->mTransformation), NodeTransformation);
    
    const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, NodeName);
    
    if (pNodeAnim) {
        aiVector3D scaling;
        calcInterpolatedScaling(scaling, animationTime, pNodeAnim);
        glm::mat4 scalingMatrix= glm::scale( glm::mat4(1.0), glm::vec3(scaling.x, scaling.y, scaling.z));
        
        
        high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        aiQuaternion rotationQuat;
        calcInterpolatedRotation(rotationQuat, animationTime, pNodeAnim);
        aiMatrix4x4 assimpRotationMatrix(rotationQuat.GetMatrix());
        glm::mat4 rotationMatrix;
        aiMat2glmMat(&assimpRotationMatrix, rotationMatrix);
        
        aiVector3D translation;
        calcInterpolatedPosition(translation, animationTime, pNodeAnim);
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(translation.x, translation.y, translation.z));
        
        
        
        high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        duration<double, std::milli> time_span = duration_cast<duration<double, std::milli> >(end - start);
        
        NodeTransformation = translationMatrix * rotationMatrix;
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
        readNodeHierarchyQuat(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}
void RigMesh::boneTransform(float TimeInSeconds, std::vector<glm::mat4>& transforms)
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
        //cout << Duration << endl;
        float animationTime = fmod(TimeInTicks,Duration);
        
        readNodeHierarchyQuat(animationTime, m_pScene->mRootNode, Identity);
        
        transforms.resize(m_NumBones);
        
        for (uint i = 0 ; i < m_NumBones ; i++)
        {
            transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
        
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;
}

/*
 This function traverses the node tree and generates the final transformation for each node/bone according to the specified animation time. It is limited in the sense that it assumes that the mesh has only a single animation sequence. If you want to support multiple animations you will need to tell it the animation name and search for it in the m_pScene->mAnimations[] array. The code above is good enough for the demo mesh that we use.
 The node transformation is initialized from the mTransformation member in the node. If the node does not correspond to a bone then that is its final transformation. If it does we overwrite it with a matrix that we generate. This is done as follows: first we search for the node name in the channel array of the animation. Then we interpolate the scaling vector, rotation quaternion and translation vector based on the animation time. We combine them into a single matrix and multiply with the matrix we got as a parameter (named GlobablTransformation). This function is recursive and is called for the root node with the GlobalTransformation param being the identity matrix. Each node recursively calls this function for all of its children and passes its own transformation as GlobalTransformation. Since we start at the top and work our way down, we get the combined transformation chain at every node.
 The m_BoneMapping array maps a node name to the index that we generate and we use that index to as an entry into the m_BoneInfo array where the final transformations are stored. The final transformation is calculated as follows: we start with the node offset matrix which brings the vertices from their local space position into there node space. We then multiple with the combined transformations of all of the nodes parents plus the specific transformation that we calculated for the node according to the animation time.
 */

float RigMesh::getStartEndQuat(aiQuaternion& Start, aiQuaternion& End, float animationTime, const aiNodeAnim* pNodeAnim)
{
    uint RotationIndex = findRotation(animationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    
    Start = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    End = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    
    return Factor;
}

float RigMesh::getStartEndPos(aiVector3D& Start, aiVector3D& End, float animationTime, const aiNodeAnim* pNodeAnim)
{
    uint PositionIndex = findPosition(animationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    
    float DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    
    Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    
    return Factor;
    
}

/*
 Main entry method called from inside external diplay callback.
 Loading of the bone information at the vertex level that we saw earlier is done only once when the mesh is loading during startup with loadBones().
 Here we come to the second part which is calculating the bone transformations that go into the shader every frame. This function above is the entry point to this activity. The caller (e.g. display/render application callback) reports the current time in seconds (which can be a fraction) and provides a vector of matrices which we must update. We find the relative time inside the animation cycle and process the node heirarchy. The result is an array of transformations which is returned to the caller.
 Calls:
 - readNodeHierarchy()
 */

#if INTERPOLATION==DQUAT
void RigMesh::boneTransformDQ(float TimeInSeconds, std::vector<glm::mat4>& transforms)
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
        
        readNodeHierarchyDQ(animationTime, m_pScene->mRootNode, Identity);
        
        transforms.resize(m_NumBones);
        
        for (uint i = 0 ; i < m_NumBones ; i++)
        {
            transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
        
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;
}

void RigMesh::readNodeHierarchyDQ(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
    //std::cout<<"@@@GPTEMP: inside readNodeHierarchy()"<<std::endl;
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];
    
    glm::mat4 NodeTransformation;
    aiMat2glmMat( &(pNode->mTransformation), NodeTransformation);
    
    const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, NodeName);
    
    if (pNodeAnim) {
        aiVector3D scaling;
        calcInterpolatedScaling(scaling, animationTime, pNodeAnim);
        glm::mat4 scalingMatrix= glm::scale( glm::mat4(1.0), glm::vec3(scaling.x, scaling.y, scaling.z));
        
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        NodeTransformation = calcInterpolatedRotationTranslationDQ(animationTime, pNodeAnim);
        
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        duration<double, std::milli> time_span = duration_cast<duration<double, std::milli> >(end - start);
        avgTime = time_span.count();
    }
    
    glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchyDQ(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}
glm::mat4 RigMesh::calcInterpolatedRotationTranslationDQ(float animationTime, const aiNodeAnim* pNodeAnim)
{
    glm::mat4 finalRotTrans;
    
    aiVector3D StartPos, EndPos;
    aiQuaternion StartQuat, EndQuat;
    
    if (pNodeAnim->mNumPositionKeys == 1 && pNodeAnim->mNumRotationKeys == 1)
    {
        aiVector3D StartPos = pNodeAnim->mPositionKeys[0].mValue;
        aiQuaternion StartQuat = pNodeAnim->mRotationKeys[0].mValue;
        mat4 translationMatrix = glm::translate(mat4(1.0), vec3(StartPos.x, StartPos.y, StartPos.z));
        aiMatrix4x4 assimpRotationMatrix(StartQuat.GetMatrix());
        mat4 rotationMatrix;
        aiMat2glmMat(&assimpRotationMatrix, rotationMatrix);
        finalRotTrans = translationMatrix * rotationMatrix;
    }
    else{
        float Factor;
        if (pNodeAnim->mNumPositionKeys == 1 && pNodeAnim->mNumRotationKeys > 1)
        {
            StartPos = pNodeAnim->mPositionKeys[0].mValue;
            EndPos = pNodeAnim->mPositionKeys[0].mValue;
            
            Factor = getStartEndQuat(StartQuat, EndQuat, animationTime, pNodeAnim);
        }
        else if (pNodeAnim->mNumPositionKeys > 1 && pNodeAnim->mNumRotationKeys > 1)
        {
            getStartEndPos(StartPos, EndPos, animationTime, pNodeAnim);
            Factor = getStartEndQuat(StartQuat, EndQuat, animationTime, pNodeAnim);
        }
        else // >1 ==1
        {
            Factor = getStartEndPos(StartPos, EndPos, animationTime, pNodeAnim);
            StartQuat = pNodeAnim->mRotationKeys[0].mValue;
        }
        
        glm::dualquat startDQ(glm::quat(StartQuat.w, StartQuat.x, StartQuat.y, StartQuat.z), glm::vec3(StartPos.x, StartPos.y, StartPos.z));
        glm::dualquat endDQ(glm::quat(EndQuat.w, EndQuat.x, EndQuat.y, EndQuat.z), glm::vec3(EndPos.x, EndPos.y, EndPos.z));
        
        if (Factor<0)
        {
            Factor=-Factor;
        }
        
        glm::dualquat finalDQ = glm::lerp(startDQ, endDQ, Factor);
        
        glm::mat3x4 final2 = glm::mat3x4_cast(finalDQ);
        finalRotTrans = glm::transpose(glm::mat4(final2));
    }
    
    return finalRotTrans;
}

#endif


glGA::aiNodeAnimglGA* RigMesh::findNodeAnim(glGA::aiAnimationglGA* pAnimation, const std::string NodeName)
{
    //std::cout<<"@@@GPTEMP: inside findNodeAnim() "<<std::endl;
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        glGA::aiNodeAnimglGA* pNodeAnim = pAnimation->mChannels[i];
        
        if (std::string(pNodeAnim->assimpAiNode->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }
    
    return NULL;
}

const aiNodeAnim* RigMesh::findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    //std::cout<<"@@@GPTEMP: inside findNodeAnim() "<<std::endl;
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        
        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }
    
    return NULL;
}

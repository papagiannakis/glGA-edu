//
//  glGARigMesh.h
//  
//
//  Created by George Papagiannakis
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//
//  References: 
//      http://ogldev.atspace.co.uk/, Etay Meiri

#ifndef glGACharacterApp_glGARigMesh_h
#define glGACharacterApp_glGARigMesh_h


#include "glGAMeshEntry.h"

#include <map>
#include <vector>
#include <string>
#include <cassert>                  //includes C assert.h
// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/anim.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/Logger.hpp>        // Logging facility
#include <assimp/DefaultLogger.hpp>
#include <assimp/matrix4x4.h>
#include "glGAMath/glGAMath.h"
//GLEW
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/dual_quaternion.hpp>

#include "glGA/glGAHelper.h"

#if !defined(__APPLE__)
#include <chrono>
//typedef	unsigned int		uint;
#endif

// Select Animation Interpolation method in glGARigMesh.h
// Quaternions: #define INTERPOLATION QUAT
// Dual Quaternions: #define INTERPOLATION DQUAT
// Euclidean GA: #define INTERPOLATION GA
// Conformal GA: #define INTERPOLATION CGA
// Euclidean GA GAALOP:#define INTERPOLATION GA_GAALOP
// Euclidean CGA GAALOP:#define INTERPOLATION CGA_GAALOP
// Euclidean CGA GAALOP converted to rotors:#define INTERPOLATION CGA_GAALOP_ROTORS

// for Euclidean GA and Conformal GA libglGAMath required

using namespace glm;

namespace glGA {
    
    
    struct aiNodeAnimglGA
    {
        aiNodeAnim* assimpAiNode;
        
        #if INTERPOLATION==CGA_GAALOP
        aiRotor* rotor;
        #endif
        
        #if INTERPOLATION==GA_GAALOP || INTERPOLATION==CGA_GAALOP
        aiRotor* rotorGA_GAALOP;
        #endif
        
        #if INTERPOLATION==CGA_GAIGEN
        c3ga::rotor* rotorGaigen;
        #endif
        
        #if INTERPOLATION==GA_GAIGEN
        e3ga::rotor* rotorGaigen2;
        #endif
        
        #if INTERPOLATION==GA_VERSOR || INTERPOLATION==CGA_VERSOR
        vsr::cga::Rot* rotorVersor;
        #endif
    };
    
    struct aiAnimationglGA
    {
        /** The number of bone animation channels. Each channel affects
         *  a single node. */
        unsigned int mNumChannels;
        
        /** The node animation channels. Each channel affects a single node.
         *  The array is mNumChannels in size. */
        aiNodeAnimglGA** mChannels;
    };
    
    struct aiNodeglGA
    {
        C_STRUCT aiString mName;
        
        /** The transformation relative to the node's parent. */
        C_STRUCT aiMatrix4x4 mTransformation;
        
        /** Parent node. NULL if this node is the root node. */
        C_STRUCT aiNodeglGA* mParent;
        
        /** The number of child nodes of this node. */
        unsigned int mNumChildren;
        
        /** The child nodes of this node. NULL if mNumChildren is 0. */
        C_STRUCT aiNodeglGA** mChildren;
        
        #if INTERPOLATION==GA_GAALOP || INTERPOLATION==CGA_GAALOP
        float* rotor = new float[32];
        #endif
        
        #if INTERPOLATION==GA_GAIGEN || INTERPOLATION==CGA_GAIGEN
        c3ga::TRSversor versorGaigen;
        #endif
    };
    
    
}

class RigMesh
{
public:
    glGA::aiNodeglGA* rootNode;
    glGA::aiAnimationglGA* aiAnim;
    
    static float IdentityVersor[32];
    float scale;
    glm::vec3 position, rotation;
    float scaleCGA;
    glm::mat4 initialTransformation;
    int i=0;
    float total=0;
    float avgTime=0;
    float getAnimationInterpolationTime();
    
    RigMesh();
    
    ~RigMesh();
    /*
     * Simple loadRigMesh to load simple 3D model with Phong based illumination
     */
    bool        loadRigMesh(const std::string& filename);
    bool        loadRigMesh(const std::string& filename, glm::vec3 rotation, glm::vec3 position, float scale);
    
    void        render();
    
    uint numBones() const
    {
        return m_NumBones;
    }
    
    void boneTransform(float TimeInSeconds, std::vector<glm::mat4>& transforms);
    void readNodeHierarchyQuat(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform);
    
    #if INTERPOLATION==DQUAT
    void boneTransformDQ(float TimeInSeconds, std::vector<glm::mat4>& transforms);
    void readNodeHierarchyDQ(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform);
    glm::mat4 calcInterpolatedRotationTranslationDQ(float animationTime, const aiNodeAnim* pNodeAnim);
    #endif
    
    float getStartEndQuat(aiQuaternion& Start, aiQuaternion& End, float animationTime, const aiNodeAnim* pNodeAnim);
    
#if INTERPOLATION==GA_GAIGEN
    void boneTransformGA_Gaigen(float TimeInSeconds, std::vector<glm::mat4>& transforms);
    void readNodeHierarchyGA_Gaigen(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform);
    glm::mat4 calcInterpolatedRotationGA_GAIGEN(float animationTime, const glGA::aiNodeAnimglGA* nodeAnim);
    float calcInterpolatedScalingGA(float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim);
#endif
    
#if INTERPOLATION==CGA_GAIGEN
    void boneTransformCGA_Gaigen(float TimeInSeconds, std::vector<glm::mat4>& transforms);
    void readNodeHierarchyCGA_Gaigen(float animationTime, const glGA::aiNodeglGA* pNode, const c3ga::TRSversor& parentTransform);
    c3ga::TRSversor calcInterpolatedRotationTranslationCGA_GAIGEN(float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim);
    float getStartEndRotorGaigen(c3ga::rotor& Start, c3ga::rotor& End, float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim);
#endif

#if INTERPOLATION==GA_VERSOR
    void boneTransformGA_Versor(float TimeInSeconds, std::vector<glm::mat4>& transforms);
    glm::mat4 calcInterpolatedRotationGAVersor(float animationTime, const glGA::aiNodeAnimglGA* nodeAnim);
    void readNodeHierarchyGA_Versor(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform);
#endif
    
#if INTERPOLATION==CGA_VERSOR
    void boneTransformCGA_Versor(float TimeInSeconds, std::vector<glm::mat4>& transforms);
    vsr::cga::Mot calcInterpolatedRotationTranslationCGA_Versor(float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim);
    void readNodeHierarchyCGA_Versor(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform);
#endif
    
#if INTERPOLATION==GA_VERSOR || INTERPOLATION==CGA_VERSOR
    float getStartEndQuat(vsr::cga::Rot& Start, vsr::cga::Rot& End, float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim);
#endif
    
#if INTERPOLATION==GA_GAALOP
    //GA GAALOP
    void boneTransformGA_GAALOP(float TimeInSeconds, std::vector<glm::mat4>& transforms);
    aiRotor calculateGA_GAALOP_Rot(float srcAngle, float srcX, float srcY, float srcZ);
    void readNodeHierarchyGA_GAALOP(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform);
#endif
    
#if INTERPOLATION==CGA_GAALOP
    //CGA GAALOP
    std::vector<RotorDefScale> boneTransformCGA_GAALOP(float TimeInSeconds);
    RotorDef calcInterpolatedRotationTranslationCGA_GAALOP(float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim);
    aiRotor calculate(float angle, float axis1X, float axis1Y, float axis1Z);
    float getStartEndQuat(aiRotor& Start, aiRotor& End, float animationTime, const glGA::aiNodeAnimglGA* pNodeAnim);
    
    void readNodeHierarchyCGA_GAALOP(float animationTime, const glGA::aiNodeglGA* pNode, float* parentTransform);
#endif
#if INTERPOLATION==GA_GAALOP || INTERPOLATION==CGA_GAALOP
    float calcInterpolatedScalingGA(float animationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedRotationGA_GAALOP(aiQuaternion& out, float animationTime, const glGA::aiNodeAnimglGA* nodeAnim);
#endif
    
    glGA::aiNodeglGA* copyAiNode(aiNode* node, glGA::aiNodeglGA* parent);
    void copyAnimationData();
    void QuatToRotors(aiNode* pNode);
    glGA::aiNodeAnimglGA* findNodeAnim2(glGA::aiAnimationglGA* pAnimation, const std::string NodeName);
    
    
    /*
     * ASSIMP uses row-major 4x4 matrices whereas OpenGL and
     * GLM assume column major 4x4 matrices.
     ASSIMP:                   GLM:
     a1 a2 a3 a4            a1 b1 c1 d1
     b1 b2 b3 b4            a2 b2 c2 d2
     c1 c2 c3 c4            a3 b3 c3 d3
     d1 d2 d3 d4            a4 b4 c4 d4
     */
    void aiMat2glmMat(const aiMatrix4x4 *from, glm::mat4 &to) {
        to[0][0] = from->a1; to[1][0] = from->a2;
        to[2][0] = from->a3; to[3][0] = from->a4;
        to[0][1] = from->b1; to[1][1] = from->b2;
        to[2][1] = from->b3; to[3][1] = from->b4;
        to[0][2] = from->c1; to[1][2] = from->c2;
        to[2][2] = from->c3; to[3][2] = from->c4;
        to[0][3] = from->d1; to[1][3] = from->d2;
        to[2][3] = from->d3; to[3][3] = from->d4;
    }
    
    
#define NUM_BONES_PER_VERTEX    4
#define MAX_BONES               100
    
#ifndef INDEX_BUFFER
#define INDEX_BUFFER        0
#endif
#ifndef POS_VB
#define POS_VB              1
#endif
#ifndef NORMAL_VB
#define NORMAL_VB           2
#endif
#ifndef TEXCOORD_VB
#define TEXCOORD_VB         3
#endif
    
    class BoneInfo
    {
    public:
        glm::mat4   BoneOffset;
        glm::dualquat   BoneOffsetDQ;
        #if INTERPOLATION==CGA_GAALOP
        float*   BoneOffsetVersor;
        #endif
        
        #if INTERPOLATION==CGA_GAIGEN
        c3ga::TRSversor BoneOffsetGaigen;
        #endif
        
        glm::mat4   FinalTransformation;
        glm::dualquat   FinalTransformationDQ;
        float*   FinalTransformationVersor;
        #if INTERPOLATION==GA_GAALOP || INTERPOLATION==CGA_GAALOP
        RotorDef   FinalTransformationVersorRotorDef;
        #endif
        #if INTERPOLATION==GA_GAIGEN || INTERPOLATION==CGA_GAIGEN
        c3ga::TRSversor FinalTransformationGaigen;
        #endif
        BoneInfo()
        {
            BoneOffset = glm::mat4( 0.0 ); // initialize matrix to zero
            FinalTransformation = glm::mat4( 0.0 );
        }
        
    };
    
    // Private class containing for a single vertex, an array of 4 Bone IDs and 4 Weights
    class VertexBoneData
    {
    public:
        uint IDs[NUM_BONES_PER_VERTEX];
        float Weights[NUM_BONES_PER_VERTEX];
        
        VertexBoneData()
        {
            reset();
        };
        
        void reset()
        {
            ZERO_MEM(IDs);
            ZERO_MEM(Weights);
        }
        
        void addBoneData(uint boneID, float weight);
    };
    
private:
    
    
    
    
    float getStartEndPos(aiVector3D& Start, aiVector3D& End, float animationTime, const aiNodeAnim* pNodeAnim);
    
    
    
    
    float getStartEndQuat(aiRotor& Start, aiRotor& End, float animationTime, const aiNodeAnim* pNodeAnim);
    
    glm::mat4 calcInterpolatedRotationTranslationCGA_GAALOP(float animationTime, const aiNodeAnim* pNodeAnim);
    

    
    void calcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim);
    uint findScaling(float animationTime, const aiNodeAnim* pNodeAnim);
    uint findRotation(float animationTime, const aiNodeAnim* pNodeAnim);
    uint findPosition(float animationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
    glGA::aiNodeAnimglGA* findNodeAnim(glGA::aiAnimationglGA* pAnimation, const std::string NodeName);
    
    
    void computeTransformVertices(glm::mat4& initialTransformation);
    bool initFromScene(const aiScene* pScene, const std::string& filename);
    void initRigMesh(uint RigMeshIndexB,
                     const aiMesh* paiMesh,
                     std::vector<glm::vec3>& positions,
                     std::vector<glm::vec3>& normals,
                     std::vector<glm::vec2>& texCoords,
                     std::vector<VertexBoneData>& bones,
                     std::vector<unsigned int>& indices
                     );
    bool initMaterials(const aiScene* pScene, const std::string& filename);
    void clear();
    
protected:
    void loadBones(uint meshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& bones);
    
public:
    
    std::vector<glm::vec3>          Positions;
    std::vector<glm::vec3>          Normals;
    std::vector<glm::vec2>          TexCoords;
    std::vector<VertexBoneData>     Bones;
    std::vector<unsigned int>       Indices;
    
    enum VBO_TYPES
    {
        INDEX_VBO,
        POS_VBO,
        NORMAL_VBO,
        TEXCOORD_VBO,
        BONE_VBO,
        COL_VBO,
        NUM_VBOs
    };
    
    //private:
    std::vector<MeshEntry>       m_Entries;
    std::vector<Texture*>           m_Textures;
    
    std::map<std::string, uint>     m_BoneMapping; //maps a bone name to its bone index
    uint                            m_NumBones;
    std::vector<BoneInfo>           m_BoneInfo;
    glm::mat4                       m_GlobalInverseTransform;
    glm::dualquat                   m_GlobalInverseTransformDQ;
    #if INTERPOLATION==GA_GAIGEN || INTERPOLATION==CGA_GAIGEN
    c3ga::TRSversor                 m_GlobalInverseTransformGaigen;
    #endif
    #if INTERPOLATION==GA_GAALOP || INTERPOLATION==CGA_GAALOP
    float*                          m_GlobalInverseTransformVersor;
    #endif
    bool                            m_useGAforInterpolation;
    float                           m_GAfactorDenominator;
    const aiScene*                  m_pScene;
    Assimp::Importer*                m_Importer; //a pointer to Importer is crucial otherwise it goes out of scope and invalidates aiScene
    
}; // end class RigMesh



#endif //glGACharacterApp_glGARigMesh_h

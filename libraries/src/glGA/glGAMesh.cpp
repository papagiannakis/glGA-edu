//
//  glGAMesh.cpp
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//
//  References:
//      http://ogldev.atspace.co.uk/, Etay Meiri

#include <iostream>
#include <cassert>

#include "glGA/glGAMesh.h"
#include "glGA/glGAMeshEntry.h"

#define GLM_SWIZZLE
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

using namespace glm;

#define USE_OPENGL32

#define AI_CONFIG_PP_PTV_NORMALIZE   "PP_PTV_NORMALIZE"

Mesh::Mesh() : initialTransformation(glm::mat4(1.0)), numVertices(0), numIndices(0) {}

bool Mesh::loadMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyOptimised)
{
    this->rotation = rotation;
    this->position = position;
    this->scale = scale;

    //release a previously loaded mesh (if it exists)
    numIndices = 0;
    numVertices = 0;


    bool Ret = false;
    //create an instance of the importer class
    Assimp::Importer importer;

    Assimp::DefaultLogger::create( ASSIMP_DEFAULT_LOG_NAME,
                                  Assimp::Logger::VERBOSE,
                                  aiDefaultLogStream_DEBUGGER|aiDefaultLogStream_FILE,
                                  nullptr );

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example:
    // aiProcess_CalcTangentSpace       |
    // aiProcess_Triangulate            |
    // aiProcess_JoinIdenticalVertices  |
    // aiProcess_SortByPType |
    // aiProcess_FlipUVs
    // NOTE: you cannot have aiProcess_GenNormals AND aiProcess_GenSmoothNormals!
    aiScene* pScene = nullptr;

    if (!fullyOptimised)
    {
        pScene = const_cast<aiScene*> ( importer.ReadFile( filename.c_str(),
                                                          aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace) );
    }
    else
    {
        // These settings below work fine with Crytek Sponza
        pScene = const_cast<aiScene*> (importer.ReadFile( filename.c_str(),
                                                         aiProcess_CalcTangentSpace              |
                                                         aiProcess_GenSmoothNormals              |
                                                         aiProcess_JoinIdenticalVertices         |
                                                         aiProcess_ImproveCacheLocality          |
                                                         aiProcess_LimitBoneWeights              |
                                                         aiProcess_RemoveRedundantMaterials      |
                                                         aiProcess_SplitLargeMeshes              |
                                                         aiProcess_Triangulate                   |
                                                         aiProcess_GenUVCoords                   |
                                                         aiProcess_SortByPType                   |
                                                         aiProcess_FindDegenerates               |
                                                         aiProcess_FindInvalidData               |
                                                         aiProcess_TransformUVCoords             |
                                                         aiProcess_FlipUVs                       |
                                                         aiProcess_ImproveCacheLocality          |
                                                         aiProcess_ValidateDataStructure         |
                                                         /*

                                                          aiProcess_OptimizeGraph                 |
                                                          aiProcess_FixInfacingNormals            |
                                                          */
                                                         aiProcess_OptimizeMeshes                |

                                                         aiProcess_PreTransformVertices
                                                         ) );
    }
    if (pScene) {
        Ret = initFromScene(pScene, filename);
    }
    else
    {
        std::cout<<"Error parsing "<< filename.c_str() <<": "<<importer.GetErrorString() <<std::endl;
    }

    //make sure the VAO is not changed from outside
    //glBindVertexArray(0);

    mat4 tr = translate(mat4(), position);
    mat4 rotateX = rotate(tr, rotation.x, vec3(1.0, 0.0, 0.0));
    mat4 rotateY = rotate(rotateX, rotation.y, vec3(0.0, 1.0, 0.0));
    mat4 rotateZ = rotate(rotateY, rotation.z, vec3(0.0, 0.0, 1.0));
    mat4 initialTransformation = glm::scale(rotateZ, vec3(scale, scale, scale));
    this->initialTransformation = initialTransformation;

    return Ret;
}//end loadMesh()

bool Mesh::loadMesh(const std::string& filename, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyOptimised)
{
    this->rotation = rotation;
    this->position = position;
    this->scale = scale;

    //release a previously loaded mesh (if it exists)
    numIndices = 0;
    numVertices = 0;

    bool Ret = false;
    //create an instance of the importer class
    Assimp::Importer importer;

    Assimp::DefaultLogger::create( ASSIMP_DEFAULT_LOG_NAME,
                                  Assimp::Logger::VERBOSE,
                                  aiDefaultLogStream_DEBUGGER|aiDefaultLogStream_FILE,
                                  nullptr );

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example:
    // aiProcess_CalcTangentSpace       |
    // aiProcess_Triangulate            |
    // aiProcess_JoinIdenticalVertices  |
    // aiProcess_SortByPType |
    // aiProcess_FlipUVs
    // NOTE: you cannot have aiProcess_GenNormals AND aiProcess_GenSmoothNormals!
    aiScene* pScene = nullptr;

    if (!fullyOptimised)
    {
        pScene = const_cast<aiScene*> ( importer.ReadFile( filename.c_str(),
                                                          aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace) );
    }
    else
    {
        // These settings below work fine with Crytek Sponza
        pScene = const_cast<aiScene*> (importer.ReadFile( filename.c_str(),
                                                         aiProcess_CalcTangentSpace              |
                                                         aiProcess_GenSmoothNormals              |
                                                         aiProcess_JoinIdenticalVertices         |
                                                         aiProcess_ImproveCacheLocality          |
                                                         aiProcess_LimitBoneWeights              |
                                                         aiProcess_RemoveRedundantMaterials      |
                                                         aiProcess_SplitLargeMeshes              |
                                                         aiProcess_Triangulate                   |
                                                         aiProcess_GenUVCoords                   |
                                                         aiProcess_SortByPType                   |
                                                         aiProcess_FindDegenerates               |
                                                         aiProcess_FindInvalidData               |
                                                         aiProcess_TransformUVCoords             |
                                                         aiProcess_FlipUVs                       |
                                                         aiProcess_ImproveCacheLocality          |
                                                         aiProcess_ValidateDataStructure         |
                                                         /*

                                                          aiProcess_OptimizeGraph                 |
                                                          aiProcess_FixInfacingNormals            |
                                                          */
                                                         aiProcess_OptimizeMeshes                |

                                                         aiProcess_PreTransformVertices
                                                         ) );
    }
    if (pScene) {
        Ret = initFromScene(pScene, filename);
    }
    else
    {
        std::cout<<"Error parsing "<< filename.c_str() <<": "<<importer.GetErrorString() <<std::endl;
    }

    //make sure the VAO is not changed from outside
    //glBindVertexArray(0);

    mat4 tr = translate(mat4(), position);
    mat4 rotateX = rotate(tr, rotation.x, vec3(1.0, 0.0, 0.0));
    mat4 rotateY = rotate(rotateX, rotation.y, vec3(0.0, 1.0, 0.0));
    mat4 rotateZ = rotate(rotateY, rotation.z, vec3(0.0, 0.0, 1.0));
    mat4 initialTransformation = glm::scale(rotateZ, vec3(scale, scale, scale));
    this->initialTransformation = initialTransformation;

    return Ret;
}//end loadMesh()

bool Mesh::loadMesh(const std::string& filename, bool fullyOptimised)
{
    //release a previously loaded mesh (if it exists)
    numIndices = 0;
	numVertices = 0;

    bool Ret = false;
    //create an instance of the importer class
    Assimp::Importer importer;

    Assimp::DefaultLogger::create( ASSIMP_DEFAULT_LOG_NAME,
                                  Assimp::Logger::VERBOSE,
                                  aiDefaultLogStream_DEBUGGER|aiDefaultLogStream_FILE,
                                   nullptr );

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example:
    // aiProcess_CalcTangentSpace       |
    // aiProcess_Triangulate            |
    // aiProcess_JoinIdenticalVertices  |
    // aiProcess_SortByPType |
    // aiProcess_FlipUVs
    // NOTE: you cannot have aiProcess_GenNormals AND aiProcess_GenSmoothNormals!
    aiScene* pScene = nullptr;

    if (!fullyOptimised)
    {
        pScene = const_cast<aiScene*> ( importer.ReadFile( filename.c_str(),
                aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace) );
    }
    else
    {
    // These settings below work fine with Crytek Sponza
    pScene = const_cast<aiScene*> (importer.ReadFile( filename.c_str(),
                                              aiProcess_CalcTangentSpace              |
                                              aiProcess_GenSmoothNormals              |
                                              aiProcess_JoinIdenticalVertices         |
                                              aiProcess_ImproveCacheLocality          |
                                              aiProcess_LimitBoneWeights              |
                                              aiProcess_RemoveRedundantMaterials      |
                                              aiProcess_SplitLargeMeshes              |
                                              aiProcess_Triangulate                   |
                                              aiProcess_GenUVCoords                   |
                                              aiProcess_SortByPType                   |
                                              aiProcess_FindDegenerates               |
                                              aiProcess_FindInvalidData               |
                                              aiProcess_TransformUVCoords             |
                                              aiProcess_FlipUVs                       |
                                              aiProcess_ImproveCacheLocality          |
                                              aiProcess_ValidateDataStructure         |
                                              /*

                                               aiProcess_OptimizeGraph                 |
                                               aiProcess_FixInfacingNormals            |
                                               */
                                              aiProcess_OptimizeMeshes                |

                                              aiProcess_PreTransformVertices
                                              ) );
    }
    if (pScene) {
        Ret = initFromScene(pScene, filename);
    }
    else
    {
        std::cout<<"Error parsing "<< filename.c_str() <<": "<<importer.GetErrorString() <<std::endl;
    }

    //make sure the VAO is not changed from outside
    //glBindVertexArray(0);

    return Ret;
}//end loadMesh()

bool Mesh::initFromScene(const aiScene* pScene, const std::string& filename)
{
    m_Entries.resize(pScene->mNumMeshes);

    // count the num of vertices and indices
    for (unsigned int i=0; i<m_Entries.size(); ++i)
    {
        m_Entries[i].materialIndex  = pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].numIndices     = pScene->mMeshes[i]->mNumFaces *3;
        m_Entries[i].baseVertex     = numVertices;
        m_Entries[i].baseIndex      = numIndices;

        numVertices += pScene->mMeshes[i]->mNumVertices;
        numIndices  += m_Entries[i].numIndices;
    }

    // Reserve space in the vectors for the vertex attributes and indixes
    Positions.reserve(numVertices);
    Normals.reserve(numVertices);
    TexCoords.reserve(numVertices);
    Indices.reserve(numIndices);
	Tangents.reserve(numVertices);

    // initialize the scene meshes one by one
    for (unsigned int i=0; i<m_Entries.size(); ++i) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
		initMesh(paiMesh, Positions, Normals, TexCoords, Indices, Tangents);
    }
    if (!initMaterials(pScene, filename)) {
        return false;
    }

    // @@@GPTODO: Colors are missing

    return  GLCheckError();

}//end initFromScene()

void Mesh::initMesh(const aiMesh* paiMesh,
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

        positions.emplace_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        if (paiMesh->HasNormals())
        {
            if(pNormal!=nullptr)
            {
                vec4 normal(pNormal->x, pNormal->y, pNormal->z, 1.0);
                normals.emplace_back(glm::vec3(normal[0], normal[1], normal[2]));
            }
        }

        texCoords.emplace_back(glm::vec2(pTexCoord->x,pTexCoord->y));

        if (paiMesh->HasTangentsAndBitangents())
        {
            if (pTangent!=nullptr)
                tangents.emplace_back(glm::vec3(pTangent->x, pTangent->y, pTangent->z));
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

bool Mesh::initMaterials(const aiScene* pScene, const std::string& filename)
{
    // Extract the directory part from the file name
    const auto slashIndex = filename.find_last_of('/');
    std::string dir;

    if (slashIndex == std::string::npos) {
        dir = ".";
    } else if (slashIndex == 0) {
        dir = "/";
    } else {
        dir = filename.substr(0, slashIndex);
    }

    // Initialize the materials
    aiString path;
    aiMaterial* pMaterial;
    std::string fullPath;

    for (auto i = 0; i < pScene->mNumMaterials; ++i) {
        pMaterial = pScene->mMaterials[i];

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE)) {
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
                fullPath = dir + "/" + path.data;
                m_Textures.emplace_back(Texture(GL_TEXTURE_2D, fullPath));

                if (!m_Textures.back().loadTexture()) {
                    std::cout << "Error Loading texture: " << fullPath << std::endl;
                    m_Textures.pop_back();
                    return false;
                } else {
                    std::cout << "Loaded texture: " << fullPath << std::endl;
                    return true;
                }
            }
        }
    }

    return true;
}//end initMaterials()

void Mesh::render()
{
    for (const auto& pEntry : m_Entries) {
        const auto materialIndex = pEntry.materialIndex;

        if (materialIndex < m_Textures.size()) {
            m_Textures[materialIndex].bindTexture(GL_TEXTURE0);
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

#ifdef USE_OPENGL32

        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 pEntry.numIndices,
                                 GL_UNSIGNED_INT,
                                 reinterpret_cast<void*>(sizeof(unsigned int) * pEntry.baseIndex),
                                 pEntry.baseVertex);

#else
        glDrawElements(GL_TRIANGLES, pEntry.numIndices, GL_UNSIGNED_INT, 0);
#endif
    }//end for

}//end render()







































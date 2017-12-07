//
//  glGAMesh.h
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//
//  References:
//      http://ogldev.atspace.co.uk/, Etay Meiri

#ifndef glGACharacterApp_glGAMesh_h
#define glGACharacterApp_glGAMesh_h

#include "glGAMeshEntry.h"

#include <map>
#include <vector>
#include <string>
#include <cassert>                  //includes C assert.h
// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/Logger.hpp>        // Logging facility
#include <assimp/DefaultLogger.hpp>
// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
//GLEW
#include <GL/glew.h>

#include "glGA/glGAHelper.h"

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

// Basic vertex data structure
class Vertex
{

public:

    glm::vec3   m_pos;
    glm::vec2   m_tex;
    glm::vec3   m_normal;

    Vertex() = default;

    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
    {
        m_pos       = pos;
        m_tex       = tex;
        m_normal    = normal;
    }

};//end class glGAVertex

// Basic static mesh data structure for loading any object static mesh with the Assimp library
class Mesh
{
public:
    glm::vec3 position, rotation;
    float scale;
    glm::mat4 initialTransformation;

    std::vector<MeshEntry>  m_Entries;

    Mesh();

    /*
     * Use this loadMesh to load a static mesh to be displayed with the standard Phong-based illumination model
     */
    bool loadMesh(const std::string& filename, bool fullyOptimised=false);
    bool loadMesh(const std::string& filename, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyOptimised);
    /*
     * Use this loadMesh to load a static 3D mesh in order to be displayed with the advanced PRT illumination model
     */
    bool loadMesh(const std::string& filename, bool shadowed, glm::vec3 rotation, glm::vec3 position, float scale, bool fullyOptimised);

    void render();

    //GLuint                      m_Buffers[4];
    //GLuint                      m_VAO;
    std::vector<Texture>        m_Textures;
    std::vector<GLuint>         m_TextureSamplers;
    std::vector<glm::vec3>      Positions;
    std::vector<glm::vec3>      Colors;
    std::vector<glm::vec3>      Normals;
    std::vector<glm::vec2>      TexCoords;
	std::vector<glm::vec3>		Tangents;
    std::vector<unsigned int>   Indices;

    unsigned int numVertices;
    unsigned int numIndices;

private:

    bool initFromScene(const aiScene* pScene, const std::string& filename);
    void initMesh(const aiMesh* paiMesh,
                  std::vector<glm::vec3>& positions,
                  std::vector<glm::vec3>& normals,
                  std::vector<glm::vec2>& texCoords,
                  std::vector<unsigned int>& indices,
				  std::vector<glm::vec3>& tangents
                  );
    bool initMaterials(const aiScene* pScene, const std::string& filename);


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
#ifndef TANGENT_VB
#define TANGENT_VB			4
#endif
#ifndef COLOR_VB
#define COLOR_VB			5
#endif

}; // end class Mesh





#endif //glGACharacterApp_glGAMesh_h

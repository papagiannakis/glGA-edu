//
//  Structs.h
//  PRT
//
//  Created by margarita on 3/8/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef __MetaioOpenGLES__Structs__
#define __MetaioOpenGLES__Structs__

#include <stdio.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

using namespace glm;

namespace StructDef
{
    struct vector3
    {
    public:
        
        double x;
        double y;
        double z;
        
        vector3()
        {
        }
        
        vector3(double x, double y, double z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
    };
    
    struct SHSample
    {
        vector3 sphereCoord;
        vec2 texture;

        vector3 vectorCoord;
        double *coeff;
    };
    
    struct rgb
    {
    public:
        float red;
        float green;
        float blue;
    };
}

struct Vertex1
{
public:
    vec3 position;
    vec2 texCoord;
    vec3 normal;
    vec3 color;
    
    Vertex1(vec3 position, vec2 texCoords, vec3 normal, vec3 color)
    {
        this->position = position;
        this->texCoord = texCoords;
        this->normal = normal;
        this->color = color;
    }
};

#endif /* Structs.h */

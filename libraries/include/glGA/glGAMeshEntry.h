//
//  glGAMeshEntry.h
//  glGACharacterApp
//
//  Created by Margrarita Papaefthimiou on 8/21/15.
//  Copyright (c) 2015 George Papagiannakis. All rights reserved.
//

#ifndef __glGACharacterApp__glGAMeshEntry__
#define __glGACharacterApp__glGAMeshEntry__

#include <stdio.h>

#define INVALID_MATERIAL    0xFFFFFFFF

class  MeshEntry
{
public:
    MeshEntry();
    
    unsigned int numIndices;
    unsigned int baseVertex;
    unsigned int baseIndex;
    unsigned int materialIndex;
};//MeshEntry

#endif /* defined(__glGACharacterApp__glGAMeshEntry__) */
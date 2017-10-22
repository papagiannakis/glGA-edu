//
//  glGAMeshEntry.cpp
//  glGACharacterApp
//
//  Created by Margrarita Papaefthimiou on 8/21/15.
//  Copyright (c) 2015 George Papagiannakis. All rights reserved.
//

#include "glGA/glGAMeshEntry.h"

MeshEntry::MeshEntry()
{
    numIndices      = 0;
    baseVertex      = 0;
    baseIndex       = 0;
    materialIndex   = INVALID_MATERIAL;
}
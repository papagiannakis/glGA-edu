//
//  HDRI_frame.h
//  PRT
//
//  Created by margarita on 3/10/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#ifndef HDRI_FRAME_H
#define HDRI_FRAME_H

#include "glGA/glGAMesh.h"

#include <iostream>
#include <fstream>

//reading hdr
#include "HDRloader.h"

class HDRI_frame {
 public:
    HDRI_frame();
    void ReadHDR(const char* fileName);
    void BindCubeMapTexture();
    void SetTextureUnit(int);
    void SetTextureImage(GLuint);
    void CreateCubeMapFaces();
    int TextureUnit;
    GLenum TextureUnitEnum;
    GLuint TextureImage;
    int dimx;
    int dimy;
    HDRLoaderResult hdrResult;
    float * cubemap[6];
 };

#endif

#include "HDRI_frame.h"
#include <GL/glew.h>

HDRI_frame::HDRI_frame()
{
	std::cout<<"\n";
}


void HDRI_frame::ReadHDR(const char* filename)
{

HDRLoader::load(filename,this->hdrResult);
this->dimx = this->hdrResult.width;
this->dimy = this->hdrResult.height;
}




void HDRI_frame::SetTextureUnit(int t)
{
 this->TextureUnit=t;
 switch (t)
 {
     case 0:
        this->TextureUnitEnum = GL_TEXTURE0;
		printf("\nTexture unit assigned to 0\n");
        break;
     case 1:
        this->TextureUnitEnum = GL_TEXTURE1;
		printf("\nTexture unit assigned to 1\n");
        break;
     case 2:
        this->TextureUnitEnum = GL_TEXTURE2;
		printf("\nTexture unit assigned to 2\n");
        break;
    case 3:
        this->TextureUnitEnum = GL_TEXTURE3;
		printf("\nTexture unit assigned to 3\n");
        break;
    case 4:
        this->TextureUnitEnum = GL_TEXTURE4;
		printf("\nTexture unit assigned to 4\n");
        break;
     default:
        std::cout << "something wrong with texture unit number" << std::endl;
        break;
 }
}

void HDRI_frame::BindCubeMapTexture()
{

    //only for hdr images atm as they are the only ones the CreateCubemapFaces work for


glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureImage);

glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


int tmpwidth = this->dimx/3;
int tmpheight = this->dimy/4;

glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA16F_ARB, tmpwidth, tmpheight, 0, GL_RGB, GL_FLOAT, this->cubemap[0]);

glTexImage2D (GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA16F_ARB, tmpwidth, tmpheight, 0, GL_RGB, GL_FLOAT, this->cubemap[1]);

glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA16F_ARB, tmpwidth, tmpheight, 0, GL_RGB, GL_FLOAT, this->cubemap[2]);

glTexImage2D (GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA16F_ARB, tmpwidth, tmpheight, 0, GL_RGB, GL_FLOAT, this->cubemap[3]);

glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA16F_ARB, tmpwidth, tmpheight, 0, GL_RGB, GL_FLOAT, this->cubemap[4]);

glTexImage2D (GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA16F_ARB, tmpwidth, tmpheight, 0, GL_RGB, GL_FLOAT, this->cubemap[5]);

glBindTexture(GL_TEXTURE_2D, this->TextureImage);
glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


}


void HDRI_frame::SetTextureImage(GLuint textimg)
{
 this->TextureImage = textimg;
}

void HDRI_frame::CreateCubeMapFaces()
{

//only works if texture is a cross cube map and if u are using a hdr image

    int w = this->dimx/3;
    int h = this->dimy/4;


    for(int i = 0; i < 6; ++i){
        this->cubemap[i] = new float[3*w*h];
    }

    int length = w * 3;
    int fulllength =  this->dimx * 3;

    for(int j = 0; j <  this->dimy; ++j){
        for(int i = 0; i < 3; ++i){

            // Positive Y
            if(j < h && i == 1)
                memcpy(this->cubemap[2] + (j%h)*3*w, this->hdrResult.cols + j * fulllength + i*length, length * sizeof(float));

            // Negative Y
            if(j >= 2*h && j < 3*h  && i == 1)
                memcpy(this->cubemap[3] + (j%h)*3*w, this->hdrResult.cols + j * fulllength + i*length, length * sizeof(float));

            // Positive X
            if(j>=h && j<2*h && i==2)
                memcpy(this->cubemap[0] + (j%h)*3*w, this->hdrResult.cols + j * fulllength + i*length, length * sizeof(float));

            // Negative X
            if(j>=h && j<2*h && i==0)
                memcpy(this->cubemap[1] + (j%h)*3*w, this->hdrResult.cols + j * fulllength + i*length, length * sizeof(float));

            // Positive Z
            if(j>=h && j<2*h && i==1)
                memcpy(this->cubemap[4] + (j%h)*3*w, this->hdrResult.cols + j * fulllength + i*length, length * sizeof(float));

            // Negative Z
            if(j>=3*h && j<4*h && i==1){
                int offset = (h-1-(j%h))*3*w;
                memcpy(this->cubemap[5] + offset, this->hdrResult.cols + j * fulllength + i*length, length * sizeof(float));
                for(int k = 0; k < (w/2)*3; k+=3){
                    float tmp;
                    for(int l = 0; l < 3; ++l){
                        tmp = this->cubemap[5][offset + k + l];
                        this->cubemap[5][offset + k + l] = this->cubemap[5][offset + 3*(w-1)-k + l];
                        this->cubemap[5][offset + 3*(w-1) -k + l] = tmp;
                    }
                }
            }
        }
    }


}
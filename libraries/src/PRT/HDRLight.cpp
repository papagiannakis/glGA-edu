//
//  HDRLight.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/10/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/HDRLight.h"
#include "PRT/Light.h"
#include <SOIL2/SOIL2/stb_image.h>
#include <SOIL2/SOIL2/SOIL2.h>
#include "SphericalHarmonics.h"

#ifndef __APPLE__
#define _USE_MATH_DEFINES 
#include <math.h>
#endif  // __APPLE__

using namespace std;

HDRLight::HDRLight(std::string img)
{
    int x, y, n;
    float* result;
    
    /* check if the image is HDR */
    if ( stbi_is_hdr(img.c_str()))
    {
        result = stbi_loadf(img.c_str(), &x, &y, &n, 0);
        if (result==NULL)
        {
            cout << "Probe HDRI image not found." << endl;
            exit(0);
        }
        
        if (x!=y)
        {
            cout << "Image must be probe HDRI format." << endl;
            exit(0);
        }
    }
    else
    {
        cout << "Image must be probe HDRI format." << endl;
        exit(0);
    }
	
    pixels = new StructDef::rgb[x*y*n];
    imgSz = x;
    int pos=x*y;
	
    for (int i=0; i<x*y*n; i+=3)
    {
        glm::vec3 color(result[i], result[i+1], result[i+2]);
        
        float gamma=2.0f;
        vec3 mapped2 = color / (color + vec3(1.0));
		// Exposure
		//float exposure = 1.0;
		//vec3 mapped2 = vec3(1.0) - exp(-color * exposure);
        // Gamma correction
        mapped2 = pow(mapped2, vec3(1.0 / gamma));
       
        pixels[pos].red = mapped2[0];
        pixels[pos].green = mapped2[1];
        pixels[pos].blue = mapped2[2];
		//pixels[pos].red = result[i];
		//pixels[pos].green = result[i + 1];
		//pixels[pos].blue = result[i + 2];
        pos--;
    }
	
    imgSz = x;
}

vec3 HDRLight::getLight(SHSample samplesCoefficients)
{
    double dx = samplesCoefficients.vectorCoord.x;
    double dy = samplesCoefficients.vectorCoord.y;
    double dz = samplesCoefficients.vectorCoord.z;
    
    // assume angle map projection
    const float one_over_pi = 1.0f / M_PI;
    float invl = 1.0f / sqrtf(dx * dx + dy * dy);
    float r = one_over_pi * acosf(dz) * invl;
    float u = dx * r;           // -1..1
    float v = dy * r;           // -1..1
    
    // map to pixel coordinates
    int x = int(u * imgSz + imgSz) >> 1;
    int y = int(v * imgSz + imgSz) >> 1;
    
    // RGB
    return vec3(pixels[y * imgSz + x].red, pixels[y * imgSz + x].green, pixels[y * imgSz + x].blue);
};

int HDRLight::diretionToImageSpace(SHSample samplesCoefficients)
{
    double dx = samplesCoefficients.vectorCoord.x;
    double dy = samplesCoefficients.vectorCoord.y;
    double dz = samplesCoefficients.vectorCoord.z;
    
    // assume angle map projection
    const float one_over_pi = 1.0f / M_PI;
    float invl = 1.0f / sqrtf(dx * dx + dy * dy);
    float r = one_over_pi * acosf(dz) * invl;
    float u = dx * r;           // -1..1
    float v = dy * r;           // -1..1
    
    // map to pixel coordinates
    int x = int(u * imgSz + imgSz) >> 1;
    int y = int(v * imgSz + imgSz) >> 1;
    
    // RGB
    return (y * imgSz + x);
};

void HDRLight::computeSHImage(glm::vec3* lightCoeffs, string name)
{
    int size=imgSz*6;
    int pixelsSize=imgSz * imgSz * 3;
     uint8* pixels1=new uint8[pixelsSize];
    for (int i=0; i<pixelsSize; i++)
    {
        pixels1[i]=0;
    }
            
    for (int i=0; i<size; i++)
    {
        for (int j=0; j<size; j++)
        {
            // generate samples
            double x = (i + 0.000001f)/ (double)size;
            double y = (j + 0.0000001f)/(double)size;
 
            double theta = 2.0f * acos(std::sqrt(1.0f - x));
            double phi = 2.0f * M_PI * y;
            SHSample cur;
 
            cur.sphereCoord = vector3(theta,phi,1.0) ;
 
            float X = sin(theta)*cos(phi);
            float Y = sin(theta)*sin(phi);
            float Z = cos(theta);
 
            cur.vectorCoord = vector3(X,Y,Z);
 
            int toImg = diretionToImageSpace(cur);
 
            //RGB
            float final1=0, final2=0,final3=0;
            for(int l=0; l<3; l++)
            {
                for(int m=-l; m<=l; m++)
                {
                    int index = l*(l+1)+m;
                    SphericalHarmonics yy;
                    final1 = final1 + lightCoeffs[index][0] * yy.y(l, m, theta, phi);
                    final2 = final2 + lightCoeffs[index][1] * yy.y(l, m, theta, phi);
                    final3 = final3 + lightCoeffs[index][2] * yy.y(l, m, theta, phi);
 
                }
            }
            
            pixels1[toImg*3]= final1*255.0f;
            pixels1[toImg*3+1]= final2*255.0f;
            pixels1[toImg*3+2]= final3*255.0f;
        }
    }
    SOIL_save_image(name.c_str(), SOIL_SAVE_TYPE_BMP, imgSz, imgSz, 3, pixels1);
 }

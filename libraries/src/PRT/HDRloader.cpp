#include "PRT/HDRloader.h"
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <iostream>
#include <SOIL2/SOIL2/stb_image.h>
#include <glm/glm.hpp>

using namespace std;

bool HDRLoader::load(const char *fileName, HDRLoaderResult &res)
{
    int x, y, n;
    float* result;
    
    /* check if the image is HDR */
    if ( stbi_is_hdr(fileName))
    {
        result = stbi_loadf(fileName, &x, &y, &n, 0);
        if (result==NULL)
        {
            cout << "Cross HDRI image not found." << endl;
            exit(0);
        }
        
        if (x==y)
        {
            cout << "Image must be cross HDRI format." << endl;
            exit(0);
        }
    }
    else
    {
        cout << "Image must be cross HDRI format." << endl;
        exit(0);
    }
    
    res.width = x;
    res.height = y;
    
    const float gamma = 2.2;
    
    for (int i=0; i<x*y*n; i+=3)
    {
        glm::vec3 color(result[i], result[i+1], result[i+2]);
        
        // Reinhard tone mapping
        glm::vec3 mapped = color / (color + glm::vec3(1.0));
        // Gamma correction
        mapped = pow(mapped, glm::vec3(1.0 / gamma));
        
        result[i] = mapped[0];
        result[i+1] = mapped[1];
        result[i+2] = mapped[2];
    }
    
    res.cols = result;
    
    printf("\nLoading cross HDRI successful.\n");
    
    return true;
}
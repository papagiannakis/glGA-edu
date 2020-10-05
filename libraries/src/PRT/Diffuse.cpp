//
//  Diffuse.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/Diffuse.h"
#include <fstream>
#include "stdio.h"
#include <string>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include "PRT/Split.h"
#include "glGA/glGAMeshPRT.h"

extern double Hs;
extern bool Ray_hit;
extern double **sh_buffer;
float u;
float v;
int bounces;
int index_array[3];

Diffuse::Diffuse(int numCoeff)
{
    this->numCoeff=numCoeff;
}

void Diffuse::setRotation(glm::mat4 rotation)
{
    this->rotation = rotation;
}

void Diffuse::coefficients(glm::mat4& transformation, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, std::string filename, bool shadow, KdTreeAccel *kdTree)
{
   for (int l=0; l<positions.size(); l++)
    {
        vec4 pos = transformation * glm::vec4(positions[l][0], positions[l][1], positions[l][2], 1);
        positions[l] = vec3(pos[0], pos[1], pos[2]);
    }
    
    filename.append(std::to_string(numCoeff));
    
    filename.append(".txt");
    
    const char * c = filename.c_str();
    
    FILE * file;
    file = fopen (c, "r");
    
	if (file == NULL)
	{

		bounces = 0;

		sh_buffer = (double**)malloc((bounces+1) * sizeof(double*));

		cout << "Compute coefficients: " << filename << endl;
		vertCoeff = new double[positions.size() * numCoeff];

		int numSamples = (int)samplesCoefficients.size();

		for (int n_bounces = 0; n_bounces <= bounces; n_bounces++){

			sh_buffer[n_bounces] = (double*)malloc(positions.size() * numCoeff * sizeof(double));
			memset(sh_buffer[n_bounces], 0, positions.size() * numCoeff * sizeof(double));


			// for each vertex
			if (n_bounces == 0){


				for (unsigned int i = 0; i < positions.size(); i++)
				{
#ifndef __APPLE__
					double *curVertCoeff = (double*)malloc(sizeof(double)*numCoeff);
#else
					double curVertCoeff[numCoeff];
#endif  // __APPLE__

					for (int j = 0; j < numCoeff; j++)
					{
						curVertCoeff[j] = 0.0;
					}


					// for each sample
					for (int j = 0; j < numSamples; j++)
					{
						// dot product of normal and sample
						double brightness;

						
						brightness = computeBrightness(normals[i], positions[i], samplesCoefficients[j], kdTree);


						if (brightness > 0.0)
						{
							for (int m = 0; m < numCoeff; m++)
							{
								double val = (brightness * samplesCoefficients.at(j).coeff[m]);

								curVertCoeff[m] += val;
							}
						}
					}


					double factor = 4.0f*M_PI / (double)numSamples;
					for (int j = 0; j < numCoeff; j++)
					{
						curVertCoeff[j] = curVertCoeff[j] * factor;
						vertCoeff[i*numCoeff + j] = curVertCoeff[j];
					}

					sh_buffer[n_bounces] = vertCoeff;
				
				}
				
			}
			else{

				Hs = 0.0;
				Ray_hit = false;
				u = 0.0;
				v = 0.0;
				index_array[0] = 0;
				index_array[1] = 0;
				index_array[2] = 0;
			
				for (unsigned int i = 0; i < positions.size(); i++)
				{
#ifndef __APPLE__
					double *curVertCoeff = (double*)malloc(sizeof(double)*numCoeff);
#else
					double curVertCoeff[numCoeff];
#endif  // __APPLE__


					for (int j = 0; j < numCoeff; j++)
					{
						curVertCoeff[j] = 0.0;
					}

					// for each sample
					for (int j = 0; j < numSamples; j++)
					{
						double brightness;

						brightness = computeBrightness(normals[i], positions[i], samplesCoefficients[j], kdTree);

						if (Hs > 0.0){
							if (Ray_hit){

							

								float w = 1.0 - (u + v);

							
								double *ptr0 = sh_buffer[n_bounces - 1] + index_array[0]*numCoeff;
								double *ptr1 = sh_buffer[n_bounces - 1] + index_array[1]*numCoeff;
								double *ptr2 = sh_buffer[n_bounces - 1] + index_array[2]*numCoeff;

								for (int k = 0; k < numCoeff; k++){
									curVertCoeff[k] = u * (*ptr0++) + v * (*ptr1++) + w * (*ptr2++);
								}

								for (int k = 0; k < numCoeff; k++){
									sh_buffer[n_bounces][i*numCoeff + k + 0 * numCoeff] += Hs * curVertCoeff[k + 0 * numCoeff];
								}
							}
						}
					}
				}
				double factor = 4.0f*M_PI / (double)numSamples;
				double *ptr = sh_buffer[n_bounces];
				for (int j = 0; j < positions.size() * numCoeff; j++)
				{
					*ptr++ *= factor;
				}
			}
		}

		for (int i = 1; i <= bounces; i++){
			double *ptra = sh_buffer[0];
			double *ptrb = sh_buffer[i];
			for (int j = 0; j < positions.size() * numCoeff; j++){
				*ptra++ += *ptrb++;
			}

		}

		writeToFile(c, (int)positions.size() * numCoeff);
	}
    else
    {
       readFile(c, (int) positions.size() * numCoeff);
    }
    
}

void Diffuse::writeToFile(const char *c, int coeffSize)
{
    ofstream myfile(c);
    
    if (myfile.is_open())
    {
        for (int i=0; i<coeffSize; i++)
        {
            myfile << vertCoeff[i];
            myfile << " ";
        }
        myfile.close();
        cout << " Done" << endl;
    }
    else
        cout << "Unable to open file";
}

void Diffuse::readFile(const char *c, int coeffSize)
{
    cout << "Loading coefficients " << c << endl;
    
    string line;
    
    vertCoeff = new double[coeffSize];
    
    ifstream myfile(c);
    
    if (myfile.is_open())
    {
        getline (myfile,line);
        
        std::istringstream buf(line);
        std::istream_iterator<std::string> beg(buf), end;
        
        std::vector<std::string> tokens(beg, end);
        
        for (int i=0; i<coeffSize; i++)
        {
            vertCoeff[i] = atof(tokens[i].c_str());
        }
        
        myfile.close();
    }
    else
        cout << "Unable to open file";
}

std::vector<glm::vec3> Diffuse::computeColors(int numVertices)
{
    std::vector<glm::vec3> Colors;
    
    Colors.resize(numVertices);
    for (int i=0; i<numVertices; i++)
    {
        glm::vec3 curColor(0.0,0.0,0.0);
        for (int j=0; j<numCoeff; j++)
        {
            glm::vec3 b = lightCoeffs[j];
            
            double a = vertCoeff[i*numCoeff + j];
            
            glm::vec3 color = vec3(b[0]*a,b[1]*a, b[2]*a);
            curColor +=  color;
        }
        glm::vec3 finalColor(curColor[0]/M_PI,curColor[1]/M_PI,curColor[2]/M_PI);
        Colors[i] = finalColor;
    }
    return Colors;
}

void Diffuse::setLightCoeffs(glm::vec3* lightCoeffs)
{
    this->lightCoeffs=lightCoeffs;
}

//
//  MonteCarlo.cpp
//  glGAframework
//
//  Created by Margrarita Papaefthimiou on 05/07/2017.
//
//

#include "PRT/MonteCarlo.h"
#include "PRT/HDRLight.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iterator>

void MonteCarlo::coefficients(glm::mat4& transformation, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, std::string filename, bool shadow, KdTreeAccel *kdTree)
{
    for (int l=0; l<positions.size(); l++)
    {
        vec4 pos = transformation * glm::vec4(positions[l][0], positions[l][1], positions[l][2], 1);
        positions[l] = vec3(pos[0], pos[1], pos[2]);
    }
    
    filename.append(".txt");
    
    const char * c = filename.c_str();
    
    FILE * file;
    file = fopen (c, "r");
    
    if (file == NULL)
    {
        
        cout << "Compute coefficients: " << filename << endl;
        vertCoeff.resize(positions.size());
        
        int numSamples = (int)samplesCoefficients.size();
        
        for (unsigned int i = 0; i < positions.size(); i++)
        {
            glm::vec3 curVertCoeff;
            
            curVertCoeff[0] = 0.0; curVertCoeff[1] = 0.0; curVertCoeff[2] = 0.0;
                
            // for each sample
            for (int j = 0; j < numSamples; j++)
            {
                // dot product of normal and sample
                double brightness = computeBrightness(normals[i], positions[i], samplesCoefficients[j], kdTree);
                
                if (brightness > 0.0)
                {
                    vec3 l = light->getLight(samplesCoefficients[j]);
                    double val1 = brightness * l[0];
                    double val2 = brightness * l[1];
                    double val3 = brightness * l[2];
                    curVertCoeff[0] += val1;
                    curVertCoeff[1] += val2;
                    curVertCoeff[2] += val3;
                }
            }
                    
            double factor = 4.0f*M_PI / (double)numSamples;
            curVertCoeff[0] = curVertCoeff[0] * factor;
            curVertCoeff[1] = curVertCoeff[1] * factor;
            curVertCoeff[2] = curVertCoeff[2] * factor;
                    
            for (int l=0; l<3; l++)
                vertCoeff[i][l] = curVertCoeff[l];
                    
        }
        writeToFile(c, (int)positions.size());
    }
    else
    {
        readFile(c, (int) positions.size());
    }
}

void MonteCarlo::writeToFile(const char *c, int coeffSize)
{
    ofstream myfile(c);
    
    if (myfile.is_open())
    {
        for (int i=0; i<coeffSize; i++)
        {
            for (int j=0; j<3; j++)
            {
                myfile << vertCoeff[i][j];
                myfile << " ";
            }
        }
        myfile.close();
        cout << " Done" << endl;
    }
    else
        cout << "Unable to open file";
}

void MonteCarlo::readFile(const char *c, int coeffSize)
{
    cout << "Loading coefficients " << c << endl;
    
    string line;
    
    vertCoeff.reserve(coeffSize);
    
    ifstream myfile(c);
    
    if (myfile.is_open())
    {
        getline (myfile,line);
        
        std::istringstream buf(line);
        std::istream_iterator<std::string> beg(buf), end;
        
        std::vector<std::string> tokens(beg, end);
        int l=0;
        for (int i=0; i<coeffSize; i++)
        {
            vec3 x;
            for (int j=0; j<3; j++){
                x[j] = atof(tokens[l].c_str());
                l++;
            }
            
            vertCoeff.push_back(x);
            
        }
        
        myfile.close();
    }
    else
        cout << "Unable to open file";
}

std::vector<glm::vec3> MonteCarlo::computeColors(int numVertices)
{
    std::vector<glm::vec3> Colors;
    Colors.resize(numVertices);
    
    for (int i=0; i<numVertices; i++)
    {
        Colors[i] = vertCoeff[i]/M_PI;
    }
    
    return Colors;
}

void MonteCarlo::setRotation(glm::mat4 rotation)
{
    this->rotation = rotation;
}

void  MonteCarlo::setLight(Light* light)
{
    this->light=light;
}

void MonteCarlo::computeShadow(glm::mat4& transformation, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, int numCoeff, std::string filename, bool shadow, KdTreeAccel *kdTree, Light *light1)
{
    cout << "Compute shadows: " << filename << endl;
    
    int numSamples = (int)samplesCoefficients.size();
    
   for (unsigned int i = 0; i < positions.size(); i++)
        {
            // for each sample
            for (int j = 0; j < numSamples; j++)
            {
                // dot product of normal and sample
                double brightness;
                
                vec3 dir2(samplesCoefficients[j].vectorCoord.x, samplesCoefficients[j].vectorCoord.y, samplesCoefficients[j].vectorCoord.z);
                
                double H = glm::dot(dir2, normals[i]);
                
                int x = 0;
                
                bool V;
                
                if (H > 0){
                    
                    const Ray r(Point3(positions[i][0], positions[i][1], positions[i][2]), Vector(dir2[0], dir2[1], dir2[2]));
                    V = kdTree->IntersectP(r);
                    
                    if (V == false)
                        x = 1;
                }
                
                if (H > 0 && V == false)
                {
                    bright1.push_back(true);
                    brightness = H * x;
                    bright.push_back(brightness);
                }
                else
                {
                    bright1.push_back(false);
                }
            }
        }
}

void MonteCarlo::coefficients(glm::mat4& transformation, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, int numCoeff, std::string filename, bool shadow, KdTreeAccel *kdTree, glm::mat4 lightRotation, Light *light1)
{
    int brightnessInd = 0;
    int brightnessInd1 = 0;
    
    cout << "Compute coefficients: " << filename << endl;
    vertCoeff.resize(positions.size());
    
    int numSamples = (int)samplesCoefficients.size();
    
        for (unsigned int i = 0; i < positions.size(); i++)
        {
            glm::vec3 curVertCoeff;
            
            curVertCoeff[0] = 0.0;
            curVertCoeff[1] = 0.0;
            curVertCoeff[2] = 0.0;
            
            // for each sample
            for (int j = 0; j < numSamples; j++)
            {
                if (bright1[brightnessInd]){
                    // dot product of normal and sample
                    double brightness = bright[brightnessInd1];
                    brightnessInd1++;
                    
                    vec4 dir(samplesCoefficients[j].vectorCoord.x, samplesCoefficients[j].vectorCoord.y, samplesCoefficients[j].vectorCoord.z,1 );
                    
                    SHSample samplesCoefficient;
                    
                    vec4 dir2 = lightRotation*dir;
                    samplesCoefficient.vectorCoord.x = dir2[0];
                    samplesCoefficient.vectorCoord.y = dir2[1];
                    samplesCoefficient.vectorCoord.z = dir2[2];
                    
                    vec3 light = light1->getLight(samplesCoefficient);
                    
                    double val1 = brightness * light[0];
                    double val2 = brightness * light[1];
                    double val3 = brightness * light[2];
                    curVertCoeff[0] += val1;
                    curVertCoeff[1] += val2;
                    curVertCoeff[2] += val3;
                }
                brightnessInd++;
            }
            
            double factor = 4.0f*M_PI / (double)numSamples;
            curVertCoeff[0] = curVertCoeff[0] * factor;
            curVertCoeff[1] = curVertCoeff[1] * factor;
            curVertCoeff[2] = curVertCoeff[2] * factor;
            
			for (int l = 0; l < 3; l++)
				vertCoeff[i][l] = curVertCoeff[l];
            
        }
}

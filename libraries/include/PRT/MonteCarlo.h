//
//  MonteCarlo.hpp
//  PRT
//
//  Created by Margrarita Papaefthimiou on 05/07/2017.
//
//

#ifndef MonteCarlo_hpp
#define MonteCarlo_hpp

#include <stdio.h>
#include <PRT/Illumination.h>
#include <PRT/HDRLight.h>

class MonteCarlo: public Illumination
{
private:
    std::vector<vec3> vertCoeff;
    
public:
    std::vector<double> bright;
    std::vector<bool> bright1;
    std::vector<double> shadow1;
    
    Light* light;
    glm::mat4 rotation;
    
    void coefficients(glm::mat4& coefficients, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, std::string name, bool shadow, KdTreeAccel *kdTree);
    
    void writeToFile(const char *, int coeffSize);
    
    void readFile(const char *c, int coeffSize);
    
    virtual double computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient, KdTreeAccel *kdTree) = 0;
    
    std::vector<glm::vec3> computeColors(int numVertices);
    
    void setRotation(glm::mat4 rotation);
    void setLight(Light* light);
    
    void computeShadow(glm::mat4& transformation, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, int numCoeff, std::string filename, bool shadow, KdTreeAccel *kdTree, Light *light1);
    
    void coefficients(glm::mat4& transformation, vector<glm::vec3> normals, vector<glm::vec3> positions, vector<SHSample> samplesCoefficients, int numCoeff, std::string filename, bool shadow, KdTreeAccel *kdTree, glm::mat4 lightRotation, Light *light1);
};
#endif /* MonteCarlo_h */

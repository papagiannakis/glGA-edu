//
//  Shadow.h
//  MetaioOpenGLES
//
//  Created by margarita on 3/16/15.
//  Copyright (c) 2015 Margarita. All rights reserved.
//

#include "PRT/Shadow.h"

extern double Hs;
extern bool Ray_hit;

double Shadow::computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient)
{
    vec3 dir(samplesCoefficient.vectorCoord.x, samplesCoefficient.vectorCoord.y, samplesCoefficient.vectorCoord.z);
    
    double H = glm::dot(dir, normal);
    
    return H;
};

double Shadow::computeBrightness(vec3 normal, vec3 position, SHSample samplesCoefficient, KdTreeAccel *kdTree)
{
    vec3 dir(samplesCoefficient.vectorCoord.x, samplesCoefficient.vectorCoord.y, samplesCoefficient.vectorCoord.z);
    
    double H = glm::dot(dir, normal);
	
	int x = 0;

	Hs = H;

	if (H > 0){

		const Ray r(Point3(position[0], position[1], position[2]), Vector(dir[0], dir[1], dir[2]));
		bool V = kdTree->IntersectP(r);
		Ray_hit = V;

		if (V == false)
			x = 1;
	}
	
    return H * x;
};

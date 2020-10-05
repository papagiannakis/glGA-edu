//
//  Primitive.h
//  MetaioOpenGLES
//
//  Created by margarita on 3/14/15.
//  Copyright (c) 2015 Lisa. All rights reserved.
//

#ifndef __MetaioOpenGLES__Primitive__
#define __MetaioOpenGLES__Primitive__

#include <stdio.h>
#include "geometry.h"
#include "memory.h"

class PrimitiveTriangle : public ReferenceCounted
{
private:
    Point3 p1;
    Point3 p2;
    Point3 p3;
    
public:
    
	Point3 point_1;
	Point3 point_2;
	Point3 point_3;
	int	   i1;
	int    i2;
	int    i3;
    PrimitiveTriangle(Point3 &point1, Point3 &point2, Point3 &point3);
    BBox WorldBound();
    bool IntersectP(const Ray &ray)const;
    bool Intersect(const Ray &ray, float *tHit, float *rayEpsilon) const;
	
};

#endif /* defined(__MetaioOpenGLES__Primitive__) */
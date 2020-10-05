//
//  Primitive.cpp
//  MetaioOpenGLES
//
//  Created by margarita on 3/14/15.
//  Copyright (c) 2015 Lisa. All rights reserved.
//

#include "KDTree/PrimitiveTriangle.h"
extern float u;
extern float v;
extern int index1;
extern int index2;
extern int index3;

PrimitiveTriangle::PrimitiveTriangle(Point3 &point1, Point3 &point2, Point3 &point3)
{
    p1 = point1;
    p2 = point2;
    p3 = point3;
	i1 = index1;
	i2 = index2;
	i3 = index3;
	point_1 = point1;
	point_2 = point2;
	point_3 = point3;
}

BBox PrimitiveTriangle::WorldBound()
{
    return Union(BBox(p1, p2), p3);
}


bool PrimitiveTriangle::Intersect(const Ray &ray, float *tHit, float *rayEpsilon) const {
    // Compute $\VEC{s}_1$
    
    // Get triangle vertices in _p1_, _p2_, and _p3_
    Vector e1 = p2 - p1;
    Vector e2 = p3 - p1;
    Vector s1 = Cross(ray.d, e2);
    float divisor = Dot(s1, e1);
    
    if (divisor == 0.)
        return false;
    float invDivisor = 1.f / divisor;
    
    // Compute first barycentric coordinate
    Vector s = ray.o - p1;
    float b1 = Dot(s, s1) * invDivisor;
    if (b1 < 0. || b1 > 1.)
        return false;
    
    // Compute second barycentric coordinate
    Vector s2 = Cross(s, e1);
    float b2 = Dot(ray.d, s2) * invDivisor;
    if (b2 < 0. || b1 + b2 > 1.)
        return false;
    
    // Compute _t_ to intersection point
    float t = Dot(e2, s2) * invDivisor;
    if (t < ray.mint || t > ray.maxt)
        return false;
    
    // Compute triangle partial derivatives
    Vector dpdu, dpdv;
    /*float uvs[3][2];
    GetUVs(uvs);
    
    // Compute deltas for triangle partial derivatives
    float du1 = uvs[0][0] - uvs[2][0];
    float du2 = uvs[1][0] - uvs[2][0];
    float dv1 = uvs[0][1] - uvs[2][1];
    float dv2 = uvs[1][1] - uvs[2][1];
    Vector dp1 = p1 - p3, dp2 = p2 - p3;
    float determinant = du1 * dv2 - dv1 * du2;
    if (determinant == 0.f) {
        // Handle zero determinant for triangle partial derivative matrix
        CoordinateSystem(Normalize(Cross(e2, e1)), &dpdu, &dpdv);
    }
    else {
        float invdet = 1.f / determinant;
        dpdu = ( dv2 * dp1 - dv1 * dp2) * invdet;
        dpdv = (-du2 * dp1 + du1 * dp2) * invdet;
    }
    
    // Interpolate $(u,v)$ triangle parametric coordinates
    float b0 = 1 - b1 - b2;
    float tu = b0*uvs[0][0] + b1*uvs[1][0] + b2*uvs[2][0];
    float tv = b0*uvs[0][1] + b1*uvs[1][1] + b2*uvs[2][1];
    
    // Test intersection against alpha texture, if present
    /*if (ray.depth != -1) {
        if (mesh->alphaTexture) {
            DifferentialGeometry dgLocal(ray(t), dpdu, dpdv,
                                         Normal(0,0,0), Normal(0,0,0),
                                         tu, tv, this);
            if (mesh->alphaTexture->Evaluate(dgLocal) == 0.f)
                return false;
        }
    }
    
    // Fill in _DifferentialGeometry_ from triangle hit
    *dg = DifferentialGeometry(ray(t), dpdu, dpdv,
                               Normal(0,0,0), Normal(0,0,0),
                               tu, tv, this);*/
    *tHit = t;
    *rayEpsilon = 1e-3f * *tHit;
    
    return true;
}


bool PrimitiveTriangle::IntersectP(const Ray &ray)const{
    
    // Compute $\VEC{s}_1$
    
    if ((ray.o.x == p1.x && ray.o.y == p1.y && ray.o.z == p1.z) ||
        (ray.o.x == p2.x && ray.o.y == p2.y && ray.o.z == p2.z) ||
        (ray.o.x == p3.x && ray.o.y == p3.y && ray.o.z == p3.z))
    {
        return false;
    }
        
    // Get triangle vertices in _p1_, _p2_, and _p3_
    Vector e1 = p2 - p1;
    Vector e2 = p3 - p1;
    Vector s1 = Cross(ray.d, e2);
    float divisor = Dot(s1, e1);
    
    if (divisor == 0.)
        return false;
    float invDivisor = 1.f / divisor;
    
    // Compute first barycentric coordinate
    Vector d = ray.o - p1;
    float b1 = Dot(d, s1) * invDivisor;
	if (b1 < 0. || b1 > 1.){
		u = 0.0;
		v = 0.0;
		return false;
	}
    
    // Compute second barycentric coordinate
    Vector s2 = Cross(d, e1);
    float b2 = Dot(ray.d, s2) * invDivisor;
	if (b2 < 0. || b1 + b2 > 1.){
		u = 0.0;
		v = 0.0;
		return false;
	}
    
    if (b1 == 0 && b2 ==0)
        return false;
    if (b1 == 1 && b2 ==0)
        return false;
    if (b1 == 0 && b2 ==1)
        return false;
    
    // Compute _t_ to intersection point
    float t = Dot(e2, s2) * invDivisor;
	if (t < ray.mint || t > ray.maxt){
		u = 0.0;
		v = 0.0;
		return false;
	}
    
    // Test shadow ray intersection against alpha texture, if present
    /*if (ray.depth != -1 && mesh->alphaTexture) {
        // Compute triangle partial derivatives
        Vector dpdu, dpdv;
        float uvs[3][2];
        GetUVs(uvs);
        
        // Compute deltas for triangle partial derivatives
        float du1 = uvs[0][0] - uvs[2][0];
        float du2 = uvs[1][0] - uvs[2][0];
        float dv1 = uvs[0][1] - uvs[2][1];
        float dv2 = uvs[1][1] - uvs[2][1];
        Vector dp1 = p1 - p3, dp2 = p2 - p3;
        float determinant = du1 * dv2 - dv1 * du2;
        if (determinant == 0.f) {
            // Handle zero determinant for triangle partial derivative matrix
            CoordinateSystem(Normalize(Cross(e2, e1)), &dpdu, &dpdv);
        }
        else {
            float invdet = 1.f / determinant;
            dpdu = ( dv2 * dp1 - dv1 * dp2) * invdet;
            dpdv = (-du2 * dp1 + du1 * dp2) * invdet;
        }
        
        // Interpolate $(u,v)$ triangle parametric coordinates
        float b0 = 1 - b1 - b2;
        float tu = b0*uvs[0][0] + b1*uvs[1][0] + b2*uvs[2][0];
        float tv = b0*uvs[0][1] + b1*uvs[1][1] + b2*uvs[2][1];
        DifferentialGeometry dgLocal(ray(t), dpdu, dpdv,
                                     Normal(0,0,0), Normal(0,0,0),
                                     tu, tv, this);
        if (mesh->alphaTexture->Evaluate(dgLocal) == 0.f)
            return false;
    }*/

	//Barycentric coordinates
	u = b1;
	v = b2;

	return true;
}

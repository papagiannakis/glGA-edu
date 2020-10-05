//
//  SHRotation.h
//  PRT
//
//  Created by Margrarita Papaefthimiou on 11/10/15.
//  Copyright (c) 2015 George Papagiannakis. All rights reserved.
//

#ifndef __PRT__SHRotation__
#define __PRT__SHRotation__

#include <stdio.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <vector>
#include <iostream>

class SHRotateMatrix
{
public:
    SHRotateMatrix()
    {
        e = new float*[DIM];
        for(int i=0; i<DIM; i++)
            e[i]=new float[DIM];
    }
    
    static int DIM;
    static int NB_BANDS;
    
    //the matrix array
    float **e;
    
    //SH rotation matrices are block diagonal sparse [Green]
    //given a band index l, this will offset the lookup into the block within
    //the matrix that is responsible for rotating that band and shift it by (a,b)
    float & operator () (const int l, const int a, const int b)
    {
        int centre = (l + 1) * l;
        return (e[centre + a][centre + b]);
    }//end operator ()
    
    float operator () (const int l, const int a, const int b) const
    {
        int centre = (l + 1) * l;
        return (e[centre + a][centre + b]);
    }
    
    glm::vec3* transform(glm::vec3* source);
    //void transform(const RGBCoeffList& source, RGBCoeffList& dest) const;
};//end class SHRotateMatrix


class PermutedMatrix
{
public:
    PermutedMatrix(const glm::mat4& m) :_matrix(m){};
    
    static int permute(const int v)
    {
        if(v == 1) return 0;
        if(v == -1) return 1;
        if (v == 0) return 2;
        assert(false);
        return 0;
    }
    
    float operator () (const int m, const int n) const
    {
        // m = row, n = column
        return _matrix[permute(n)][permute(m)];
    }
    
private:
    const glm::mat4& _matrix;
};

class SHRotation
{
public:
    /* Kronecker delta*/
    float delta(const int m, const int n)
    {
        return (m == n ? 1 : 0);
    }
    
    void uvw(const int l, const int m, const int n, float& u, float& v, float& w);
    
    float pIndex(const int i, const int l, const int a, const int b, const PermutedMatrix& r, const SHRotateMatrix& m);
    
    float uIndex(const int l, const int m, const int n, const PermutedMatrix& rx, const SHRotateMatrix& mx);
    
    float vIndex(const int l, const int m, const int n, const PermutedMatrix& rx, const SHRotateMatrix& mx);
    
    float wIndex(const int l, const int m, const int n, const PermutedMatrix& rx, const SHRotateMatrix& mx);
    
    float mIndex(const int l, const int m, const int n, const PermutedMatrix& rx, const SHRotateMatrix& mx);
    
    void shRotate(SHRotateMatrix& shrm, const glm::mat4& rotation);
};
#endif /* defined(__PRT__SHRotation__) */

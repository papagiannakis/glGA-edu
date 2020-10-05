//
//  SHRotation.cpp
//  PRT
//
//  Created by Margrarita Papaefthimiou on 11/10/15.
//  Copyright (c) 2015 George Papagiannakis. All rights reserved.
//

#include "PRT/SHRotation.h"

int SHRotateMatrix::DIM=0;
int SHRotateMatrix::NB_BANDS=0;

glm::vec3* SHRotateMatrix::transform(glm::vec3* source)
{
    glm::vec3*  dest = new glm::vec3[NB_BANDS*NB_BANDS];
    //Loop through each band l
    for (int l = 0; l < NB_BANDS; l++)
    {
        for (int mo = -l; mo <= l; mo++ )
        {
            dest[l*(l+1)+mo][0] = dest[l*(l+1)+mo][1] = dest[l*(l+1)+mo][2] = 0;
            
            for (int mi = -l; mi <=l; mi++ )
            {
                glm::vec3 a = source[l*(l+1)+mo];
                float b = (*this)(l, mo, mi);
                dest[l*(l+1)+mo] +=  a * b;
            }
        }
    }
    return dest;
}

void SHRotation::uvw(const int l, const int m, const int n, float& u, float& v, float& w)
{
    // Pre-calculate simple reusable terms
    float d = delta(m, 0);
    int abs_m = abs(m);
    
    // Only calculate the required denominator once
    float denom;
    if (abs(n) == l)
        denom = (2 * l) * (2 * l - 1);
    
    else
        denom = (l + n) * (l - n);
    
    // Now just calculate the scalars
    u = sqrt((l + m) * (l - m) / denom);
    v = 0.5f * sqrt((1 + d) * (l + abs_m - 1) * (l + abs_m) / denom) * (1 - 2 * d);
    w = -0.5f * sqrt((l - abs_m - 1) * (l - abs_m) / denom) * (1 - d);
}//end uvw()

float SHRotation::pIndex(const int i, const int l, const int a, const int b, const PermutedMatrix& rx, const SHRotateMatrix& mx)
{
    if (b == -l)
        return (rx(i, 1) * mx(l - 1, a, -l + 1) + rx(i, -1) * mx(l - 1, a, l - 1));
    
    else if (b == l)
        return (rx(i, 1) * mx(l - 1, a, l - 1) - rx(i, -1) * mx(l - 1, a, -l + 1));
    
    else // |b|<l
        return (rx(i, 0) * mx(l - 1, a, b));
}//end p()

float SHRotation::uIndex(const int l, const int m, const int n, const PermutedMatrix& rx, const SHRotateMatrix& mx)
{
    if (m == 0)
        return (pIndex(0, l, 0, n, rx, mx));
    
    // For both m>0, m<0
    return (pIndex(0, l, m, n, rx, mx));
}//end u()

float SHRotation::vIndex(const int l, const int m, const int n, const PermutedMatrix& rx, const SHRotateMatrix& mx)
{
    if (m == 0)
    {
        float p0 = pIndex(1, l, 1, n, rx, mx);
        float p1 = pIndex(-1, l, -1, n, rx, mx);
        return (p0 + p1);
    }
    
    else if (m > 0)
    {
        float d = delta(m, 1);
        float p0 = pIndex(1, l, m - 1, n, rx, mx);
        float p1 = pIndex(-1, l, -m + 1, n, rx, mx);
        return (p0 * sqrt(1 + d) - p1 * (1 - d));
    }
    
    else // m < 0
    {
        float d = delta(m, -1);
        float p0 = pIndex(1, l, m + 1, n, rx, mx);
        float p1 = pIndex(-1, l, -m - 1, n, rx, mx);
        return (p0 * (1 - d) + p1 * sqrt(1 - d));
    }
}//end v()

float SHRotation::wIndex(const int l, const int m, const int n, const PermutedMatrix& rx, const SHRotateMatrix& mx)
{
    if (m == 0)
    {
        // Never gets called as kd=0
        assert(false);
        return (0);
    }
    
    else if (m > 0)
    {
        float p0 = pIndex(1, l, m + 1, n, rx, mx);
        float p1 = pIndex(-1, l, -m - 1, n, rx, mx);
        return (p0 + p1);
    }
    
    else // m < 0
    {
        float p0 = pIndex(1, l, m - 1, n, rx, mx);
        float p1 = pIndex(-1, l, -m + 1, n, rx, mx);
        return (p0 - p1);
    }
}//end w()

float SHRotation::mIndex(const int l, const int m, const int n, const PermutedMatrix& rx, const SHRotateMatrix& mx)
{
    // First get the scalars
    float u1, v1, w1;
    uvw(l, m, n, u1, v1, w1);
    
    // Scale by their functions
    if (u1)
        u1 *= uIndex(l, m, n, rx, mx);
    if (v1)
        v1 *= vIndex(l, m, n, rx, mx);
    if (w1)
        w1 *= wIndex(l, m, n, rx, mx);
    
    return (u1 + v1 + w1);
}//end m()

void SHRotation::shRotate(SHRotateMatrix& shrm, const glm::mat4& rotation)
{
    // Start with already known 1x1 rotation matrix for band zero
    //memset(shrm.e, 0, shrm.DIM * shrm.DIM * sizeof(float));
    shrm.e[0][0] = 1;
    
    // Create matrix index modifiers
    PermutedMatrix pm(rotation);
    
    for (int m = -1; m <= 1; m++)
        for (int n = -1; n <= 1; n++)
            shrm(1, m, n) = pm(m, n);
    
    // Calculate each block of the rotation matrix for each band
    for (int band = 2; band < SHRotateMatrix::NB_BANDS; band++)
    {
        for (int m = -band; m <= band; m++)
            for (int n = -band; n <= band; n++)
                shrm(band, m, n) = mIndex(band, m, n, pm, shrm);
    }
}

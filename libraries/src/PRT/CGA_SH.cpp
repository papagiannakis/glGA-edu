//
//  CGA_SH.cpp
//  glGA
//
//  Created by Margrarita Papaefthimiou on 22/10/16.
//  Copyright © 2016 George Papagiannakis. All rights reserved.
//

#include "CGA_SH.hpp"
#include <stdio.h>
#include <iostream>

using namespace std;

float computeSH10(float x, float y, float z)
{
    float a2[32]; float distance10[32]; float line2[32]; float points2[32];
    
    line2[6] = z; // e1 ^ e2
    line2[7] = (-y); // e1 ^ e3
    line2[10] = x; // e2 ^ e3
    
    if (z>0){
        points2[16] = line2[6] / 2.0; // e1 ^ (e2 ^ e3)
        points2[18] = line2[6]; // e1 ^ (e2 ^ e0)
        points2[20] = line2[7]; // e1 ^ (e3 ^ e0)
        points2[23] = line2[10]; // e2 ^ (e3 ^ e0)
        float val2=points2[23] * points2[23] + (-points2[20]) * (-points2[20]) + points2[18] * points2[18];
        float val1=sqrtf(sqrtf(fabs(points2[16] * points2[16] * points2[16] * points2[16])));
        a2[1] = val1 * points2[23] / (val2) + (-((-points2[23]) * points2[16] / (val2))); // e1
        a2[2] = val1 * (-points2[20]) / (val2) + (-(points2[20] * points2[16] / (val2))); // e2
        a2[3] = val1 * points2[18] / (val2) + (-((-points2[18]) * points2[16] / (val2))); // e3
        distance10[0] = sqrtf(a2[1] * a2[1] + a2[2] * a2[2] + a2[3] * a2[3]); // 1.0
    }
    else{
        points2[16] = line2[6] * -0.5; // e1 ^ (e2 ^ e3)
        points2[18] = line2[6]; // e1 ^ (e2 ^ e0)
        points2[20] = line2[7]; // e1 ^ (e3 ^ e0)
        points2[23] = line2[10]; // e2 ^ (e3 ^ e0)
        float val2=points2[23] * points2[23] + (-points2[20]) * (-points2[20]) + points2[18] * points2[18];
        float val1=sqrtf(sqrtf(fabs(points2[16] * points2[16] * points2[16] * points2[16])));
        a2[1] = val1 * points2[23] / (val2) + (-((-points2[23]) * points2[16] / (val2))); // e1
        a2[2] = val1 * (-points2[20]) / (val2) + (-(points2[20] * points2[16] / (val2))); // e2
        a2[3] = val1 * points2[18] / (val2) + (-((-points2[18]) * points2[16] / (val2))); // e3
        
        distance10[0] = -(sqrtf(a2[1] * a2[1] + a2[2] * a2[2] + a2[3] * a2[3])); // 1.0
    }
    return distance10[0];
}

float computeSH1_1(float x, float y, float z)
{float a1[32]; float distance1_1[32]; float Earth11[32]; float Earth21[32]; float line1[32]; float p1[32]; float points1[32];
    p1[5] = 1.0; // e0
    line1[6] = z; // e1 ^ e2
    line1[7] = (-y); // e1 ^ e3
    line1[10] = x; // e2 ^ e3
    Earth11[2] = 0.5; // e2
    Earth11[5] = 1.0; // e0
    Earth21[2] = -0.5; // e2
    Earth21[5] = 1.0; // e0
    if (y<0){
        points1[16] = (-(line1[7] / 2.0)); // e1 ^ (e2 ^ e3)
        points1[18] = line1[6]; // e1 ^ (e2 ^ e0)
        points1[20] = line1[7]; // e1 ^ (e3 ^ e0)
        points1[23] = line1[10]; // e2 ^ (e3 ^ e0)
        float val1=points1[23] * points1[23] + (-points1[20]) * (-points1[20]) + points1[18] * points1[18];
        float val2=sqrtf(sqrtf(fabs(points1[16] * points1[16] * points1[16] * points1[16])));
        a1[1] = val2 * points1[23] / (val1) + (-(points1[23] * points1[16] / (val1))); // e1
        a1[2] = val2 * (-points1[20]) / (val1) + (-((-points1[20]) * points1[16] / (val1))); // e2
        a1[3] = val2 * points1[18] / (val1) + (-(points1[18] * points1[16] / (val1))); // e3
        
        distance1_1[0] = (sqrtf(a1[1] * a1[1] + a1[2] * a1[2] + a1[3] * a1[3])); // 1.0
    }
    else
    {
        points1[16] = (-(line1[7] * -0.5)); // e1 ^ (e2 ^ e3)
        points1[18] = line1[6]; // e1 ^ (e2 ^ e0)
        points1[20] = line1[7]; // e1 ^ (e3 ^ e0)
        points1[23] = line1[10]; // e2 ^ (e3 ^ e0)
        float val1=points1[23] * points1[23] + (-points1[20]) * (-points1[20]) + points1[18] * points1[18];
        float val2=sqrtf(sqrtf(fabs(points1[16] * points1[16] * points1[16] * points1[16])));
        
        a1[1] = val2 * points1[23] / (val1) + (-(points1[23] * points1[16] / (val1))); // e1
        a1[2] = val2 * (-points1[20]) / (val1) + (-((-points1[20]) * points1[16] / (val1))); // e2
        a1[3] = val2 * points1[18] / (val1) + (-(points1[18] * points1[16] / (val1))); // e3
        distance1_1[0] = -sqrtf(a1[1] * a1[1] + a1[2] * a1[2] + a1[3] * a1[3]); // 1.0
    }
    
    return distance1_1[0];
}

float computeSH11(float x, float y, float z){
    float a3[32]; float distance11[32]; double line3[32]; float p1[32]; float points3[32];
    
    p1[5] = 1.0; // e0
    line3[6] = z; // e1 ^ e2
    line3[7] = (-y); // e1 ^ e3
    line3[10] = x; // e2 ^ e3
    
    if (x<0){
        points3[16] = line3[10] * -0.5; // e1 ^ (e2 ^ e3)
        points3[18] = line3[6]; // e1 ^ (e2 ^ e0)
        points3[20] = line3[7]; // e1 ^ (e3 ^ e0)
        points3[23] = line3[10]; // e2 ^ (e3 ^ e0)
        float val1=points3[23] * points3[23] + (-points3[20]) * (-points3[20]) + points3[18] * points3[18];
        float val2=sqrtf(sqrtf(fabs(points3[16] * points3[16] * points3[16] * points3[16])));
        a3[1] = val2 * points3[23] / (val1) + (-((-points3[23]) * points3[16] / (val1))); // e1
        a3[2] = val2 * (-points3[20]) / (val1) + (-(points3[20] * points3[16] / (val1))); // e2
        a3[3] = val2 * points3[18] / (val1) + (-((-points3[18]) * points3[16] / (val1))); // e3
        
        distance11[0] = sqrtf(a3[1] * a3[1] + a3[2] * a3[2] + a3[3] * a3[3]); // 1.0
    }
    else
    {
        points3[16] = line3[10] / 2.0; // e1 ^ (e2 ^ e3)
        points3[18] = line3[6]; // e1 ^ (e2 ^ e0)
        points3[20] = line3[7]; // e1 ^ (e3 ^ e0)
        points3[23] = line3[10]; // e2 ^ (e3 ^ e0)
        float val1=points3[23] * points3[23] + (-points3[20]) * (-points3[20]) + points3[18] * points3[18];
        float val2=sqrtf(sqrtf(fabs(points3[16] * points3[16] * points3[16] * points3[16])));
        a3[1] = val2 * points3[23] / (val1) + (-((-points3[23]) * points3[16] / (val1))); // e1
        a3[2] = val2 * (-points3[20]) / (val1) + (-(points3[20] * points3[16] / (val1))); // e2
        a3[3] = val2 * points3[18] / (val1) + (-((-points3[18]) * points3[16] / (val1))); // e3
        
        distance11[0] = -(sqrtf(a3[1] * a3[1] + a3[2] * a3[2] + a3[3] * a3[3])); // 1.0
    }
    
    return distance11[0];
}

float computeSH2_2(float x, float y, float z)
{
    return computeSH11(x,y,z)*computeSH1_1(x,y,z);
}


float computeSH2_1(float x, float y, float z)
{
    return computeSH10(x,y,z)*computeSH1_1( x,y,z);
}

float computeSH20(float x, float y, float z)
{
    float val=computeSH10(x,y,z);
    return (3.0f*val*val)-1.0;
}

float computeSH21(float x1, float y1, float z1)
{
    return computeSH11(x1,y1,z1)*computeSH10( x1,y1,z1);
}

float computeSH22(float x1, float y1, float z1)
{
    float val1=computeSH11(x1,y1,z1);
    float val2=computeSH1_1(x1,y1,z1);
    return (val1*val1-val2*val2);
}

float computeSH3_3(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float val=(3*x*x-y*y)*y;
    return val;
}

float computeSH3_2(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=x*y*z;
    return val;
}

float computeSH3_1(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=y*(4*z*z-x*x-y*y);
    return val;
}

float computeSH30(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=z*(2*z*z-3*x*x-3*y*y);
    return val;
}

float computeSH31(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=x*(4*z*z-x*x-y*y);
    return val;
}

float computeSH4_4(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float val=(x*y*(x*x-y*y));
    return val;
}

float computeSH4_3(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=(3*x*x-y*y)*y*z;
    return val;
}

float computeSH4_2(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=x*y*(7*z*z-1);
    return val;
}

float computeSH4_1(float x1, float y1, float z1)
{
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=(y*z*(7*z*z-3));
    return val;
}

float computeSH40(float x1, float y1, float z1)
{
    float z=computeSH10(x1,y1,z1);
    float val=(35.0f*z*z*z*z-30*z*z+3);
    return val;
}

float computeSH41(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=(x*z*(7*z*z-3));
    return val;
}

float computeSH42(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=((x*x-y*y)*(7*z*z-1));
    return val;
}

float computeSH43(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=(x*x-3*y*y)*x*z;
    return val;
}
float computeSH44(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float val=(x*x*(x*x-3*y*y)-y*y*(3*x*x-y*y));
    return val;
}

float computeSH5_5(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float val=y*(5*x*x*x*x-10.0f*y*y*x*x+y*y*y*y);
    return val;
}

float computeSH5_4(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=y*x*(x*x-y*y)*z;
    return val;
}

float computeSH5_3(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=y*(3*x*x-y*y)*(-1+9*z*z);
    return val;
}

float computeSH5_2(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=y*x*z*(-1+3*z*z);
    return val;
}

float computeSH5_1(float x1, float y1, float z1)
{
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=y*(-14*z*z+21*z*z*z*z+1);
    return val;
}

float computeSH50(float x1, float y1, float z1)
{
    float z=computeSH10(x1,y1,z1);
    float val=z*(63*z*z*z*z-70*z*z+15);
    return val;
}

float computeSH51(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=x*(-14*z*z+21*z*z*z*z+1);
    return val;
}

float computeSH52(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=(x*x-y*y)*z*(-1+3*z*z);
    return val;
}

float computeSH53(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=x*(x*x-3*y*y)*(-1+9*z*z);
    return val;
}

float computeSH54(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=(x*x*x*x-6*y*y*x*x+y*y*y*y)*z;
    return val;
}

float computeSH55(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float val=x*(x*x*x*x-10*y*y*x*x+5*y*y*y*y);
    return val;
}

float computeSH32(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float z=computeSH10(x1,y1,z1);
    float val=(x*x-y*y)*z;
    return val;
}

float computeSH33(float x1, float y1, float z1)
{
    float x=computeSH11(x1,y1,z1);
    float y=computeSH1_1(x1,y1,z1);
    float val=(x*x-3*y*y)*x;
    return val;
}

float computeSH(int l, int m, float x, float y, float z)
{
    if (l==0 && m==0)
        return sqrt(1/(4*M_PI));
    if (l==1 && m==1)
        return sqrt(3/(4*M_PI))*computeSH11(x, y, z);
    else if (l==1 && m==0)
        return sqrt(3/(4*M_PI))*computeSH10(x, y, z);
    else if (l==1 && m==-1)
        return sqrt(3/(4*M_PI))*computeSH1_1(x, y, z);
    else if (l==2 && m==-2)
        return sqrt(15/(4*M_PI))*computeSH2_2(x, y, z);
    else if (l==2 && m==-1)
        return sqrt(15/(4*M_PI))*computeSH2_1(x, y, z);
    else if (l==2 && m==0)
        return sqrt(5/(16*M_PI))*computeSH20(x, y, z);
    else if (l==2 && m==1)
        return sqrt(15/(4*M_PI))*computeSH21(x, y, z);
    else if (l==2 && m==2)
        return sqrt(15/(16*M_PI))*computeSH22(x, y, z);
    
    else if (l==3 && m==-3)
        return (1/4.0f)*sqrt(35/(2*M_PI))*computeSH3_3(x, y, z);
    else if (l==3 && m==-2)
        return (1/2.0f)*sqrt(105.0f/(M_PI))*computeSH3_2(x, y, z);
    else if (l==3 && m==-1)
        return (1/4.0f)*sqrt(21/(2*M_PI))*computeSH3_1(x, y, z);
    else if (l==3 && m==0)
        return (1/4.0f)*sqrt(7/(M_PI))*computeSH30(x, y, z);
    else if (l==3 && m==1)
        return (1/4.0f)*sqrt(21/(2*M_PI))*computeSH31(x, y, z);
    else if (l==3 && m==2)
        return (1/4.0f)*sqrt(105/(M_PI))*computeSH32(x, y, z);
    else if (l==3 && m==3)
        return (1/4.0f)*sqrt(35/(2*M_PI))*computeSH33(x, y, z);
    
    else if (l==4 && m==-4)
        return (3.0f/4.0f)*sqrt(35.0f/(M_PI))*computeSH4_4(x, y, z);
    else if (l==4 && m==-3)
        return (3.0f/4.0f)*sqrt(35.0f/(2*M_PI))*computeSH4_3(x, y, z);
    else if (l==4 && m==-2)
        return (3.0f/4.0f)*sqrt(5.0f/(M_PI))*computeSH4_2(x, y, z);
    else if (l==4 && m==-1)
        return (3.0f/4.0f)*sqrt(5.0f/(2*M_PI))*computeSH4_1(x, y, z);
    else if (l==4 && m==0)
        return (3.0f/16.0f)*sqrt(1.0f/(M_PI))*computeSH40(x, y, z);
    else if (l==4 && m==1)
        return (3.0f/4.0f)*sqrt(5.0f/(2*M_PI))*computeSH41(x, y, z);
    else if (l==4 && m==2)
        return (3.0f/8.0f)*sqrt(5.0f/(M_PI))*computeSH42(x, y, z);
    else if (l==4 && m==3)
        return (3.0f/4.0f)*sqrt(35.0f/(2*M_PI))*computeSH43(x, y, z);
    else if (l==4 && m==4)
        return (3.0f/16.0f)*sqrt(35.0f/(M_PI))*computeSH44(x, y, z);
    
    else if (l==5 && m==-5)
        return ((3.0f*sqrt(2.0f)*sqrt(77.0f))/(32.0f*sqrt(M_PI)))*computeSH5_5(x, y, z);
    else if (l==5 && m==-4)
        return ((3.0f*sqrt(385.0f))/(4.0f*sqrt(M_PI)))*computeSH5_4(x, y, z);
    else if (l==5 && m==-3)
        return ((sqrt(2.0f)*sqrt(385.0f))/(32.0f*sqrt(M_PI)))*computeSH5_3(x, y, z);
    else if (l==5 && m==-2)
        return (sqrt(1155.0f)/(4.0f*sqrt(M_PI)))*computeSH5_2(x, y, z);
    else if (l==5 && m==-1)
        return (sqrt(165.0f)/(16.0f*sqrt(M_PI)))*computeSH5_1(x, y, z);
    else if (l==5 && m==0)
        return (sqrt(11.0f)/(16.0f*sqrt(M_PI)))*computeSH50(x, y, z);
    else if (l==5 && m==1)
        return (sqrt(165.0f)/(16.0f*sqrt(M_PI)))*computeSH51(x, y, z);
    else if (l==5 && m==2)
        return (sqrt(1155.0f)/(8.0f*sqrt(M_PI)))*computeSH52(x, y, z);
    else if (l==5 && m==3)
        return ((sqrt(2.0f)*sqrt(385.0f))/(32.0f*sqrt(M_PI)))*computeSH53(x, y, z);
    else if (l==5 && m==4)
        return ((3.0f*sqrt(385.0f))/((16.0f)*sqrt(M_PI)))*computeSH54(x, y, z);
    else if (l==5 && m==5)
        return ((3.0f*sqrt(2.0f)*sqrt(77.0f))/(32.0f*sqrt(M_PI)))*computeSH55(x, y, z);
    return 1;
}

float computeSH1(int l, int m, float x, float y, float z)
{
    if (l==0)
        return 1;
    else if (l==1){
        if (m==-1)
            return computeSH1_1(x, y, z);
        else if (m==0)
            return computeSH10(x, y, z);
        else
            return computeSH11(x, y, z);
    }
    else if (l==2){
        if (m==-2)
            return computeSH2_2(x, y, z);
        else if (m==-1)
            return computeSH2_1(x, y, z);
        else if (m==0)
            return computeSH20(x, y, z);
        else if (m==1)
            return computeSH21(x, y, z);
        else if (m==2)
            return computeSH22(x, y, z);
    }
    else if (l==3){
        if (m==-3)
            return computeSH3_3(x, y, z);
        else if (m==-2)
            return computeSH3_2(x, y, z);
        else if (m==-1)
            return computeSH3_1(x, y, z);
        else if (m==0)
            return computeSH30(x, y, z);
        else if (m==1)
            return computeSH31(x, y, z);
        else if (m==2)
            return computeSH32(x, y, z);
        else if (m==3)
            return computeSH33(x, y, z);
    }
    else if (l==4){
        if (m==-4)
            return computeSH4_4(x, y, z);
        else if (m==-3)
            return computeSH4_3(x, y, z);
        else if (m==-2)
            return computeSH4_2(x, y, z);
        else if (m==-1)
            return computeSH4_1(x, y, z);
        else if (m==0)
            return computeSH40(x, y, z);
        else if (m==1)
            return computeSH41(x, y, z);
        else if (m==2)
            return computeSH42(x, y, z);
        else if (m==3)
            return computeSH43(x, y, z);
        else if (m==4)
            return computeSH44(x, y, z);
    }
    else if (l==5){
        if (m==-5)
            return computeSH5_5(x, y, z);
        else if (m==-4)
            return computeSH5_4(x, y, z);
        else if (m==-3)
            return computeSH5_3(x, y, z);
        else if (m==-2)
            return computeSH5_2(x, y, z);
        else if (m==-1)
            return computeSH5_1(x, y, z);
        else if (m==0)
            return computeSH50(x, y, z);
        else if (m==1)
            return computeSH51(x, y, z);
        else if (m==2)
            return computeSH52(x, y, z);
        else if (m==3)
            return computeSH53(x, y, z);
        else if (m==4)
            return computeSH54(x, y, z);
        else if (m==5)
            return computeSH55(x, y, z);
    }
    return 1;
}

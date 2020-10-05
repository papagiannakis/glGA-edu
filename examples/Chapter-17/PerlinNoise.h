//========================================================================= 
// File: PerlinNoise.h 
//========================================================================= 
/* Creates Perlin Noise rasters  
   restructured by Ioannis Aggelogiannis to fit in glGA */ 

#ifndef PerlinNoise_H
#define PerlinNoise_H

#include <iostream>
#include <cmath>
#include <GL/glew.h>

#define MAXB 0x100 
#define N 0x1000 
#define NP 12 // 2^N 
#define NM 0xfff 
#define s_curve(t) ( t * t * (3. - 2. * t) ) 
#define lerp(t, a, b) ( a + t * (b - a) ) 
#define setup(i, b0, b1, r0, r1) t = vec[i] + N; b0 = ((int)t) & BM; b1 = (b0+1) & BM; r0 = t - (int)t; r1 = r0 - 1.;

#define at3(rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2] )


class PerlinNoise{
 public:

	
	int start; 
	int B; 
	int BM;

	int noise3DTexSize;
	GLuint noise3DTexName;
	GLubyte *noise3DTexPtr;

	PerlinNoise(int,int,int);
	void SetNoiseFrequency(int);
    void normalize3(double v[3]);
    void startNoise();
	double noise3(double vec[3]);
	void make3DNoiseTexture(int,int,int);
	void init3DNoiseTexture();
	void setNoise3DTexSize(int);
	void setNoise3DTexName(GLuint);
 };


#endif
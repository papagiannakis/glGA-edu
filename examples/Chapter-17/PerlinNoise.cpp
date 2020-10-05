//========================================================================= 
// File: PerlinNoise.cpp 
//========================================================================= 
/* Creates Perlin Noise rasters  
   restructured by Ioannis Aggelogiannis to fit in glGA */ 

#include "PerlinNoise.h"

static int p[MAXB + MAXB + 2]; 
static double g3[MAXB + MAXB + 2][3]; 

// ======================================================================== 
// Constructor 
PerlinNoise::PerlinNoise(int w, int h, int t)
{
	setNoise3DTexName(0);
	setNoise3DTexSize(128);
	make3DNoiseTexture(w, h, t);
	init3DNoiseTexture();
}
// ======================================================================== 

// Makes noise textures tile able 
void PerlinNoise::SetNoiseFrequency(int frequency)
{
	start = 1; 
	B = frequency; 
	BM = B-1;
}
//======================================================================== 

//Normalize vectors used to create noise
void PerlinNoise::normalize3(double v[3]) 
{
	double s; 
	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); 
	v[0] = v[0] / s; 
	v[1] = v[1] / s; 
	v[2] = v[2] / s;
}
// =======================================================================

//Starts noise generation algorithm 
void PerlinNoise::startNoise()
{
	int i, j, k;

	srand(30757); 
	for (i = 0; i < B; i++) 
	{ 
		p[i] = i;

		for (j = 0; j < 3; j++) 
			g3[i][j] = (double)((rand() % (B + B)) - B) / B; 
		normalize3(g3[i]); 
	}

	while (--i) 
	{ 
		k = p[i]; 
		p[i] = p[j = rand() % B]; 
		p[j] = k; 
	}

	for (i = 0; i < B + 2; i++) 
	{ 
		p[B + i] = p[i]; 
		for (j = 0; j < 3; j++) 
			g3[B + i][j] = g3[i][j]; 
	} 
}
//=========================================================================

//3D noise algorithm
double PerlinNoise::noise3(double vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11; 
	double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v; 
	int i, j;

	if (start) 
	{ 
		start = 0; 
		startNoise(); 
	}

	setup(0, bx0, bx1, rx0, rx1); 
	setup(1, by0, by1, ry0, ry1); 
	setup(2, bz0, bz1, rz0, rz1);

	i = p[bx0]; 
	j = p[bx1];

	b00 = p[i + by0]; 
	b10 = p[j + by0]; 
	b01 = p[i + by1]; 
	b11 = p[j + by1];

	t = s_curve(rx0); 
	sy = s_curve(ry0); 
	sz = s_curve(rz0);

	q = g3[b00 + bz0]; u = at3(rx0, ry0, rz0); 
	q = g3[b10 + bz0]; v = at3(rx1, ry0, rz0);

	a = lerp(t, u, v);

	q = g3[b01 + bz0]; u = at3(rx0, ry1, rz0); 
	q = g3[b11 + bz0]; v = at3(rx1, ry1, rz0); 
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[b00 + bz1]; u = at3(rx0, ry0, rz1); 
	q = g3[b10 + bz1]; v = at3(rx1, ry0, rz1); 
	a = lerp(t, u, v);

	q = g3[b01 + bz1]; u = at3(rx0, ry1, rz1); 
	q = g3[b11 + bz1]; v = at3(rx1, ry1, rz1); 
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}
//====================================================================== 

//Make width*height*thick Perlin Noise texture
void PerlinNoise::make3DNoiseTexture(int width, int height, int thick)
{
	int f, i, j, k, inc; 
	int startFrequency = 4; 
	int numOctaves = 4; 
	double ni[3]; 
	double inci, incj, inck; 
	int frequency = startFrequency; 
	GLubyte* ptr; 
	double amp = 0.5;

	if ((noise3DTexPtr = (GLubyte*) malloc(width * height * thick * 4)) == NULL) 
	{ 
		std::cerr << "Couldn't not allocate 3d Perlin Noise texture\n"; 
	}

	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5) 
	{
		SetNoiseFrequency(frequency); 
		ptr = noise3DTexPtr; 
		ni[0] = ni[1] = ni[2] = 0;

		inci = 1.0 / (width / frequency); 
		for (i = 0; i < width; ++i, ni[0] += inci) 
		{ 
			incj = 1.0 / (height / frequency); 
			for (j = 0; j < height; ++j, ni[1] += incj) 
			{ 
				inck = 1.0 / (thick / frequency); 
				for (k = 0; k < thick; ++k, ni[2] += inck, ptr += 4) 
					*(ptr + inc) = (GLubyte) (((noise3(ni) + 1.0) * amp) * 128.0); 
			} 
		} 
	} 
}
//========================================================================

//Set the size of the 3D Texture
void PerlinNoise::setNoise3DTexSize(int size)
{
	noise3DTexSize = size;
}
//========================================================================

//Set the name of the 3D Texture
void PerlinNoise::setNoise3DTexName(GLuint name)
{
	noise3DTexName = name;
}
//========================================================================

//Generate,Activate and binds the 3D Texture
void PerlinNoise::init3DNoiseTexture()
{
	glGenTextures(1, &noise3DTexName);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, noise3DTexName);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, noise3DTexSize,noise3DTexSize, noise3DTexSize, 0, GL_RGBA,GL_UNSIGNED_BYTE, noise3DTexPtr);
}
//========================================================================
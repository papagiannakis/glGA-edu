//
//  CGA_SH.hpp
//  PRT
//
//  Created by Margrarita Papaefthimiou on 22/10/16.
//  Copyright © 2016 George Papagiannakis. All rights reserved.
//
#ifndef CGA_SH_hpp
#define CGA_SH_hpp

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

float computeSH(int l, int m, float x, float y, float z);
float computeSH1(int l, int m, float x, float y, float z);

#endif /* CGA_SH_hpp */

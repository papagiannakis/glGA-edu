/************************************************************************/
/*
 * osgPRT Library
 *
 * Author: George Papagiannakis
 *
 * SphericalHarmonics.h
 * Copyright (c) 2004-5 MIRALab-University of Geneva
 * -------------------------------------------------------------------------
 * Author:   $Author: papagian $
 * Modified: $Date: 2006/02/16 15:59:42 $
 * Revision: $Revision: 1.33 $
 * -------------------------------------------------------------------------
 *
 * Class containing all maths&background related to SH and Legendre Polynomials
 * References:
 // http://research.microsoft.com/~ppsloan/
 // http://www.donw.co.uk/
 // http://www.research.scea.com/gdc2003/spherical-harmonic-lighting.html
 // http://mathworld.wolfram.com/LegendrePolynomial.html
 // http://mathworld.wolfram.com/SphericalHarmonic.html
 // http://mathworld.wolfram.com/DoubleFactorial.html
 // http://www.gnu.org/software/gsl/
 */
/************************************************************************/
#pragma once

#include <typeinfo>
#include <fstream>
#include <vector>

#ifdef WIN32
#include <direct.h>
#define chdir _chdir
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

class  SphericalHarmonics
{
public:
    int doubleFactorial(int x);
    int factorial(int x);
    double K(const int l, const int m);
    double y(const int l, const int m, const double theta, const double phi);
    double P(const int l, const int m, const double x);
};

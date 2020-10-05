//
//  SphericalHarmonics.cpp
//  glGA
//
//  Created by Margrarita Papaefthimiou on 22/10/16.
//  Copyright © 2016 George Papagiannakis. All rights reserved.
//

#include "SphericalHarmonics.h"
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

int SphericalHarmonics::doubleFactorial(int x)
{
    if (x==0 || x==-1)
    {	return 1;	}
    
    int result=x;
    while ((x-=2)>0)
    {
        result *=x;
    }
    return result;
}//end doubleFactorial()

int SphericalHarmonics::factorial(int x)
{
    if (x==0)
    { return 1;	}
    
    int result=x;
    while (--x>0)
    { result*=x; }
    
    return result;
}//end factorial()

double SphericalHarmonics::K(const int l, const int m)
{
    return(sqrt( ((2 * l + 1) * factorial(l - abs(m))) / (4 * M_PI * factorial(l + abs(m))) ));
}//end K()

double SphericalHarmonics::P(const int l, const int m, const double x)
{
    //Evaluation accroding to 3 rules from [Greene03]
    
    //Using Rule2 (needs no previous results)
    //[Greene03] specifies as rule 2 final power "m/2" howeevr, wrong results unless it is specified as "m"
    if (l==m)
    { return( pow(-1.0,m)*doubleFactorial(2*m-1)*pow(sqrt(1-x*x),m) ); }
    
    //Using Rule3 requires the result for the same argument of the previous band
    if (l==m+1)
    { return(x * (2*m+1)*P(m,m,x)); }
    
    //Using Rule1 (main reccurence term) uses the result of the same argument from
    //the previous two bands
    return( (x*(2*l-1) * P(l-1,m,x) - (l+m-1)*P(l-2,m,x)) /(l-m));
    
}//end P<LegendreNormal>()

double SphericalHarmonics::y(int l, int m, double theta, double phi)
{
    const double sqrt2=sqrt(2.0);
    
    if (m == 0)
        return( K(l,0) * P(l,0,cos(theta)) );
    
    if (m > 0)
        return ( sqrt2 * K(l,m) * cos(m*phi) * P(l,m,cos(theta)) );
    
    return ( sqrt2 * K(l,m) * sin(-m * phi) * P(l,-m,cos(theta)) );
}//end y()

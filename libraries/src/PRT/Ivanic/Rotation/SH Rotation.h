//////////////////////////////////////////////////////////////////////////////////////////
//	SH Rotation.h
//	Functions to rotate SH coefficients
//	Downloaded from: www.paulsprojects.net
//	Created:	2nd October 2003
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef SH_ROTATION_H
#define SH_ROTATION_H

double * RotateSHCoefficients(	int numBands, double * unrotatedCoeffs, double * rotatedCoeffs,
							double theta, double phi);

//Get a matrix for rotating one band of coefficients about the Z axis
void GetZRotationMatrix(int band, double * entries, double angle);

//Get a matrix for rotating one band of coefficients about the X axis, by 90 degrees
void GetX90DegreeRotationMatrix(int band, double * entries);

//Apply a matrix to a vector of coefficients
void ApplyMatrix(	int size, double * matrix, bool transpose,
					double * inVector, double * outVector);
#endif

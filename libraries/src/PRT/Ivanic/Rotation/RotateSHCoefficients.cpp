//////////////////////////////////////////////////////////////////////////////////////////
//	RotateSHCoefficients.cpp
//	Main function for rotating SH coefficients
//	Downloaded from: www.paulsprojects.net
//	Created:	2nd October 2003
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#include "PRT/Ivanic/Log/LOG.h"
#include "PRT/Ivanic/Rotation/SH Rotation.h"

double* RotateSHCoefficients(	int numBands, double * unrotatedCoeffs, double * rotatedCoeffs,
							double theta, double phi)
{
	int numFunctions=numBands*numBands;

	for(int i=0; i<numFunctions; ++i)
		rotatedCoeffs[i]=unrotatedCoeffs[i];

	//Band 0 coefficient is unchanged
	rotatedCoeffs[0]=unrotatedCoeffs[0];

	//Rotate band 1 coefficients
	if(numBands>1)
	{
		//Get the rotation matrices for band 1 (want to apply Z1*Xt*Z2*X)
		double band1X[9];
		double band1Z1[9];
		double band1Z2[9];

		GetZRotationMatrix(1, band1Z1, phi);
		GetX90DegreeRotationMatrix(1, band1X);
		GetZRotationMatrix(1, band1Z2, theta);

		//Create space to hold the intermediate results
		double band1A[3], band1B[3], band1C[3];

		//Apply the matrices
		ApplyMatrix(3, band1X, false, &unrotatedCoeffs[1], band1A);
		ApplyMatrix(3, band1Z2, false, band1A, band1B);
		ApplyMatrix(3, band1X, true, band1B, band1C);

		ApplyMatrix(3, band1Z1, false, band1C, &rotatedCoeffs[1]);
	}

	//Rotate band 2 coefficients
	if(numBands>2)
	{
		double band2X[25];
		double band2Z1[25];
		double band2Z2[25];

		GetZRotationMatrix(2, band2Z1, phi);
		GetX90DegreeRotationMatrix(2, band2X);
		GetZRotationMatrix(2, band2Z2, theta);

		//Create space to hold the intermediate results
		double band2A[5], band2B[5], band2C[5];

		//Apply the matrices
		ApplyMatrix(5, band2X, false, &unrotatedCoeffs[4], band2A);
		ApplyMatrix(5, band2Z2, false, band2A, band2B);
		ApplyMatrix(5, band2X, true, band2B, band2C);

		ApplyMatrix(5, band2Z1, false, band2C, &rotatedCoeffs[4]);
	}

	//Rotate band 3 coefficients
	if(numBands>3)
	{
		double band3X[49];
		double band3Z1[49];
		double band3Z2[49];

		GetZRotationMatrix(3, band3Z1, phi);
		GetX90DegreeRotationMatrix(3, band3X);
		GetZRotationMatrix(3, band3Z2, theta);

		//Create space to hold the intermediate results
		double band3A[7], band3B[7], band3C[7];

		//Apply the matrices
		ApplyMatrix(7, band3X, false, &unrotatedCoeffs[9], band3A);
		ApplyMatrix(7, band3Z2, false, band3A, band3B);
		ApplyMatrix(7, band3X, true, band3B, band3C);

		ApplyMatrix(7, band3Z1, false, band3C, &rotatedCoeffs[9]);
	}
    
    return rotatedCoeffs;
}

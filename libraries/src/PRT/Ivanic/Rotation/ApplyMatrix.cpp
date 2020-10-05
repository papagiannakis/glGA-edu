//////////////////////////////////////////////////////////////////////////////////////////
//	ApplyMatrix.cpp
//	Apply a matrix to a vector of coefficients
//	Downloaded from: www.paulsprojects.net
//	Created:	2nd October 2003
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#include "PRT/Ivanic/Maths/Maths.h"
#include "PRT/Ivanic/Log/LOG.h"
#include "PRT/Ivanic/Rotation/SH Rotation.h"

void ApplyMatrix(	int size, double * matrix, bool transpose,
					double * inVector, double * outVector)
{
	//Loop through entries
	for(int i=0; i<size; ++i)
	{
		//Clear this entry of outVector
		outVector[i]=0.0;

		//Loop through matrix row/column
		for(int j=0; j<size; ++j)
		{
			if(transpose)
				outVector[i]+=matrix[j*size+i]*inVector[j];
			else
				outVector[i]+=matrix[i*size+j]*inVector[j];
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//	GetZRotationMatrix.cpp
//	Get a matrix for rotating one band of coefficients about the Z axis
//	Downloaded from: www.paulsprojects.net
//	Created:	2nd October 2003
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#include "PRT/Ivanic/Maths/Maths.h"
#include "PRT/Ivanic/Rotation/SH Rotation.h"

void GetZRotationMatrix(int band, double * entries, double angle)
{
	//Calculate the size of the matrix
	int size=2*band+1;

	//Convert angle to radians
	angle*=M_PI/180.0;

	//Entry index
	int currentEntry=0;

	//Loop through the rows and columns of the matrix
	for(int i=0; i<size; ++i)
	{
		for(int j=0; j<size; ++j, ++currentEntry)
		{
			//Initialise this entry to zero
			entries[currentEntry]=0.0;

			//For the central row (i=(size-1)/2), entry is 1 if j==i, else zero
			if(i==(size-1)/2)
			{
				if(j==i)
					entries[currentEntry]=1.0;

				continue;
			}

			//For i<(size-1)/2, entry is cos if j==i or sin if j==size-i-1
			//The angle used is k*angle where k=(size-1)/2-i
			if(i<(size-1)/2)
			{
				int k=(size-1)/2-i;

				if(j==i)
					entries[currentEntry]=cos(k*angle);

				if(j==size-i-1)
					entries[currentEntry]=sin(k*angle);

				continue;
			}

			//For i>(size-1)/2, entry is cos if j==i or -sin if j==size-i-1
			//The angle used is k*angle where k=i-(size-1)/2
			if(i>(size-1)/2)
			{
				int k=i-(size-1)/2;

				if(j==i)
					entries[currentEntry]=cos(k*angle);

				if(j==size-i-1)
					entries[currentEntry]=-sin(k*angle);

				continue;
			}
		}
	}

	return;
}

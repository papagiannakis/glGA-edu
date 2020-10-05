//////////////////////////////////////////////////////////////////////////////////////////
//	GetX90DegreeRotationMatrix.cpp
//	Get a matrix for rotating one band of coefficients about the X axis by 90 degrees
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

void GetX90DegreeRotationMatrix(int band, double * entries)
{
	//Ensure that 0<=band<=3
	if(band>3)
	{
		LOG::Instance()->OutputError("X rotation matrices are only known for bands 0-3");
		return;
	}

	if(band==0)
	{
		entries[0]= 1.0;
	}

	if(band==1)
	{
		entries[0]= 0.0;
		entries[1]= 1.0;
		entries[2]= 0.0;
		entries[3]=-1.0;
		entries[4]= 0.0;
		entries[5]= 0.0;
		entries[6]= 0.0;
		entries[7]= 0.0;
		entries[8]= 1.0;
	}

	if(band==2)
	{
		entries[ 0]= 0.0;
		entries[ 1]= 0.0;
		entries[ 2]= 0.0;
		entries[ 3]= 1.0;
		entries[ 4]= 0.0;
		entries[ 5]= 0.0;
		entries[ 6]=-1.0;
		entries[ 7]= 0.0;
		entries[ 8]= 0.0;
		entries[ 9]= 0.0;
		entries[10]= 0.0;
		entries[11]= 0.0;
		entries[12]=-0.5;
		entries[13]= 0.0;
		entries[14]=-sqrt(3.0)/2;
		entries[15]=-1.0;
		entries[16]= 0.0;
		entries[17]= 0.0;
		entries[18]= 0.0;
		entries[19]= 0.0;
		entries[20]= 0.0;
		entries[21]= 0.0;
		entries[22]=-sqrt(3.0)/2;
		entries[23]= 0.0;
		entries[24]= 0.5;
	}

	if(band==3)
	{
		//Initialise all entries to 0
		for(int i=0; i<49; ++i)
			entries[i]=0.0;

		entries[ 3]=-sqrt(0.625);
		entries[ 5]= sqrt(0.375);
		
		entries[ 8]=-1.0;
		
		entries[17]=-sqrt(0.375);
		entries[19]=-sqrt(0.625);
		
		entries[21]= sqrt(0.625);
		entries[23]= sqrt(0.375);
		
		entries[32]=-0.25;
		entries[34]=-sqrt(15.0)/4;
		
		entries[35]=-sqrt(0.375);
		entries[37]= sqrt(0.625);

		entries[46]=-sqrt(15.0)/4;
		entries[48]= 0.25;
	}

	return;
}

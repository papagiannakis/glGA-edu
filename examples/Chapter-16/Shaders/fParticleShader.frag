#version 150 core

in  vec4  Color;
in  float STime;
in	float LT;

uniform int firew;

out vec4 colorOUT;

void main() 
{ 
	if(firew == 1)
	{
		colorOUT = vec4(1.0- STime/350,0.0 + STime/100,0.0,1.0);
	}
	else
	{
		colorOUT = vec4(0.0 + STime/100,1.0- STime/100,0.0,1.0);
	}

	if(LT < STime)
	{
		colorOUT = vec4(0.0,5.0,0.0,0.0);
	}
} 


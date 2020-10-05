#version 150 core

#define MaxTexCoord 1

in vec3 normal;
in vec4 TexCoord[MaxTexCoord];

uniform sampler2D gSampler; 

out vec4 colorOUT;

void main() 
{ 
	colorOUT = texture(gSampler, TexCoord[0].xy);
}



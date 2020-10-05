#version 150 core

in  vec2  UV;

uniform sampler2D texID;

out vec4 colorOUT;

void main() 
{                                            
	float Depth = texture(texID, UV).x;                              
    Depth = 1.0 - (1.0 - Depth);                            
    colorOUT = vec4(Depth);
} 


#version 150 core

in  vec4  color;
in  vec3  coords;

uniform samplerCube CubeMapTex;

out vec4 colorOUT;

void main() 
{ 
	colorOUT = color;
	colorOUT = texture(CubeMapTex,coords);
} 


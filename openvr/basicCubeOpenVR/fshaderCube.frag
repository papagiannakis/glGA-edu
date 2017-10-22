#version 410 core

in vec4 color;
in vec2 texCoord;
out  vec4  colorOUT;

uniform sampler2D texture1;

void main() 
{ 
    colorOUT = texture(texture1, texCoord);
	
    //colorOUT = vec4(0.0,1.0,0.0,1.0);
} 


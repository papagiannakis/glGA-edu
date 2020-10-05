#version 150 core

in  vec4 vPosition;
in  vec3 vNormal;
in  vec4 vColor;

uniform mat4 MV_mat;
uniform mat4 MVP_mat;

out vec4 color;
out vec3 coords;

void main() 
{
	gl_Position		= MVP_mat * vPosition;

	coords = vPosition.xyz;

	color = vColor;
} 

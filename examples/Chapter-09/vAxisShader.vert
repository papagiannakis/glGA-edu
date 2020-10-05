#version 150 core

in  vec4 vPosition;
in  vec4 vColor;

uniform mat4 MV_mat;
uniform mat4 MVP_mat;
uniform mat4 Normal_mat;

out vec4 color;
out vec3 normal;
out vec4 eye;

void main() 
{
  gl_Position = MVP_mat * vPosition;
  color = vColor;
} 

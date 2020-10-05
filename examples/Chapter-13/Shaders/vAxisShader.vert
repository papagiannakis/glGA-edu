#version 150 core

in  vec4 vPosition;
in  vec4 vColor;

uniform mat4 MVP_mat;

out vec4 color;

void main() 
{
  gl_Position = MVP_mat * vPosition;
  color = vColor;
} 

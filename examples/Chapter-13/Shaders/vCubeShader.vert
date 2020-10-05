#version 150 core

in  vec4 vPosition;
in  vec4 vColor;

uniform mat4 MVP_mat;
uniform int cubeli;

out vec4 color;

void main() 
{

  gl_Position = MVP_mat * vPosition;

  if(cubeli == 1)
  {
	color = vec4(1.0,0.0,1.0,1.0);
  }
  else
  {
	color = vec4(0.0,1.0,0.0,1.0);
  }
} 

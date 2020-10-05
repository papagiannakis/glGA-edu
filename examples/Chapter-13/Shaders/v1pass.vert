#version 150 core

in  vec4 vPosition;

out vec2 UV;

void main() 
{
  gl_Position = vPosition;

  UV = (vPosition.xy+vec2(1,1))/2.0;
} 
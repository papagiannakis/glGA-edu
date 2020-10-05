#version 150 core

in  vec4 vPosition;

uniform float exposure;

out vec4 color;
out vec2 UV;
out float Exposure;

void main() 
{
  gl_Position = vPosition;

  UV = (vPosition.xy+vec2(1,1))/2.0;

  Exposure = pow(2.0,exposure);
} 
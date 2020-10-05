#version 150 core

in  vec2  UV;
in float Exposure;

uniform sampler2D texID;
uniform float Gamma;
uniform float Defog;
uniform vec3 FogColor;

out vec4 colorOUT;

void main() 
{ 

	vec3 DefogColor = (Defog * FogColor);

	vec3 c = texture(texID, UV).rgb;

	c = c - DefogColor;
	c = max(vec3(0.0,0.0,0.0),c);

	c *= Exposure;

	c = vec3(pow(c.r,Gamma),pow(c.g,Gamma),pow(c.b,Gamma));

	colorOUT = vec4(c.rgb,1.0);
} 


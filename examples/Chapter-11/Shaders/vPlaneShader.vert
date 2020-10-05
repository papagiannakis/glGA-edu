#version 150 core

#define MaxTexCoord			   1

in  vec4 MCvertex;
in  vec3 vNormal;
in  vec4 MultiTexCoord0;

uniform mat4 MVP_mat;
uniform mat3 Normal_mat;
uniform mat4 TextureMatrix[MaxTexCoord];

out	vec4 TexCoord[MaxTexCoord];
out vec3 normal;

void main() 
{
	vec4 ecPosition;

	// transform the normal into eye space and normalize the result
	normal = normalize(Normal_mat * vNormal);

	gl_Position = MVP_mat * MCvertex;

	TexCoord[0] = TextureMatrix[0] * MultiTexCoord0;
}


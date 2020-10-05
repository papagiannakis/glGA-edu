#version 150 core

in vec3 MCVertex;
in vec3 MCNormal;
in vec2 TexCoord0;

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

uniform vec3 LightPosition;
uniform float Time;
uniform float Scale;

out vec3 ObjPos;
out float V;
out float LightIntensity;

void main()
{
	ObjPos = (MCVertex + vec3(0.0, 0.0, Time)) * Scale;

	vec3 pos = vec3(MVMatrix * vec4(MCVertex,1.0));
	vec3 tnorm = normalize(NormalMatrix * MCNormal);
	vec3 lightVec = normalize(LightPosition - pos);

	LightIntensity = max(dot(lightVec, tnorm), 0.0);

	V = TexCoord0.t; // try .s for vertical stripes

	gl_Position = MVPMatrix * vec4(MCVertex,1.0);
}
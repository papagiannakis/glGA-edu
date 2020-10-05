#version 150 core

in vec4 MCVertex;
in vec3 MCNormal;

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform mat4 RotateCloud;
uniform mat3 NormalMatrix;

uniform vec3 LightPos;
uniform float Scale;

out float LightIntensity;
out vec3 MCposition;

void main()
{
	vec3 ECposition = vec3(MVMatrix * MCVertex);
	MCposition = (RotateCloud * MCVertex).xyz * Scale;

	vec3 tnorm = normalize(vec3(NormalMatrix * MCNormal));

	LightIntensity = dot(normalize(LightPos - ECposition), tnorm);
	LightIntensity *= 1.5;

	gl_Position = MVPMatrix * MCVertex;
}
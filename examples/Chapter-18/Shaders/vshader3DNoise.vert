#version 150 core

in vec4 MCVertex;
in vec3 MCNormal;

uniform mat4 MVPMatrix;

uniform float Scale;

out vec3 MCposition;

void main()
{
	MCposition = vec3(MCVertex) * Scale;

	gl_Position = MVPMatrix * MCVertex;
}
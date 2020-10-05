#version 150 core

in vec4 MCVertex;
in vec2 TexCoord0;

uniform mat4 MVPMatrix;

out vec2 TexCoord;

void main()
{
	gl_Position = MVPMatrix * MCVertex;

	TexCoord = TexCoord0;
}
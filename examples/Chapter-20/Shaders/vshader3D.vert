#version 150 core

in vec3 MCVertex;
in vec3 MCNormal;
in vec2 TexCoord0;

uniform mat4 MMatrix;
uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

out vec2 TexCoordZ;
out vec3 Position,Normal;

void main()
{
	Position    = (MMatrix * vec4(MCVertex,1.0)).xyz;

	Normal      = (MMatrix * vec4(MCNormal,1.0)).xyz;

	TexCoordZ    = TexCoord0;

	gl_Position = MVPMatrix * vec4(MCVertex,1.0);
}

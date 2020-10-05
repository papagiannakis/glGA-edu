#version 150 core

in vec2 MCVertex;

uniform sampler2D WaterNormalBumpMapTexture;
uniform float WaterLevel,ODWMS;
uniform mat4 MVPMatrix;

out vec3 Position;
out vec2 TexCoord0;
out vec4 TexCoord1;

void main()
{
	Position.xz = MCVertex.xy;

	TexCoord0.st= MCVertex.xy * ODWMS + 0.5;

	vec3 Normal = texture(WaterNormalBumpMapTexture,TexCoord0.st).rgb;

	Position.y  = WaterLevel + texture(WaterNormalBumpMapTexture, TexCoord0.st).a;

	TexCoord1   = MVPMatrix * vec4(Position, 1.0);

	gl_Position = TexCoord1;
}

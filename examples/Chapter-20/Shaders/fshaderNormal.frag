#version 150 core

in vec2 TexCoord;

uniform sampler2D WaterNormalBumpMapTexture;

uniform float ODWNBMTR, WMSDWNBMTRM2;

out vec4 FragColor;

void main()
{
	float y[5];
	
	y[0] = texture(WaterNormalBumpMapTexture, TexCoord.st + vec2(ODWNBMTR, 0.0)).a;
	y[1] = texture(WaterNormalBumpMapTexture, TexCoord.st + vec2(0.0, ODWNBMTR)).a;
	y[2] = texture(WaterNormalBumpMapTexture, TexCoord.st - vec2(ODWNBMTR, 0.0)).a;
	y[3] = texture(WaterNormalBumpMapTexture, TexCoord.st - vec2(0.0, ODWNBMTR)).a;
	
	y[4] = texture(WaterNormalBumpMapTexture, TexCoord.st).a;

	vec3 Normal = normalize(vec3(y[2] - y[0], WMSDWNBMTRM2, y[1] - y[3]));
	
	FragColor = vec4(Normal, y[4]);
}
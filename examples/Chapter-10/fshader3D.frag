#version 150 core

uniform samplerCube CubeMapTex;

uniform vec3 BaseColor;
uniform float MixRatio;

in vec3  ReflectDir;
in float LightIntensity;

in vec2 TexCoords;

out vec4 FragColor;

void main() 
{ 
	// Look up enviroment map value in cube map

	vec3 envColor = vec3(texture(CubeMapTex, ReflectDir));

	// Add lighting to base color and mix

	vec3 base = LightIntensity * BaseColor;
	envColor = mix(envColor, base, MixRatio);

	//FragColor = vec4(1.0,2.0,1.0,1.0);
	FragColor = vec4(envColor,1.0);

} 

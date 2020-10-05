#version 150 core

uniform samplerCube SpecularEnvMap;
uniform samplerCube DiffuseEnvMap;

uniform vec3  BaseColor;
uniform float SpecularPercent;
uniform float DiffusePercent;
uniform int	  Mode;

in vec3  ReflectDir;
in vec3  Normal;

out vec4 FragColor;

void main() 
{ 
	// Look up environment map values in cube maps

	vec3 specularColor = texture(SpecularEnvMap, normalize(ReflectDir)).rgb;
	vec3 diffuseColor  = texture(DiffuseEnvMap, normalize(Normal)).rgb;

	// Add lighting to base color and mix

	vec3 color = mix(BaseColor, diffuseColor*BaseColor, DiffusePercent);
	color = mix(color, specularColor + color, SpecularPercent);

	if(Mode == 0)
	{
		FragColor = vec4(color, 1.0);
	}
	else if(Mode == 1)
	{
		FragColor = vec4(mix(BaseColor,specularColor,SpecularPercent),1.0);
	}
	else
	{
		FragColor = vec4(mix(BaseColor,diffuseColor,DiffusePercent),1.0);
	}
	

} 

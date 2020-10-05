#version 150 core

in	vec3 wPosition;
in	vec3 EyeDirection;
in	vec3 LightDirection;
in	vec3 fNormal;
in	vec2 vTexCoord0;
in	vec4 LightClipSpacePos;

uniform sampler2D textureMap; 
uniform sampler2DShadow shadowMap;

struct Lights
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
	float intensity;
};

uniform Lights lights[1];

struct MaterialProperties
{  
   vec4 ambient;   
   vec4 diffuse;   
   vec4 specular;   
   float shininess; 
};

uniform MaterialProperties frontMaterial;

out vec4 colorOUT;

void main() 
{ 
	vec3 n = normalize(fNormal);
	vec3 l = normalize(LightDirection);
	vec3 v = normalize(EyeDirection);
	vec3 H = normalize(l + v);
	
	// cosine of the angle between the normal and the light direction
	float cosTheta = max( dot( n,l ), 0.0 );
	
	// Cosine of the angle between the Eye vector and the half way vector,
	float cosAlpha = pow( max(dot( n, H ), 0.0), frontMaterial.shininess );
	
	float d = length( (lights[0].position).xyz - wPosition );

	float sum = 0;
	// Sum contributions from texels around ShadowCoord
	sum += textureProjOffset(shadowMap, LightClipSpacePos,ivec2(-1,-1));
	sum += textureProjOffset(shadowMap, LightClipSpacePos,ivec2(-1,1));
	sum += textureProjOffset(shadowMap, LightClipSpacePos,ivec2(1,1));
	sum += textureProjOffset(shadowMap, LightClipSpacePos,ivec2(1,-1));

	float shadow = sum * 0.25;
	vec4 Light = vec4((frontMaterial.ambient).xyz * (lights[0].ambient).xyz +
			shadow * (frontMaterial.diffuse).xyz  * (lights[0].diffuse).xyz  * lights[0].intensity * cosTheta / (d*d) +				
			shadow * (frontMaterial.specular).xyz * (lights[0].specular).xyz * lights[0].intensity * cosAlpha / (d*d), 1.0);			// 20.0 == LightIntensity
	
	vec4 SampledColor = texture(textureMap, vTexCoord0.st);
	colorOUT = SampledColor * Light;
} 

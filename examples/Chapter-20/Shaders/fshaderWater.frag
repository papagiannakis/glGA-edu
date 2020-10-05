#version 150 core

in vec3 Position;
in vec2 TexCoord0;
in vec4 TexCoord1;

uniform sampler2D WaterNormalBumpMapTexture,ReflectionTexture, RefractionTexture;

uniform float WaterLevel;
uniform vec3 CameraPosition;
uniform mat4 ViewProjectionBiasInverse;

struct Lighting
{
	vec4 position;  

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
                             
    float constantAttenuation;  
    float linearAttenuation;     
    float quadraticAttenuation;
};

uniform Lighting SceneLight;

out vec4 FragColor;

void main()
{
	vec3 Normal = texture(WaterNormalBumpMapTexture,TexCoord0.st).rgb;
	
	vec2 TexCoord = TexCoord1.st / TexCoord1.w * 0.5 + 0.5;
	
	vec2 Offset = Normal.xz * 0.05;

	vec3 LightDirection0 = SceneLight.position.xyz - Position;
	float LightDistance2 = dot(LightDirection0, LightDirection0);
	float LightDistance = sqrt(LightDistance2);
	LightDirection0 /= LightDistance;
	float NdotLD = max(0.0, dot(Normal, LightDirection0));
	float att = SceneLight.constantAttenuation;
	att += SceneLight.linearAttenuation * LightDistance;
	att += SceneLight.quadraticAttenuation * LightDistance2;
	vec3 Light = (SceneLight.ambient.rgb + SceneLight.diffuse.rgb * NdotLD) / att;

	if(CameraPosition.y > WaterLevel)
	{
		vec3 Reflection = texture(ReflectionTexture, TexCoord + Offset).rgb;
		
		vec3 Refraction;
		
		Refraction.r = texture(RefractionTexture, Offset * 0.6 + TexCoord).r;
		Refraction.g = texture(RefractionTexture, Offset * 0.8 + TexCoord).g;
		Refraction.b = texture(RefractionTexture, Offset * 1.0 + TexCoord).b;

		vec3 LightDirection = normalize(Position - SceneLight.position.xyz);
		vec3 LightDirectionReflected = reflect(LightDirection, Normal);
		vec3 CameraDirection = normalize(CameraPosition - Position);
		float CDdotLDR = max(dot(CameraDirection, LightDirectionReflected), 0.0);
		vec3 Specular = SceneLight.specular.rgb * pow(CDdotLDR, 128.0);
		float NdotCD = max(dot(Normal, CameraDirection), 0.0);
		
		FragColor.rgb = mix(Reflection, Refraction, NdotCD) ;

		if(FragColor.r <= 0.0001 && FragColor.g <= 0.0001 && FragColor.b <= 0.0001)
		{
			FragColor = vec4(mix(vec3(0.0,0.0,1.0),Specular,0.7),1.0);
			FragColor.rgb += Light;
		}
	}
	else
	{
		FragColor.r = texture(RefractionTexture, Offset * 0.6 + TexCoord).r;
		FragColor.g = texture(RefractionTexture, Offset * 0.8 + TexCoord).g;
		FragColor.b = texture(RefractionTexture, Offset * 1.0 + TexCoord).b;

		if(FragColor.r <= 0.0001 && FragColor.g <= 0.0001 && FragColor.b <= 0.0001)
		{
			FragColor.rgb = vec3(0.0,0.0,1.0);
			FragColor.rgb += Light;
		}
	}
}


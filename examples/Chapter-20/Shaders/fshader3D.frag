#version 150 core

in vec2 TexCoordZ;
in vec3 Position,Normal;

uniform int ClipType;
uniform sampler2D Texture,WaterNormalBumpMapTexture;
uniform float WaterLevel, ODWMS;

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
	if(ClipType == 1) if(Position.y < WaterLevel) discard;

	vec3 LightDirection = SceneLight.position.xyz - Position;
	float LightDistance2 = dot(LightDirection, LightDirection);
	float LightDistance = sqrt(LightDistance2);
	LightDirection /= LightDistance;
	float NdotLD = max(0.0, dot(Normal, LightDirection));
	float att = SceneLight.constantAttenuation;
	att += SceneLight.linearAttenuation * LightDistance;
	att += SceneLight.quadraticAttenuation * LightDistance2;
	vec3 Light = (SceneLight.ambient.rgb + SceneLight.diffuse.rgb * NdotLD) / att;
	
	vec2 TexCoord = Position.xz * ODWMS + 0.5;										
	float waterlevel = WaterLevel + texture(WaterNormalBumpMapTexture, TexCoord).a;

	if(NdotLD > 0.0)
	{
	 	if(SceneLight.position.y > WaterLevel)
		{
	    	if(Position.y > waterlevel)
			{
				LightDirection = SceneLight.position.xyz;
				LightDirection.y = WaterLevel * 2.0 - LightDirection.y;
				LightDirection -= Position;
				LightDistance2 = dot(LightDirection, LightDirection);
				LightDistance = sqrt(LightDistance2);
				LightDirection /= LightDistance;
				NdotLD = max(0.0, dot(Normal, LightDirection));
				att = SceneLight.constantAttenuation;
				att += SceneLight.linearAttenuation * LightDistance;
				att += SceneLight.quadraticAttenuation * LightDistance2;
			}
			
			if(LightDirection.y != 0.0)
			{
				float Distance = (WaterLevel - Position.y) / LightDirection.y;
				
				if(Distance > 0.0 && Distance < LightDistance)
				{
					TexCoord = (LightDirection.xz * Distance + Position.xz) * ODWMS + 0.5;
				}
			}
			
			vec3 WaterNormal = texture(WaterNormalBumpMapTexture, TexCoord).rgb;
			if(Position.y > waterlevel) WaterNormal = -WaterNormal;
			float WNdotLD = max(dot(WaterNormal, LightDirection), 0.0);
			float Caustic = NdotLD * pow(WNdotLD, 4.0) / att;
		    Light += SceneLight.specular.rgb * Caustic;
		}
	}

	FragColor = texture(Texture,TexCoordZ);
	FragColor.rgb *= Light;
}

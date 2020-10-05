#version 330 core

out vec4 FragColor;
uniform vec3 LightColor;
uniform float Shininess;
uniform vec2 gScreenSize;
uniform float AmbientI;
uniform float DiffuseI;
uniform float SpecularI;
uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;
uniform vec3 LightDir;



void main()
{
    vec2 TexCoord = gl_FragCoord.xy / gScreenSize;
	vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
	vec3 Color = texture(gColorMap, TexCoord).xyz;
	vec3 Normal = texture(gNormalMap, TexCoord).xyz;
	Normal = normalize(Normal);

    vec3 LightDirection = LightDir;

	vec4 AmbientColor = vec4(LightColor * AmbientI, 1.0);
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = vec4(LightColor * DiffuseI * DiffuseFactor, 1.0);

        vec3 VertexToEye = normalize(vec3(0,0,0) - WorldPos);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);        
        if (SpecularFactor > 0.0) {
            SpecularFactor = pow(SpecularFactor, Shininess);
            SpecularColor = vec4(LightColor * SpecularI * SpecularFactor, 1.0);
        }
    }


    vec4 Result = (AmbientColor + DiffuseColor + SpecularColor);

	
	FragColor = vec4(Color, 1.0) * Result;
}

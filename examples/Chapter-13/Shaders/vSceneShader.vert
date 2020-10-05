#version 150 core

in  vec3 vPosition;
in  vec3 vNormal;
in  vec2 vTexture;

uniform mat4 M_mat;
uniform mat4 V_mat;
uniform mat4 MVP_mat;
uniform mat4 ShadowMatrix;
uniform vec3 CameraPosition;
uniform vec3 LightPosition;

struct Lights
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
	float intensity;
};

uniform Lights lights[1];

out	vec3 wPosition;
out vec3 EyeDirection;
out vec3 LightDirection;
out vec4 LightClipSpacePos;
out vec3 fNormal;
out	vec2 vTexCoord0;

void main() 
{
    // modelspace to clip space
	gl_Position =  MVP_mat * vec4(vPosition, 1.0);
	vTexCoord0 = vTexture;
	
	// modelspace to worldspace
	wPosition = (M_mat * vec4(vPosition, 1.0)).xyz;
	
	//transform vertices to light's clip space
	LightClipSpacePos = ShadowMatrix * vec4(vPosition, 1.0);  
	
	// modelspace to camera space
	vec3 cPosition = ( V_mat * M_mat * vec4(vPosition,1.0)).xyz;
	// vector from vertex to camera
	EyeDirection = CameraPosition - cPosition;		
	
	// light position in camera space
	vec3 cLightPosition = ( V_mat * vec4((lights[0].position).xyz, 1.0)).xyz;
	// vector from vertex to light
	LightDirection = cLightPosition + EyeDirection;		
	
	// Normal of the the vertex, in camera space
	fNormal = ( V_mat * M_mat * vec4(vNormal, 0.0)).xyz;
}

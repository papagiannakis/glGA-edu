#version 150 core

in  vec3 MCVertex;
in  vec3 MCNormal;
in	vec2 MCCoords;

out float LightIntensity;
out vec3  ReflectDir;
out vec2 TexCoords;

uniform vec3 LightPosition;

uniform mat4 MV_mat;
uniform mat4 MVP_mat;
uniform mat3 Normal_mat;

void main() 
{

	gl_Position		= MVP_mat * vec4(MCVertex,1.0);

    vec3 normal		= normalize(Normal_mat * MCNormal);
	
	TexCoords		= MCCoords;

	vec4 pos		= MV_mat * vec4(MCVertex,1.0);

	vec3 eyeDir		= normalize(-pos.xyz);

	ReflectDir		= reflect(eyeDir,normal);

	vec3 LightDir	= normalize(vec3(MV_mat * vec4(LightPosition,1.0)) - eyeDir);

	LightIntensity	= max(dot(LightDir, normal), 0.0);
}

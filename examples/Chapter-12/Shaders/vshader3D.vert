#version 150 core

in  vec3 MCVertex;
in  vec3 MCNormal;

out vec3  ReflectDir;
out vec3  Normal;

uniform mat4 MV_mat;
uniform mat4 MV_matTR;
uniform mat4 MVP_mat;
uniform mat3 Normal_mat;

void main() 
{

	gl_Position		= MVP_mat * vec4(MCVertex,1.0);

    Normal			= normalize(Normal_mat * MCNormal);

	vec4 pos		= MV_mat * vec4(MCVertex,1.0);

	vec3 eyeDir		= pos.xyz;

	ReflectDir		= vec3(MV_matTR * vec4(reflect(eyeDir,Normal),1.0));

	Normal			= vec3(MV_matTR * vec4(Normal,1.0));
}

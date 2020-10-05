#version 150 core

in  vec3 vPosition;
in  vec3 vNormal;
in  vec2 vTexture;
in  vec3 vTangent;

uniform mat4 MV_mat;
uniform mat4 MVP_mat;
uniform mat3 Normal_mat;

uniform vec3 LightPosition;

out vec2 TexCoord;
out vec3 LightDir;
out vec3 EyeDir;

void main() 
{
    gl_Position = MVP_mat * vec4(vPosition,1.0);

	EyeDir   = vec3(MV_mat * vec4(vPosition,1.0));
    TexCoord = vTexture.st;

	vec3 n = normalize(Normal_mat * vNormal);
	vec3 t = normalize(Normal_mat * vTangent);
	vec3 b = cross(n, t);

	vec3 v;
	v.x = dot(LightPosition, t);
	v.y = dot(LightPosition, b);
	v.z = dot(LightPosition, n);
	LightDir = normalize(v);

	v.x = dot(EyeDir, t);
	v.y = dot(EyeDir, b);
	v.z = dot(EyeDir, n);
	EyeDir = normalize(v);
}

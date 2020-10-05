#version 150 core

in  vec3 vPosition;
in  vec3 vNormal;
in  vec2 vTexture;

uniform mat4 MV_mat;
uniform mat4 MVP_mat;
uniform mat4 MVl_mat;
uniform mat3 Normal_mat;

struct Lights
{
	int activeLight;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;

	vec3 spotDirection;     
    float spotExponent;        
    float spotCutoff;      // (range: [0.0,90.0], 180.0)   
    float spotCosCutoff;   // (range: [1.0,0.0],-1.0)

    float constantAttenuation;  
    float linearAttenuation;     
    float quadraticAttenuation;
};
uniform int NumEnabledLights;
uniform Lights lights[4];
uniform vec4 eyes;


struct LightDir
{
	vec3 LightsDir;
	vec3 HalfWay_Vector;
};

struct spotL
{
	vec3 SD;
};

out	vec2 TexCoord;
out vec3 normal;
out vec3 eye_V;
out vec3 ecPosition3;
out float FogFragCoord;
out LightDir LD[4];
out spotL sD[1];
out vec2 test;

vec2 SphereMap(const in vec3 ecPosition3,const in vec3 normal)
{
	float m;
	vec3 r, u;

	u = normalize(ecPosition3);
	r = reflect(u, normal);
	r.z += 1.0;
	m = 0.5 * inversesqrt( dot(r,r) );

	return r.xy * m + 0.5;
}

void main() 
{

  vec4 ecPosition;
  // transform the normal into eye space and normalize the result
  normal = normalize(Normal_mat * vNormal);

  // Transform vertex to eye coordinates
  ecPosition  = MV_mat * vec4(vPosition,1.0);
  ecPosition3 = (vec3(ecPosition)) / ecPosition.w;

  
  // Trasform eye_position to eye coordinates 
  vec4 eYe = MVl_mat * eyes;
  vec3 eye = (vec3(eYe)) / eYe.w;
  	
  // Compute eye_vector form vertex to eye 
  //eye_V = vec3(eYe) - ecPosition3;
  eye_V = vec3(eYe) - ecPosition3;

  //re-ajusting Lights Direction for the moving camera
  for(int i = 0; i < 4; i++)
  {
	if(lights[i].position.w == 0.0)
	{
		LD[i].LightsDir =  vec3(MVl_mat * lights[i].position);
		LD[i].HalfWay_Vector = normalize(eye_V + normalize(LD[i].LightsDir));
	}
	else if(lights[i].position.w != 0.0)
	{
		vec4 le = MVl_mat * lights[i].position;
		vec3 lee = (vec3(le)) / le.w;

		//LD[i].LightsDir = lee - ecPosition3;
		LD[i].LightsDir = vec3(lee) - ecPosition3;

		if(lights[i].spotCutoff != 180.0)
		{
			vec4 de = MVl_mat * vec4(lights[i].spotDirection,1.0);
			sD[i-3].SD = ((vec3(de)) / de.w);
			sD[i-3].SD = sD[i-3].SD - lee;
		}
	}
  }

  FogFragCoord = abs(ecPosition.z);
  FogFragCoord = length(ecPosition.xyz );

  gl_Position = MVP_mat * vec4(vPosition,1.0);

  TexCoord = vTexture;
}

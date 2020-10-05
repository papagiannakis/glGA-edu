#version 150 core

#define AutomaticCoords		   false
#define MaxTexCoord 1
#define TexGenSphere	       false
#define TexGenReflection       false
#define TexGenObject		   false
#define TexGenEye		       false
#define	TexGenNormal		   false
#define NumEnabledTextureUnits 1
#define MaxClipPlanes		   1

out float gl_ClipDistance[MaxClipPlanes];

in  vec4 MCvertex;
in  vec4 vColor;
in  vec3 vNormal;
in  vec4 MultiTexCoord0;

uniform mat4 V_mat;
uniform mat4 MV_mat;
uniform mat4 MVP_mat;
uniform mat3 Normal_mat;
uniform mat4 TextureMatrix[MaxTexCoord];

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

out	vec4 TexCoord[MaxTexCoord];
out vec4 color;
out vec3 normal;
out vec3 eye_V;
out vec3 ecPosition3;
out LightDir LD[4];
out spotL sD[1];
out float FogFragCoord;

// GL_REFLECTION_MAP computation
vec3 ReflectionMap(const in vec3 ecPosition3,const in vec3 normal)
{
	float NdotU, m;
	vec3 u;

	u = normalize(ecPosition3);

	return (reflect(u, normal));
}

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
	vec2 sphereMap;
	vec3 reflection;

	vec4 ClipPlane[MaxClipPlanes];
	ClipPlane[0] = vec4(0.0,-1.0,0.0,0.0);

	// transform the normal into eye space and normalize the result
	normal = normalize(Normal_mat * vNormal);

	// Transform vertex to eye coordinates
	ecPosition  = MV_mat * MCvertex;
	ecPosition3 = (vec3(ecPosition)) / ecPosition.w;

	vec4 ObjectPlaneS[NumEnabledTextureUnits];
	ObjectPlaneS[0] = vec4(1.0,0.0,0.0,0.0);
	vec4 ObjectPlaneT[NumEnabledTextureUnits];
	ObjectPlaneT[0] = vec4(0.0,1.0,0.0,0.0); 
	vec4 ObjectPlaneR[NumEnabledTextureUnits];
	ObjectPlaneR[0] =  vec4(0.0,0.0,0.0,0.0); 
	vec4 ObjectPlaneQ[NumEnabledTextureUnits];
	ObjectPlaneQ[0] =  vec4(0.0,0.0,0.0,0.0); 

	vec4 EyePlaneS[NumEnabledTextureUnits];
	EyePlaneS[0] = vec4(1.0,0.0,0.0,0.0);
	vec4 EyePlaneT[NumEnabledTextureUnits];
	EyePlaneT[0] = vec4(0.0,1.0,0.0,0.0);
	vec4 EyePlaneR[NumEnabledTextureUnits];
	EyePlaneR[0] = vec4(0.0,0.0,0.0,0.0);
	vec4 EyePlaneQ[NumEnabledTextureUnits];
	EyePlaneQ[0] = vec4(0.0,0.0,0.0,0.0);

	// Compute sphere map coordinates if needed
	if(TexGenSphere)
	{
		sphereMap = SphereMap(ecPosition3, normal);
	}

	// Compute reflection map coordinates if needed
	if(TexGenReflection)
	{
		reflection = ReflectionMap(ecPosition3, normal);
	}

	// Compute texture coordinate for each enabled texture unit
	for(int i = 0; i < NumEnabledTextureUnits; i++)
	{
		if(TexGenObject)
		{
			TexCoord[i].s = dot(MCvertex, ObjectPlaneS[i]);
			TexCoord[i].t = dot(MCvertex, ObjectPlaneT[i]);
			TexCoord[i].p = dot(MCvertex, ObjectPlaneR[i]);
			TexCoord[i].q = dot(MCvertex, ObjectPlaneQ[i]);
		}

		if(TexGenEye)
		{
			TexCoord[i].s = dot(ecPosition, EyePlaneS[i]);
			TexCoord[i].t = dot(ecPosition, EyePlaneT[i]);
			TexCoord[i].p = dot(ecPosition, EyePlaneR[i]);
			TexCoord[i].q = dot(ecPosition, EyePlaneQ[i]);
		}

		if(TexGenSphere)
		{
			TexCoord[i] = vec4(sphereMap, 0.0, 1.0);
		}

		if(TexGenReflection)
		{
			TexCoord[i] = vec4(reflection, 1.0);
		}

		if(TexGenNormal)
		{
			TexCoord[i] = vec4(normal, 1.0);
		}
	}

	// Trasform eye_position to eye coordinates 
	vec4 eYe = MV_mat * eyes;
  	
	// Compute eye_vector from vertex to eye 

	eye_V = vec3(eYe) - ecPosition3;

	//re-ajusting Lights Direction for the moving camera
	for(int i = 0; i < 4; i++)
	{
		if(lights[i].activeLight == 1 && lights[i].position.w == 0.0)
		{
			LD[i].LightsDir =  vec3(MV_mat * lights[i].position);
			LD[i].HalfWay_Vector = normalize(eye_V + normalize(LD[i].LightsDir));
		}
		else if(lights[i].activeLight == 1 && lights[i].position.w != 0.0)
		{
			vec4 le = MV_mat * lights[i].position;
			vec3 lee = (vec3(le)) / le.w;

			//LD[i].LightsDir = lee - ecPosition3;

			LD[i].LightsDir = vec3(MV_mat * lights[i].position) - ecPosition3;

			if(lights[i].spotCutoff != 180.0)
			{
				vec4 de = V_mat * vec4(lights[i].spotDirection,1.0);
				vec3 dee= (vec3(de)) / de.w;
				sD[i-3].SD = dee - lee;
			}
		}
	}

	FogFragCoord = abs(ecPosition.z);

	gl_Position = MVP_mat * MCvertex;

	if(AutomaticCoords)
	{
		TexCoord[0]  = TextureMatrix[0] * TexCoord[0];
	}
	else
	{
		TexCoord[0] = TextureMatrix[0] * MultiTexCoord0;
	}

	color = vColor;

	for(int i = 0;i < MaxClipPlanes; i++)
	{
		gl_ClipDistance[i] = dot( ClipPlane[i], ecPosition);
	}

}


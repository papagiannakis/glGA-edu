#version 150 core
#define MaxTexCoord 1

in vec3 normal;
in vec3 ecPosition3;
in vec3 eye_V;
in vec3 sd;
in vec4 color; 
in vec4 TexCoord[MaxTexCoord];

uniform sampler2D gSampler; 

struct Lights
{
	int activeLight;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;

	vec3 spotDirection;     
    float spotExponent;        
    float spotCutoff;        // (range: [0.0,90.0], 180.0)   
    float spotCosCutoff;     // (range: [1.0,0.0],-1.0)    
                             
    float constantAttenuation;  
    float linearAttenuation;     
    float quadraticAttenuation;
};

uniform Lights lights[4];

struct MaterialProperties
{

   vec4 emission;   
   vec4 ambient;   
   vec4 diffuse;   
   vec4 specular;   
   float shininess; 
};

uniform MaterialProperties frontMaterial;
uniform MaterialProperties backMaterial;

struct LightDir
{
	vec3 LightsDir;
	vec3 HalfWay_Vector;
};

in LightDir LD[4];

struct spotL
{
	vec3 SD;
};

in spotL sD[1];

struct FogProperties
{
	int enable;
	float density;
	float start;
	float end;
	vec4  color;
	int   equation;
	float scale;
};

uniform FogProperties Fog;

in float FogFragCoord;

out vec4 colorOUT;

// ogl_replace computation
vec4 ogl_replace()
{
	return vec4(texture(gSampler, TexCoord[0].xy));
}

void DirectionalLight(const in int i,const in vec3 normal,inout vec4 ambient,inout vec4 diffuse,inout vec4 specular)
{
	float nDotVP;		// normal . light direction
	float nDotHV;		// normal . light half vector
	float pf;			// power factor
	
	nDotVP = max(0.0, dot(normal,normalize(LD[i].LightsDir)));
	nDotHV = max(0.0, dot(normal,normalize(LD[i].HalfWay_Vector)));

	if(nDotVP == 0.0)
	{
		pf = 0.0;
	}
	else
	{
		pf = pow(nDotHV, frontMaterial.shininess);
	}

	ambient += lights[i].ambient;
	diffuse += lights[i].diffuse * nDotVP; 
	specular += lights[i].specular * pf;
}

void PointLight(const in int i,const in vec3 eye,const in vec3 ecPosition3,const in vec3 normal,inout vec4 ambient,inout vec4 diffuse,inout vec4 specular)
{
	float nDotVP;			 // normal . light direction
	float nDotHV;			 // normal . light half vector
	float pf;				 // power factor
	float attenuation;		 // computed attenuation factor
	float d;				 // distance from surface to light source
	vec3 VP,VE;				 // direction from surface to light position
	vec3 halfVector;		 // direction of maximum highlights

	// Compute vector from surface to light position //VP = vec3(LightsPos) - ecPosition3;
	VP = LD[i].LightsDir;

	// Compute distance between surface and light position
	d = length(VP);

	// Normalize the vector from surface to light position
	VP = normalize(VP);
	VE = normalize(eye);

	// Compute attenuation

	attenuation = 1.0 / (lights[i].constantAttenuation  + 
						 lights[i].linearAttenuation    * d + 
						 lights[i].quadraticAttenuation * d * d);


	halfVector = normalize(VP + VE);

	nDotVP = max(0.0, dot(normal, VP));
	nDotHV = max(0.0, dot(normal, halfVector));


	if(nDotVP == 0.0)
	{
		pf = 0.0;
	}
	else
	{
		pf = pow(nDotHV, frontMaterial.shininess);
	}

	ambient  += lights[i].ambient  * attenuation;
	diffuse  += lights[i].diffuse  * nDotVP * attenuation;
	specular += lights[i].specular * pf * attenuation;
}

void SpotLight(const in int i,const in vec3 eye,const in vec3 ecPosition3,const in vec3 normal,inout vec4 ambient,inout vec4 diffuse,inout vec4 specular)
{
	float nDotVP; // normal . light direction
	float nDotHV; // normal . light half vector
	float pf; // power factor
	float spotDot; // cosine of angle between spotlight
	float spotAttenuation; // spotlight attenuation factor
	float attenuation; // computed attenuation factor
	float d; // distance from surface to light source
	vec3 VP,VE; // direction from surface to light position
	vec3 halfVector; // direction of maximum highlights

	// Compute vector from surface to light position->VP = vec3(lights[i].position) - ecPosition3;
	VP = LD[i].LightsDir;
	
	// Compute distance between surface and light position
	d = length(VP);

	// Normalize the vector from surface to light position
	VP = normalize(VP);
	VE = normalize(eye);

	// Compute attenuation
	attenuation = 1.0 / (lights[i].constantAttenuation +
					     lights[i].linearAttenuation * d +
                         lights[i].quadraticAttenuation * d * d);

	// See if point on surface is inside cone of illumination
	spotDot = dot(-VP,normalize(sD[i-3].SD));

	if (spotDot < lights[i].spotCosCutoff)
	{
		spotAttenuation = 0.0;					// light adds no contribution
	}
	else
	{
		spotAttenuation = pow(spotDot, lights[i].spotExponent);
	}

	// Combine the spotlight and distance attenuation.
	attenuation *= spotAttenuation;

	halfVector = normalize(VP + VE);

	nDotVP = max(0.0, dot(normal, VP));
	nDotHV = max(0.0, dot(normal, halfVector));

	if (nDotVP == 0.0)
	{
		pf = 0.0;
	}
	else
	{
		pf = pow(nDotHV, frontMaterial.shininess);
	}

	ambient  += lights[i].ambient  * attenuation;
	diffuse  += lights[i].diffuse  * nDotVP * attenuation;
	specular += lights[i].specular *   pf   * attenuation;
}

void main() 
{ 
	vec4 amb  = vec4(0.0);
	vec4 diff = vec4(0.0);
	vec4 spec = vec4(0.0);
	int activeLight = 0;

	for(int i = 0; i < 4; i++)
	{
		if(lights[i].activeLight == 1 && lights[i].position.w == 0.0)
		{
			DirectionalLight(i,normal,amb,diff,spec);
			activeLight++;
		}
		else if(lights[i].activeLight == 1 && lights[i].spotCutoff == 180.0)
		{
			PointLight(i,eye_V,ecPosition3,normal,amb,diff,spec);
			activeLight++;
		}
		else
		{
			if(lights[i].activeLight == 1)
			{
				SpotLight(i,eye_V,ecPosition3,normal,amb,diff,spec);
				activeLight++;
			}
		}

	}

	if(activeLight == 0)
	{
		colorOUT = ogl_replace();
	}
	else
	{
		colorOUT = (texture(gSampler, TexCoord[0].xy) * amb ) + (texture(gSampler, TexCoord[0].xy) * diff ) + (texture(gSampler, TexCoord[0].xy) * spec );
	}

	if(Fog.enable == 1)
	{
		float fog;

		if(Fog.equation == 0) // GL_LINEAR
		{
			fog = (Fog.end - FogFragCoord) * Fog.scale;
		}
		else if(Fog.equation == 1) // GL_EXP
		{
			fog = exp(-Fog.density * FogFragCoord);
		}
		else						// GL_EXP2
		{
			fog = exp(-Fog.density * Fog.density * FogFragCoord * FogFragCoord);
		}

		fog = clamp(fog, 0.0, 1.0);

		colorOUT = mix(Fog.color, colorOUT, fog);
	}
}



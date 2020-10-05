#version 150 core

in vec2 TexCoord;
in vec3 normal;
in vec3 ecPosition3;
in vec3 eye_V;
in float FogFragCoord;
in vec2 test;

out vec4 FragColor;

uniform sampler2D gSampler1; 

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

uniform struct FogProperties
{
	int enable;
	float density;
	float end;
	vec4 color;
	int equation;
	float scale;

}Fog;

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



void DirectionalLight(const in int i,const in vec3 normal,inout vec4 ambient,inout vec4 diffuse,inout vec4 specular,const in int mode,inout vec4 bamb,inout vec4 bdiff,inout vec4 bspec)
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
		if(mode == 1)
		{
			pf = pow(nDotHV, frontMaterial.shininess);
		}
		else
		{
			pf = pow(nDotHV, backMaterial.shininess);
		}
	}

	if(mode == 1)
	{
		ambient  += lights[i].ambient;
		diffuse  += lights[i].diffuse * nDotVP; 
		specular += lights[i].specular * pf;
	}
	else
	{
		bamb  += lights[i].ambient;
		bdiff += lights[i].diffuse * nDotVP;
		bspec += lights[i].specular * pf;
	}
}

void PointLight(const in int i,const in vec3 eye,const in vec3 ecPosition3,const in vec3 normal,inout vec4 ambient,inout vec4 diffuse,inout vec4 specular,const in int mode,inout vec4 bamb,inout vec4 bdiff,inout vec4 bspec)
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
	//VE = eye;
	
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
		if(mode == 1)
		{
			pf = pow(nDotHV, frontMaterial.shininess);
		}
		else
		{
			pf = pow(nDotHV, backMaterial.shininess);
		}
	}

	if(mode == 1)
	{
		ambient  += lights[i].ambient  * attenuation;
		diffuse  += lights[i].diffuse  * nDotVP * attenuation;
		specular += lights[i].specular * pf     * attenuation;
	}
	else
	{
		bamb  += lights[i].ambient  * attenuation;
		bdiff += lights[i].diffuse  * nDotVP * attenuation;
		bspec += lights[i].specular * pf     * attenuation;
	}
}

void SpotLight(const in int i,const in vec3 eye,const in vec3 ecPosition3,const in vec3 normal,inout vec4 ambient,inout vec4 diffuse,inout vec4 specular,const in int mode,inout vec4 bamb,inout vec4 bdiff,inout vec4 bspec)
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
	//spotDot = dot(-VP, normalize(vec3(MVl_mat * vec4(lights[i].spotDirection,1.0))));
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
		if(mode == 1)
		{
			pf = pow(nDotHV, frontMaterial.shininess);
		}
		else
		{
			pf = pow(nDotHV, backMaterial.shininess);
		}
	}

	if(mode == 1)
	{
		ambient  += lights[i].ambient  * attenuation;
		diffuse  += lights[i].diffuse  * nDotVP * attenuation;
		specular += lights[i].specular *   pf   * attenuation;
	}
	else
	{
		bamb  += lights[i].ambient  * attenuation;
		bdiff += lights[i].diffuse  * nDotVP * attenuation;
		bspec += lights[i].specular *   pf   * attenuation;
	}
}

void main() 
{ 
	int activeLight = 0;

	vec4 FrontColor;
	vec4 BackColor;

	vec4 amb  = vec4(0.0);
	vec4 diff = vec4(0.0);
	vec4 spec = vec4(0.0);

	vec4 BackAmbient = vec4(0.0);
	vec4 BackDiffuse = vec4(0.0);
	vec4 BackSpecular = vec4(0.0);

	for(int i = 0; i < 4; i++)
	{
		if(lights[i].activeLight == 1 && lights[i].position.w == 0.0)
		{
			DirectionalLight(i,normal,amb,diff,spec,1,BackAmbient,BackDiffuse,BackSpecular);
			DirectionalLight(i,-normal,amb,diff,spec,0,BackAmbient,BackDiffuse,BackSpecular);
			activeLight++;
		}
		else if(lights[i].activeLight == 1 && lights[i].spotCutoff == 180.0)
		{
			PointLight(i,eye_V,ecPosition3,normal,amb,diff,spec,1,BackAmbient,BackDiffuse,BackSpecular);
			PointLight(i,eye_V,ecPosition3,-normal,amb,diff,spec,0,BackAmbient,BackDiffuse,BackSpecular);
			activeLight++;
		}
		else
		{
			if(lights[i].activeLight == 1)
			{
				SpotLight(i,eye_V,ecPosition3,normal,amb,diff,spec,1,BackAmbient,BackDiffuse,BackSpecular);
				SpotLight(i,eye_V,ecPosition3,-normal,amb,diff,spec,0,BackAmbient,BackDiffuse,BackSpecular);
				activeLight++;
			}
		}

	}

	if(activeLight == 0)
	{
		FrontColor = texture(gSampler1, TexCoord.xy);
		BackColor  = texture(gSampler1, TexCoord.xy);
	}
	else
	{
		FrontColor = (texture(gSampler1, TexCoord.xy) * amb ) + (texture(gSampler1, TexCoord.xy) * diff ) + (texture(gSampler1, TexCoord.xy) * spec );
		BackColor  = (texture(gSampler1, TexCoord.xy) * BackAmbient ) + (texture(gSampler1, TexCoord.xy) * BackDiffuse ) + (texture(gSampler1, TexCoord.xy) * BackSpecular );
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

		FrontColor = mix(Fog.color, FrontColor, fog);
	}

	if(gl_FrontFacing)
	{
		FragColor = clamp(FrontColor,0.0,1.0);
	}
	else
	{
		FragColor = clamp(BackColor,0.0,1.0);
	}
} 

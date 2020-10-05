#version 150 core

in  vec2  TexCoord;

uniform sampler2D BaseImage;
uniform sampler2D BlendImage;

uniform float Opacity;
uniform int	  Mode;
uniform int	  Opacity_Mode;
uniform float noiseScale;

const vec4 white	= vec4(1.0   , 1.0   , 1.0   , 1.0);
const vec4 lumCoeff = vec4(0.2125, 0.7154, 0.0721, 1.0);

out vec4 FragColor;

void main() 
{ 
	vec4 base = texture(BaseImage, TexCoord.xy);
	vec4 blend = texture(BlendImage, TexCoord.xy);

	vec4 result = vec4(0.0,0.0,0.0,1.0);

	if(Mode == 0)
	{
		result = blend;															// Normal 
	}
	else if(Mode == 1)
	{
		result = (base + blend) * 0.5;											// Average 
	}
	else if(Mode == 2)
	{
		float noise = (noise1(vec2(TexCoord * noiseScale)) + 1.0) * 0.5;	   // Dissolve
		result = (noise < Opacity) ? blend : base;
	}
	else if(Mode == 3)
	{
		result = (base.a == 0.0) ? blend : base;							   // Behind (only if there is any transparent values in the base image) 
	}
	else if(Mode == 4)
	{
		result.rgb = blend.rgb;												   // Clear
		result.a = 0.0;
	}
	else if(Mode == 5)
	{
	    result = min(blend, base);											   // Darken
	}
	else if(Mode == 6)
	{
		result = max(blend, base);											   // Lighten
	}
	else if(Mode == 7)
	{
		result = blend * base;												   // Multiply
	}
	else if(Mode == 8)
	{
		result = white - ((white - blend) * (white - base));				   // Screen
	}
	else if(Mode == 9)
	{
		result = white - (white - base) / blend;							   // Color Burn
	}
	else if(Mode == 10)
	{
		result = base / (white - blend);									   // Color Dodge
	}
	else if(Mode == 11)
	{
		float luminance = dot(base, lumCoeff);
		if(luminance < 0.45)
		{
			result = 2.0 * blend * base;
		}
		else if(luminance > 0.55)
		{
			result = white - 2.0 * (white - blend) * (white - base);			// Overlay
		}
		else
		{
			vec4 result1 = 2.0 * blend * base;
			vec4 result2 = white - 2.0 * (white - blend) * (white - base);
			result = mix(result1, result2, (luminance - 0.45) * 10.0);
		}
	}
	else if(Mode == 12)
	{
		result = 2.0 * base * blend + base * base - 2.0 * base * base * blend;  // Soft Light
	}
	else if(Mode == 13)
	{
		float luminance = dot(blend, lumCoeff);
		if(luminance < 0.45)
		{
			result = 2.0 * blend * base;
		}
		else if(luminance > 0.55)
		{
			result = white - 2.0 * (white - blend) * (white - base);			// Hard Light
		}
		else
		{
			vec4 result1 = 2.0 * blend * base;
			vec4 result2 = white - 2.0 * (white - blend) * (white - base);
			result = mix(result1, result2, (luminance - 0.45) * 10.0);
		}
	}
	else if(Mode == 14)
	{
		result = blend + base;													// Add
	}
	else if(Mode == 15)
	{
		result = base - blend;													// Subtract
	}
	else if(Mode == 16)
	{
		result = abs(blend - base);												// Difference
	}
	else if(Mode == 17)
	{
		result = white - abs(white - base - blend);								// Inverse Difference
	}
	else
	{
		result = base + blend - (2.0 * base * blend);							// Exclusion
	}

	if(Opacity_Mode == 1)
	{
		FragColor = mix(base, result, Opacity);									// Opacity
	}
	else
	{
		FragColor = result;
	}
} 


#version 150 core

in vec2 TexCoord;

uniform vec3 Color0;
uniform vec3 Color1;
uniform vec3 AvgColor;
uniform float Frequency;

uniform int with_aliasing;

out vec4 FragColor;

// function for getting the checker pattern without antialiasing
vec4 checker(vec2 uv)
{
  float fmodResult = mod(floor(Frequency * 2.0 * uv.x) + floor(Frequency * 2.0 * uv.y), 2.0);

  if (fmodResult < 1.0)
  {
	return vec4(0, 0, 0, 1);
  }
  else
  {
    return vec4(1, 1, 1, 1);
  }
}

void main()
{
	vec3 color;

	// Check which pattern will follow aliasing or antialiasing
	if(with_aliasing == 1)
	{
		color = checker(TexCoord).rgb;
	}
	else
	{
		// Determine the width of the projection of one pixel into s-t space
		vec2 fw = fwidth(TexCoord);


		// Determine the amount of fuzziness
		vec2 fuzz = fw * Frequency * 2.0;

		float fuzzMax = max(fuzz.s, fuzz.t);

		// Determine the position in the checkerboard pattern
		vec2 checkPos = fract(TexCoord * Frequency);

		if (fuzzMax < 0.5)
		{
			// If the filter width is small enough,
			// compute the pattern color
			vec2 p = smoothstep(vec2(0.5), fuzz + vec2(0.5), checkPos) + (1.0 - smoothstep(vec2(0.0), fuzz, checkPos));
				
			color = mix(Color0, Color1,p.x * p.y + (1.0 - p.x) * (1.0 - p.y));

			// Fade in the average color when we get close to the limit
			color = mix(color, AvgColor, smoothstep(0.125, 0.5, fuzzMax));
		}
		else
		{
			// Otherwise, use only the average color
			color = AvgColor;
		}
	}
	

	FragColor = vec4(color, 1.0);
}

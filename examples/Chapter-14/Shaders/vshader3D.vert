#version 150 core

in vec4 MCVertex;
in vec3 MCNormal;
in vec3 MCTangent; // parallel to grating lines at each vertex

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

uniform vec3 LightPosition;
uniform float GratingSpacing;
uniform float SurfaceRoughness;
uniform vec4 HighlightColor;

out vec4 Color;

// map a visible wavelength [nm] to OpenGL's RGB representation

vec3 lambda2rgb(float lambda)
{
	const float ultraviolet = 400.0;
	const float infrared = 700.0;

	// map visible wavelength range to 0.0 -> 1.0
	float a = (lambda-ultraviolet) / (infrared-ultraviolet);

	// bump function for a quick/simple rainbow map
	const float C = 7.0; // controls width of bump
	vec3 b = vec3(a) - vec3(0.75, 0.5, 0.25);

	return max((1.0 - C * b * b), 0.0);
}

void main()
{
	// extract positions from input uniforms
	vec3 lightPosition = LightPosition.xyz;
	vec3 eyePosition = -MVMatrix[3].xyz / MVMatrix[3].w;

	// H = halfway vector between light and viewer from vertex
	vec3 P = vec3(MVMatrix * MCVertex);
	vec3 L = normalize(lightPosition - P);
	vec3 V = normalize(eyePosition - P);
	vec3 H = L + V;

	// accumulate contributions from constructive interference
	// over several spectral orders.
	vec3 T = NormalMatrix * MCTangent;
	float u = abs(dot(T, H));

	vec3 diffColor = vec3(0.0);

	const int numSpectralOrders = 3;
	for (int m = 1; m <= numSpectralOrders; ++m)
	{
		float lambda = GratingSpacing * u / float(m);
		diffColor += lambda2rgb(lambda);
	}

	// compute anisotropic highlight for zero-order (m = 0) reflection.
	vec3 N = NormalMatrix * MCNormal;
	float w = dot(N, H);
	float e = SurfaceRoughness * u / w;
	vec3 hilight = exp(-e * e) * HighlightColor.rgb;

	// write the values required for fixed-function fragment processing
	const float diffAtten = 0.8; // attenuation of the diffraction color
	Color = vec4(diffAtten * diffColor + hilight, 1.0);

	gl_Position = MVPMatrix * MCVertex;
}
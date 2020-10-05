#version 150 core

#define PI 3.14159265

in float LightIntensity;
in vec3 MCposition;

uniform sampler3D Noise;
uniform vec3 SkyColor;
uniform vec3 CloudColor;

out vec4 FragColor;

void main()
{
	vec4 noisevec = texture(Noise, MCposition);

	float t = (cos( noisevec.r * PI ) + 1.0) / 2.0;
	t = clamp(t-0.25,0.0,1.0);

	vec3 color = mix(SkyColor, CloudColor, t);
	FragColor  = vec4(color,1.0);
}


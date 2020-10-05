#version 150 core

#define MAX_WAVES 32

in vec2 Position;

struct CWave
{
	float StartTime, Speed, MaxY, FrequencyMPIM2;
	vec2 Position;
};

uniform float Time;
uniform CWave Waves[MAX_WAVES];

out vec4 FragColor;

void main()
{
	float y = 0.0;
	
	for(int i = 0; i < MAX_WAVES; i++)
	{
		float d = distance(Waves[i].Position, Position);
		float t = max(Time - Waves[i].StartTime - d / Waves[i].Speed, 0.0);
		float maxy = max(Waves[i].MaxY - Waves[i].MaxY * t, 0.0);
		y -= sin(t * Waves[i].FrequencyMPIM2) * maxy / (1.0 + d);
	}

	FragColor = vec4(0.0, 1.0, 0.0, y);
}

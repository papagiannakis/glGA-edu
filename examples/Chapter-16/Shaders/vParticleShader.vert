#version 150 core

in vec3 MCVertex;
in vec3 Velocity;		 // initial velocity
in float LifeTime;		 // LifeTime of each particle

in float Speed;

uniform float Time;		 // updated each frame by the application
uniform vec4 Background; // constant color equal to background

uniform mat4 MVPMatrix;

out vec4 Color;
out float STime;
out float LT;

void main()
{
	vec4 vert;
	float t = Time - 0.1;

	if(Time >= 30.0)
	{
		vert = vec4(MCVertex,1.0) + vec4(Velocity*t*Speed,0.0);	// Initial position
	}
	else
	{
		vert = vec4(MCVertex*0.001*Time,1.0);
	}

	gl_Position = MVPMatrix * vert;
	STime = t;
	LT = LifeTime;
}
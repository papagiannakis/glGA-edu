#version 150 core

in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;

uniform mat4 modelMatrix;
uniform mat4 translate;

void main()
{
	//Uncomment if you want to see the basic cube translated.
    //gl_Position = translate*vPosition;

	gl_Position = vPosition;
    color = vColor;
}

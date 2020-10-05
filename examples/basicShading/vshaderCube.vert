#version 150 core

in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;

uniform mat4 translate;

void main()
{
	// uncomment to witness the effect of translation to the left on X by -0.5
    //gl_Position = translate * vPosition;
    
    // uncomment to have the cube centered on 0.0.0
    gl_Position =  vPosition;
    color = vColor;
}

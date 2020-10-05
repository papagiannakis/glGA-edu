#version 150 core

in  vec4 MCvertex;

out vec2 TexCoord;

void main() 
{
	TexCoord = (MCvertex.xy + vec2(1.0,1.0) ) / 2;
    //MCvertex.y=1;
	gl_Position = MCvertex;
}

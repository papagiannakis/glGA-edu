#version 150 core

in  vec4 MCvertex;

uniform float WMS, WMSD2;

out vec2 Position;

void main() 
{
	Position = (MCvertex.xy + vec2(1.0,1.0) ) / 2;

	Position = Position.xy * WMS - WMSD2;	// position of water plane

	gl_Position = MCvertex;
}


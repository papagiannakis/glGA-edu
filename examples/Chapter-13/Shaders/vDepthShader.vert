#version 150 core

in vec3 MCvertex;

uniform mat4 depthMVP;
 
void main(){

	gl_Position =  depthMVP * vec4(MCvertex,1.0);

}
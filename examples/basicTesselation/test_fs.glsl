#version 410

in vec3 colour; // computed in the evaluation shader
out vec4 fragcolour;

void main () {
	fragcolour = vec4 (colour, 1.0);
}

#version 430
in vec2 st;
uniform sampler2D img;
out vec4 fc;

void main () {
  fc = texture (img, st);
}

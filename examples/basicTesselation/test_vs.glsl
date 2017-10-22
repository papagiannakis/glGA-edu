#version 410

//input: control points
in vec3 vp_loc;
//output control points to the tessellation control shader
out vec3 controlpoint_wor;

void main() {
	controlpoint_wor = vp_loc; // control points out == vertex points in
}

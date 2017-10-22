#version 410

// number of Control Points (CPs) in patch
layout (vertices = 3) out;

//The TCS takes an input patch and emits an output patch. The developer has the option in the shader to do some transformation on the CPs or even add/delete CPs. In addition to the output patch the control shader calculates a set of numbers called Tessellation Levels (TL).

//use any algorithm in order to calculate the TLs.

// from VS (use empty modifier [] so we can say anything)
in vec3 controlpoint_wor[];

// to evluation shader. will be used to guide positioning of generated points
out vec3 evaluationpoint_wor[];

//inner level: controls the number of nested primitives
//outer level: controls the number of times to subdivide each edge
uniform float tess_fac_inner = 5.0;
uniform float tess_fac_outer = 3.0;

void main () {
	evaluationpoint_wor[gl_InvocationID] = controlpoint_wor[gl_InvocationID];

	// Calculate the tessellation levels
	gl_TessLevelInner[0] = tess_fac_inner; // number of nested primitives to generate
	gl_TessLevelOuter[0] = tess_fac_outer; // times to subdivide first side
	gl_TessLevelOuter[1] = tess_fac_outer; // times to subdivide second side
	gl_TessLevelOuter[2] = tess_fac_outer; // times to subdivide third side
}

//Primitive generator output: barycentric coordinates and their connectivity

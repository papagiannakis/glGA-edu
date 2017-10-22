#version 150 core

//The geometry shader program can call two special functions to generate primitives, EmitVertex and EndPrimitive. Each time the program calls EmitVertex, a vertex is added to the current primitive. When all vertices have been added, the program calls EndPrimitive to generate the primitive.

//the input of the geometry shader is points
layout(points) in;

//the output is lines
//the maximum number of vertices (max_vertices) per primitive is 64.
layout(line_strip, max_vertices = 64) out;

in vec3 vColor[];
in float vSides[];
out vec3 fColor;

const float PI = 3.1415926;

void main()
{
    fColor = vColor[0];

	// Safe, floats can represent small integers exactly
	for (int i = 0; i <= vSides[0]; i++) {
		// Angle between each side in radians
		float ang = PI * 2.0 / vSides[0] * i;

        // Offset from center of point (0.3 to accomodate for aspect ratio)
        vec4 offset = vec4(cos(ang) * 0.3, -sin(ang) * 0.4, 0.0, 0.0);
        
        //The gl_Position, as set in the vertex shader, can be accessed using the gl_in array
        gl_Position = gl_in[0].gl_Position + offset;

        EmitVertex();
    }

    //complete the current output primitive on the first vertex stream
    EndPrimitive();
}

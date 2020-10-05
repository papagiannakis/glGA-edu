#version 330 core                                                                       

layout (location = 0) in vec3 vPosition; 

uniform mat4 ModelView;
uniform mat4 Projection;


void main()
{          
    gl_Position = Projection * ModelView * vec4(vPosition, 1.0);
}

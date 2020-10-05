#version 330                                                                        
                                                                                    
layout (location = 0) in vec3 vPosition;                                             
layout (location = 1) in vec2 vTexCoord;                                             
layout (location = 2) in vec3 vNormal;                                               

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat4 Model;
                                        
out vec2 TexCoord0;                                                                 
out vec3 Normal0;                                                                   
out vec3 WorldPos0;                                                                 


void main()
{       
    gl_Position    = Projection * ModelView * vec4(vPosition, 1.0);
    TexCoord0      = vTexCoord;                  
    Normal0        = (Projection * ModelView * vec4(vNormal, 0.0)).xyz;   
    WorldPos0      = (Model * vec4(vPosition, 1.0)).xyz;
}
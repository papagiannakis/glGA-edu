#version 330 core

in vec4 color;
in vec2 texCoord;
out  vec4  colorOUT;

in vec3 Normal0; 
in vec3 WorldPos0; 

layout (location = 0) out vec3 WorldPosOut; 
layout (location = 1) out vec3 DiffuseOut; 
layout (location = 2) out vec3 NormalOut; 
layout (location = 3) out vec3 TexCoordOut; 


uniform sampler2D mesh_sampler;



void main() 
{ 

	WorldPosOut = WorldPos0; 
    DiffuseOut = texture(mesh_sampler, texCoord).xyz; 
    NormalOut = normalize(Normal0); 
    TexCoordOut = vec3(texCoord, 0.0); 

    //colorOUT = texture2D(mesh_sampler,texCoord)*color;
	//colorOUT.a = 1.0;
    //colorOUT = vec4(0.0,1.0,0.0,1.0);
} 

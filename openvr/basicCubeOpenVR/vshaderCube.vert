#version 410 core


in  vec4 vPosition;
in  vec4 vColor;
in  vec2 vTexCoord;


out vec2 texCoord;
out vec4 color;


uniform mat4 translate;
uniform mat4 modelViewProjection;

void main()
{
    
    // uncomment to witness the effect of translation to the left on X by -0.5
    //gl_Position = translate * vPosition;
    
    // uncomment to have the cube centered on 0.0.0
    gl_Position =  modelViewProjection * vPosition;

	texCoord = vTexCoord;
    color = vColor;
}
#version 150 core


in      vec3    vPosition;
//in      vec4    vColor;
in      vec3    vNormal;
in      vec2    vTexCoord;

out     vec4    color;
out     vec2    texCoord;
out		vec3	Normal0; 
out		vec3	WorldPos0; 

uniform vec4    COLOR;
uniform mat4    Model;
uniform mat4    ModelView;
uniform mat4    Projection;
uniform vec4    AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4    LightPosition;
uniform float   Shininess;



void    main()
{
    
    // Transform vertex position into eye coordinates
    vec3    pos = (ModelView * vec4(vPosition.x,vPosition.y,vPosition.z,1.0)).xyz;
    // calculate L in light space from Light position - vertex position (eye space)
    vec3    L = normalize(LightPosition.xyz - pos);
    // assume constant viewer at 0,0,0 (in eye space)
    vec3    E = normalize(0-pos);
    // calculate half angle vector
    vec3    H = normalize((L+E)/2);
    
    // for the rest of our Phong calculations we will need the Normal into eye space as well
    // only correct if there is no scale in MV, otherwise we use the InverseTranspose of MV
    vec3    N = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
    
    // compute terms in the illumination equation
    vec4    ambientTerm = AmbientProduct;
    
    // compute diffuse Phong Term 
    float   diffuseDotProduct = max( dot(L,N), 0.0 ); //calculate >0 only +ve values
    vec4    diffuseTerm = diffuseDotProduct * DiffuseProduct;
    
    // compute specular Phong Term
    float   specularDotProduct = pow( max(dot(N,H), 0.0), Shininess);
    vec4    specularTerm = specularDotProduct * SpecularProduct;
    
    //special provision for dark side highlights
    if (dot(L,N) < 0.0) {
        
        specularTerm = vec4(0.0,0.0,0.0,1.0);
    }
    // outpout per-vertex position in Clip space
    vec4 pos4 = vec4(vPosition.x,vPosition.y,vPosition.z,1.0);
    
    gl_Position = Projection * ModelView * pos4;

	Normal0 = (Model * vec4(vNormal, 0.0)).xyz;
	WorldPos0 = (Model * vec4(vPosition, 1.0)).xyz;
    
    // output lighted via the Phong algorithm each vertex color
    //color       = (ambientTerm + diffuseTerm + specularTerm) * vColor;
	//color       = (ambientTerm + diffuseTerm + specularTerm) * COLOR;
    color       = (ambientTerm + diffuseTerm + specularTerm);
    // pass to fragment shader per-vertex tex coords
    texCoord    = vTexCoord;
    
}
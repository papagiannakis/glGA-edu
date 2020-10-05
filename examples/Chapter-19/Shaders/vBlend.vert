#version 150 core

in  vec4 MCvertex;

out vec2 TexCoord;

mat4 RotateMatrix( const in float angle,
const in vec3 axis )
{
	vec3 n = normalize( axis );
	float theta = radians( angle );
	float c = cos( theta );
	float s = sin( theta );

	mat3 R;
	R[0] = n.xyz*n.x*(1.0-c) + vec3( c, n.z*s, -n.y*s );
	R[1] = n.xyz*n.y*(1.0-c) + vec3( -n.z*s, c, n.x*s );
	R[2] = n.xyz*n.z*(1.0-c) + vec3( n.y*s, -n.x*s, c );

	return mat4( R[0], 0.0,R[1], 0.0,R[2], 0.0,0.0, 0.0, 0.0, 1.0 );
}

void main() 
{
  gl_Position = RotateMatrix(180.0,vec3(1.0,0.0,0.0)) * MCvertex;

  TexCoord = (MCvertex.xy+vec2(1,1))/2.0;
} 
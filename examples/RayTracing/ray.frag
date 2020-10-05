#version 410
uniform vec2 resolution;
uniform float time;

out vec4 frag_colour;

#define EPSILON 0.5


float iPlane(in vec3 ro, in vec3 rd){
	return -ro.y/rd.y;
}

float iPlane2(in vec3 ro, in vec3 rd, in vec3 planeNormal, in vec3 planePoint){
	
	if(dot(planeNormal, rd ) > 0.0)
	{
		float t1 = -dot(planeNormal, (ro - planePoint)) / dot(planeNormal,rd);
		return t1;
	}
	else
		return -1.0; //no intersection or intersection behind camera
}

float iSphere(in vec3 ro, in vec3 rd, in vec4 sph){
	vec3 oc = ro - sph.xyz;
	float b = 2.0*dot(oc,rd);
	float c = dot(oc,oc) - sph.w*sph.w;
	float h = b*b - 4.0*c;
	if(h<0.0) return -1.0;
	float t = (-b - sqrt(h))/2.0;
	return t;
}

float iEllipse(in vec3 ro, in vec3 rd, in vec4 ell, in vec3 ellcenter){
	
	vec3 O_C = ro-ellcenter;
	vec3 dir = rd;
	normalize( dir );

	float a =
        ((dir.x*dir.x)/(ell.x*ell.x))
      + ((dir.y*dir.y)/(ell.y*ell.y))
      + ((dir.z*dir.z)/(ell.z*ell.z));
	float b =
        ((2.0*O_C.x*dir.x)/(ell.x*ell.x))
      + ((2.0*O_C.y*dir.y)/(ell.y*ell.y))
      + ((2.0*O_C.z*dir.z)/(ell.z*ell.z));
	float c =
        ((O_C.x*O_C.x)/(ell.x*ell.x))
      + ((O_C.y*O_C.y)/(ell.y*ell.y))
      + ((O_C.z*O_C.z)/(ell.z*ell.z))
      - ell.w*ell.w;

	float d = ((b*b)-(4.0*a*c));
	if ( d<0.0 || a==0.0 || b==0.0 || c==0.0 ) 
   	return -1.0;

	d = sqrt(d); 

	float t1 = (-b+d)/(2.0*a);
	float t2 = (-b-d)/(2.0*a);

	if( t1<=0.3 && t2<=0.3 ) return -1.0; // both intersections are behind the ray origin
	float t=0.0;
	if( t1<=EPSILON ) 
   		t = t2;
	else if( t2<=EPSILON )
      		t = t1;
   	else
      	t=(t1<t2) ? t1 : t2;
	return t;
}

float diffuseFactor(in vec3 nor, in vec3 light, in vec4 obj){
    vec3 L = normalize(light - obj.xyz);
    float dif = max(dot(L, nor),0.0);
    return dif;
}


float specularFactor(in vec3 nor,  in vec3 light, in vec4 obj)
{
    vec3 L = normalize(light - obj.xyz);
    vec3 E = normalize(-obj.xyz);
    vec3 halfAngle = normalize((L+E)/2.0);
    float ks = pow(max(dot(nor, halfAngle), 0.0), 100.0);
    
    return ks;
}

vec4 sph1 = vec4(0.0, 1.0, 0.0, 1.0);
vec4 sph2 = vec4(0.3, 1.4, 1.0, 0.2);
vec4 sph3 = vec4(-0.3, 1.4, 1.0, 0.2);
vec4 sph4 = vec4(0.2, 1.2, 2.0, 0.05);
vec4 sph5 = vec4(-0.2, 1.2, 2.0, 0.05);
vec4 ell = vec4(2.5, 1.5, 1.0, 0.1);
vec3 ellcenter = vec3(0.0, 0.7, 2.0);

float intersect(in vec3 ro, in vec3 rd, out float resT){
	resT = 1000.0;
	float tsph = iSphere(ro, rd, sph1);
	float tpla =  iPlane(ro,rd);
	float tpla2 = iPlane2(ro,rd, vec3(50.0, 0.0, 30.0), vec3( 1.0, 1.0, 3.0));
	float tpla3 = iPlane2(ro,rd, vec3(-50.0, 0.0, 30.0), vec3( -1.0, 1.0, 3.0));
	float tpla4 = iPlane2(ro,rd, vec3(10.0, 1.0, -10.0), vec3( 100.0, 1000.0, 3.0));
	float tsph2 = iSphere(ro, rd, sph2);
	float tsph3 = iSphere(ro, rd, sph3);
	float tsph4 = iSphere(ro, rd, sph4);
	float tsph5 = iSphere(ro, rd, sph5);
	float tsph6 = iEllipse(ro, rd, ell, ellcenter);
	float id = -1.0;
	if(tsph>0.0){
		id = 1.0;
		resT = tsph;
	}
	if(tpla>0.0 && tpla<resT){
		id = 2.0;
		resT = tpla;
	}
	if(tsph2>0.0 && tsph2<resT){
		id = 3.0;
		resT = tsph2;
	}
	if(tsph3>0.0 && tsph3<resT){
		id = 4.0;
		resT = tsph3;
	}
	if(tsph4>0.0 && tsph4<resT){
		id = 5.0;
		resT = tsph4;
	}
	if(tsph5>0.0 && tsph5<resT){
		id = 6.0;
		resT = tsph5;
	}
	if(tpla2>0.0 && tpla2<resT){
		id = 7.0;
		resT = tpla2;
	}
	if(tpla3>0.0 && tpla3<resT){
		id = 8.0;
		resT = tpla3;
	}
	if(tpla4>0.0 && tpla4<resT){
		id = 9.0;
		resT = tpla4;
	}
	if(tsph6>0.0 && tsph6<resT){
		id = 10.0;
		resT = tsph6;
	}
	
	return id;
}

vec3 nSphere(in vec3 pos, in vec4 sph){
	return (pos - sph.xyz)/sph.w;
}


vec3 nPlane(in vec3 pos){
	return vec3(0.0, 1.0, 0.0);
}

vec3 nPlane2(in vec3 pos){
	return vec3(2.0, 1.0, 3.0);
}


void main( void ) {
	
	vec3 light = vec3(1.0, 15.0, 4.0);
	
	vec2 uv = (gl_FragCoord.xy/resolution.xy);
	
	sph1.x = 0.5 * cos(time);
	sph1.z = 0.5 * sin(time);

	sph2.x = sph1.x + 0.3;
	sph2.z = sph1.z + 0.9;
	
	sph3.x = sph1.x - 0.3;
	sph3.z = sph1.z + 0.9;
	
	sph4.x = (sph2.x)*0.5 + 0.04;
	sph4.z = sph2.z  + 1.2;
	
	sph5.x = (sph3.x)*0.5 - 0.04;
	sph5.z = sph3.z + 1.2;
	
	ellcenter.x = sph1.x;
	ellcenter.z = sph1.z + 1.0;
	ell.y = 0.8 * cos(time*10.0) + 1.0;
	
	
	vec3 col = vec3(0.0, 0.0, 0.0);
		
	vec3 ro = vec3(0.0, 1.0, 4.0);
 	vec3 rd = normalize(vec3(-1.0 + 2.0*uv, -1.0));
 
	float t;
	
 	float id = intersect(ro, rd, t);
 
 	if(id>=0.5 && id<1.5){
		vec3 pos = ro + t*rd;
		vec3 nor = nSphere(pos, sph1);
		float dif = diffuseFactor(nor, light, sph1);
		float s = specularFactor(nor, light, sph1);
		float amb = 0.2;
		float lighting = amb + dif + s;
		col = vec3(1.0, 1.0, 0.0) * lighting;
 	}
	else if(id>=1.5 && id<2.5){
		vec3 pos = ro + t*rd;
		vec3 nor = nPlane(pos);
		float dif = clamp(dot(nor, normalize(light)), 0.0, 1.0);
		float amb = smoothstep(0.0, 2.0*sph1.w, length(pos.xz - sph1.xz));
		if(sph1.y>=0.1){
			col = vec3(amb*0.7);
		}
		else{
			col = vec3(0.7);
		}
	}
	else if(id>=2.5 && id<3.5){
		vec3 pos = ro + t*rd;
		vec3 nor = nSphere(pos, sph2);
		float dif = diffuseFactor(nor, light, sph2);
		float s = specularFactor(nor, light, sph2);
		float amb = 0.2;
		float lighting = amb + dif + s;
		col = vec3(1.0, 1.0, 1.0)*lighting;
	}
	else if(id>=3.5 && id<4.5){
		vec3 pos = ro + t*rd;
		vec3 nor = nSphere(pos, sph3);
		float dif = diffuseFactor(nor, light, sph3);
		float s = specularFactor(nor, light, sph3);
		float amb = 0.2;
		float lighting = amb + dif + s;
		col = vec3(1.0, 1.0, 1.0) * lighting;
	}
	else if(id>=4.5 && id<5.5){
		vec3 pos = ro + t*rd;
		vec3 nor = nSphere(pos, sph4);
		float dif = diffuseFactor(nor, light, sph4);
		float s = specularFactor(nor, light, sph4);
		float amb = 0.2;
		float lighting = amb + dif + s;
		col = vec3(0.0, 0.0, 0.0) * lighting;
	}
	else if(id>=5.5 && id<6.5){
		vec3 pos = ro + t*rd;
		vec3 nor = nSphere(pos, sph5);
		float dif = diffuseFactor(nor, light, sph5);
		float s = specularFactor(nor, light, sph5);
		float amb = 0.2;
		float lighting = amb + dif + s;
		col = vec3(0.0, 0.0, 0.0) * lighting;
	}
	else if(id>=6.5 && id<7.5){
		float dif = 0.1;
		float amb = 0.2;
		col = vec3(1.0,0.0,0.0) * dif + vec3(1.0, 0.0, 0.0) * amb;
	}
	else if(id>=7.5 && id<8.5){
		float dif = 0.1;
		float amb = 0.2;
		col = vec3(0.0,1.0,0.0) * dif + vec3(0.0, 1.0, 0.0) * amb;
	}
	else if(id>=8.5 && id<9.5){
		float dif = 0.1;
		float amb = 0.2;
		col = vec3(0.0,0.0,1.0) * dif + vec3(0.0, 0.0, 1.0) * amb;
	}
	else if(id>=9.5){
		vec3 nor = t - ellcenter;
		nor.x = 2.0*nor.x/(ell.x*ell.x);
		nor.y = 2.0*nor.y/(ell.y*ell.y);
		nor.z = 2.0*nor.z/(ell.z*ell.z);
		float dif = diffuseFactor(nor, light, vec4(ellcenter,1.0));
		float s = specularFactor(nor, light, vec4(ellcenter,1.0));
		float amb = 0.2;
		float lighting = amb + dif + s;
		col = vec3(0.0, 0.0, 0.0) *lighting;
	}
 	col = sqrt(col);
	
	frag_colour = vec4(col, 1.0);

}

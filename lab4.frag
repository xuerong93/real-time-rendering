varying vec3 N;
varying vec3 L;
varying vec3 R;
varying vec3 V;
varying vec4 pcolor;

uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

uniform vec4 mat_ambient;
uniform vec4 mat_diffuse;
uniform vec4 mat_specular;
uniform float mat_shine;
uniform float lightSource;

//
// this shader just passes the interpolated fragment color along 
//
void main() { 

	   vec4 ambient = light_ambient * mat_ambient; 

	   float NdotL;
	   if(dot(N,L)<0.0) NdotL=0.0;
	   else NdotL=dot(N,L);
       vec4 diffuse = light_diffuse * mat_diffuse * NdotL;

	   float RdotV=dot(R,V);
	   if(NdotL==0.0) RdotV=0.0;
	   if(RdotV<0.0) RdotV=0.0;
	   vec4 specular=light_specular*mat_specular*pow(dot(R,V),mat_shine);  

	   if (lightSource==1){
			gl_FragColor =pcolor;
	   }
	   else{
			gl_FragColor =ambient + diffuse+ specular; 
	   }
	
 } 
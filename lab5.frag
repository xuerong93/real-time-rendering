uniform int use_texture;

varying vec3 N;
varying vec3 L;
varying vec3 R;
varying vec3 V;
varying vec4 pcolor;
varying vec4 pos_in_eye;
varying vec2 FtexCoord;

uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

uniform vec4 mat_ambient;
uniform vec4 mat_diffuse;
uniform vec4 mat_specular;
uniform float mat_shine;
uniform float lightSource;


uniform mat4 eye2world;   
uniform sampler2D Tex1;
uniform samplerCube cubeMap;



void main() { 
		vec4 Lighting;
		vec4 ambient = light_ambient * mat_ambient; 
		

		float NdotL;
		if(dot(N,L)<0.0) NdotL=0.0;
		else NdotL=dot(N,L);
		vec4 diffuse = light_diffuse * mat_diffuse * NdotL;

		float RdotV=dot(R,V);
		if(NdotL==0.0) RdotV=0.0;
		if(RdotV<0.0) RdotV=0.0;
		vec4 specular=light_specular*mat_specular*pow(dot(R,V),mat_shine);  

		Lighting =ambient + diffuse+ specular; 

		vec4 texcolor;
		vec4 env_color=vec4(1,1,1,1);
		vec3 ref,view_vector;

		if(use_texture==0){
			if (lightSource==1){
				gl_FragColor =pcolor;
			}
			else{
				gl_FragColor =Lighting; 
			}
		}
		
		if(use_texture==1){
			texcolor=texture2D(Tex1,FtexCoord);

			if (lightSource==1){
				gl_FragColor =pcolor;
			}
			else{
				gl_FragColor =texcolor*Lighting; 
			}
		}

		if(use_texture==2){
			view_vector=normalize(vec3(vec4(0,0,0,1)-pos_in_eye));/////eye space
			ref=normalize(reflect(-view_vector,N));///////////N is in eye space                   ////////////////////////view_vector is -
			ref=vec3(eye2world*vec4(ref,0));
			ref[1]=-ref[1];
			env_color=textureCube(cubeMap,ref);
	
			if (lightSource==1){
				gl_FragColor =pcolor;
			}
			else{
				gl_FragColor =env_color;
			}

		}


	
 } 
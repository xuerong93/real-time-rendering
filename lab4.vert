
attribute vec4 position; 
attribute vec4 color;
attribute vec4 normal; 

varying vec3 N;
varying vec3 L;
varying vec3 R;
varying vec3 V;
varying vec4 pcolor;

uniform mat4 local2clip;
uniform mat4 local2eye;
uniform mat4 normal_matrix;
uniform mat4 world2eye; 

uniform vec4 light_pos;

void main(){
	   pcolor=color;
       gl_Position = local2clip * position;
       
       N = normalize(vec3(normal_matrix * normal)); 
       vec4 Lpos =  world2eye * light_pos; 
       vec4 Vpos =  local2eye * position; 
       L = normalize(vec3(Lpos - Vpos)); 
	   R=normalize(reflect(-L,N));
	   V=normalize(vec3(-Vpos));







}
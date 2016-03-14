//////////////////////////////////////////////////////////////////////////
////////////////5543 RT-Rending lab4  done by Xuerong Hu//////////////////
///////////////////finished by 11/18/2015/////////////////////////////////
/////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack> 
#include <math.h> 
#include <iostream>
#include <math.h>
#define Pi 3.1415926
using namespace std;
#ifdef __APPLE__
#include <GLUT/glut.h> 
#include <OpenGL/gl.h> 
#else 
#include <glew.h>
#include <GL/glut.h> 
#include <GL/gl.h> 
#endif
#include<glm.hpp> 
#include<gtx/transform.hpp> 
#define SetMaterialColor(d,r,g,b,a)  glUniform4f(d,r,g,b,a)
//////////glut mouse control/////////
int press_x, press_y;
int release_x, release_y;
int xform_mode = 0;
float x_angle = 0.0;
float y_angle = 0.0;
float scale_size = 3;
#define XFORM_NONE    0 
#define XFORM_ROTATE  1
#define XFORM_SCALE 2 
bool WIRE_FRAME = false;
float b = 0.05f;   //total scaling

typedef struct
{	float location[4];
	float normal[4];
	float color[4];
} Vertex;

GLuint programObject;
GLuint SetupGLSL(char*);

int nsphindices;
int ncylinderindices;
Vertex InitalCubevertex[24];
Vertex circlevertex[25];
Vertex *spherevertex;
Vertex *cylindervertex;
float *MAZE;

GLubyte InitalCubeindice[36];
GLubyte cirseq[25];
GLubyte *sphereindice;
GLubyte *cylinderindice;
GLubyte mazenum[54];

GLuint vboHandleCube[1];   // a VBO that contains interleaved positions and colors 
GLuint indexVBOCube;

GLuint vboHandleCir[1];
GLuint indexVBOCir;

GLuint vboHandleCyl[1];
GLuint indexVBOCyl;

GLuint vboHandleSph[1];
GLuint indexVBOSph;

GLuint vboHandlemaze[1];
GLuint indexVBOmaze;

float angle0 = Pi / 4;
float angle1 = -Pi / 4, angle2 = -Pi / 3;
float angle3 = -Pi / 4, angle4 = -Pi / 2;
float angle5 = -Pi / 2, angle6 = 0;
float angle7 = Pi / 2, angle8 = 0;
float angle9 = 0;
float  eyeangle = 0, noseangle = 0;

glm::mat4 modelM = glm::mat4(1.0f);
glm::mat4 modelM_ = glm::mat4(1.0f);
vector<glm::mat4> mat_list;
stack<glm::mat4> mat_stack;
stack<glm::mat4> mat_stack_;

/////////////////////////////////
// Define Light Properties -  Ia, Id, Is, and light position 
//////////////////////////////////
/*GLfloat light_ambient[4] = { 0.8,0.8,0.8,1 };  //Ia 
GLfloat light_diffuse[4] = { 0.8,0.8,0.8,1 };  //Id
GLfloat light_specular[4] = { 1,1,1,1 };  //Is*/
GLfloat light_ambient[4] = { 1.0,1.0,1.0,1 };  //Ia 
GLfloat light_diffuse[4] = { 1.0,1.0,1.0,1 };  //Id
GLfloat light_specular[4] = { 1.0,1.0,1.0,1 };  //Is
GLfloat light_pos[4] = { 0.5, 0.3, 1.3, 1 };
glm::mat4 lightM = glm::translate(glm::mat4(1.0f), glm::vec3(light_pos[0], light_pos[1], light_pos[2]));
/////////////////////////////////
// Define Default Material Properties -  Ka, Kd, Ks, Shininess 
//////////////////////////////////
GLfloat mat_ambient[4] = { 0.1,0.1,0.1,1 };  //Ka 
GLfloat mat_diffuse[4] = { 0.5,0.5,0.5,1 };  //Kd
GLfloat mat_specular[4] = { 0.5,0.5,0.5,1 };  //Ks
GLfloat mat_shine[1] = { 10 };
GLfloat lightSource[2] = {0,1};


/////////////////////////////////////////////////////////////////////////////////////////////////////
float  getmazevertex() {
	int maze[10][10] =//1 can go through
	{ { 0,0,0,0,0,0,0,0,0,0 },
	{ 1,1,1,0,1,1,1,0,1,0 },
	{ 0,1,1,0,1,1,1,0,1,0 },
	{ 0,1,1,1,1,0,0,1,1,0 },
	{ 0,1,0,0,0,1,1,1,1,0 },
	{ 0,1,1,1,0,1,1,1,0,0 },
	{ 0,1,0,1,1,1,0,1,1,0 },
	{ 0,1,0,0,0,1,0,0,1,0 },
	{ 0,0,1,1,1,1,0,1,1,0 },
	{ 0,0,0,0,0,0,0,1,0,0 }
	};

	int cnt = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (maze[i][j] == 0) {
				cnt++;
			}
		}
	}
	cout << "cnt=" << cnt << endl;
	MAZE = new float[cnt * 3];
	int m = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (maze[i][j] == 0) {
				m += 3;
				MAZE[m - 3] = float(i) / 5 - 0.9;
				MAZE[m - 2] = float(j) / 5 - 0.9;
				MAZE[m - 1] = -1.0;
			}
		}
	}
	return *MAZE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void getcirclevertex(float radius, float color[3]) {
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;

	for (int i = 0; i < 25; i++) {
		x = radius * cos(2 * Pi / 24 * i);
		y = radius * sin(2 * Pi / 24 * i);
		z = 0;
		circlevertex[i].location[0] = x; circlevertex[i].location[1] = y; circlevertex[i].location[2] = z; circlevertex[i].location[3] = 1.0;
		circlevertex[i].normal[0] = x; circlevertex[i].normal[1] = y; circlevertex[i].normal[2] = 0.0; circlevertex[i].normal[3] = 0.0;
		circlevertex[i].color[0] = color[0]; circlevertex[i].color[1] = color[1]; circlevertex[i].color[2] = color[2]; circlevertex[i].color[3] = 1.0;
	}
}

void InitCube(float size, float color[3]) {    
	InitalCubevertex[0].location[0] = size; InitalCubevertex[0].location[1] = -size; InitalCubevertex[0].location[2] = size; InitalCubevertex[0].location[3] = 1.0;
	InitalCubevertex[1].location[0] = size; InitalCubevertex[1].location[1] = -size; InitalCubevertex[1].location[2] = -size; InitalCubevertex[1].location[3] = 1.0;
	InitalCubevertex[2].location[0] = size; InitalCubevertex[2].location[1] = size; InitalCubevertex[2].location[2] = -size; InitalCubevertex[2].location[3] = 1.0;
	InitalCubevertex[3].location[0] = size; InitalCubevertex[3].location[1] = size; InitalCubevertex[3].location[2] = size; InitalCubevertex[3].location[3] = 1.0;
	InitalCubevertex[4].location[0] = -size; InitalCubevertex[4].location[1] = -size; InitalCubevertex[4].location[2] = size; InitalCubevertex[4].location[3] = 1.0;
	InitalCubevertex[5].location[0] = -size; InitalCubevertex[5].location[1] = -size; InitalCubevertex[5].location[2] = -size; InitalCubevertex[5].location[3] = 1.0;
	InitalCubevertex[6].location[0] = -size; InitalCubevertex[6].location[1] = size; InitalCubevertex[6].location[2] = -size; InitalCubevertex[6].location[3] = 1.0;
	InitalCubevertex[7].location[0] = -size; InitalCubevertex[7].location[1] = size; InitalCubevertex[7].location[2] = size; InitalCubevertex[7].location[3] = 1.0;
	
	InitalCubevertex[8].location[0] = -size; InitalCubevertex[8].location[1] = size; InitalCubevertex[8].location[2] = size; InitalCubevertex[8].location[3] = 1.0;
	InitalCubevertex[9].location[0] = -size; InitalCubevertex[9].location[1] = size; InitalCubevertex[9].location[2] = -size; InitalCubevertex[9].location[3] = 1.0;
	InitalCubevertex[10].location[0] = size; InitalCubevertex[10].location[1] = size; InitalCubevertex[10].location[2] = -size; InitalCubevertex[10].location[3] = 1.0;
	InitalCubevertex[11].location[0] = size; InitalCubevertex[11].location[1] = size; InitalCubevertex[11].location[2] = size; InitalCubevertex[11].location[3] = 1.0;
	InitalCubevertex[12].location[0] = -size; InitalCubevertex[12].location[1] = -size; InitalCubevertex[12].location[2] = size; InitalCubevertex[12].location[3] = 1.0;
	InitalCubevertex[13].location[0] = -size; InitalCubevertex[13].location[1] = -size; InitalCubevertex[13].location[2] = -size; InitalCubevertex[13].location[3] = 1.0;
	InitalCubevertex[14].location[0] = size; InitalCubevertex[14].location[1] = -size; InitalCubevertex[14].location[2] = -size; InitalCubevertex[14].location[3] = 1.0;
	InitalCubevertex[15].location[0] = size; InitalCubevertex[15].location[1] = -size; InitalCubevertex[15].location[2] = size; InitalCubevertex[15].location[3] = 1.0;

	InitalCubevertex[16].location[0] = -size; InitalCubevertex[16].location[1] = size; InitalCubevertex[16].location[2] = size; InitalCubevertex[16].location[3] = 1.0;
	InitalCubevertex[17].location[0] = -size; InitalCubevertex[17].location[1] = -size; InitalCubevertex[17].location[2] = size; InitalCubevertex[17].location[3] = 1.0;
	InitalCubevertex[18].location[0] = size; InitalCubevertex[18].location[1] = -size; InitalCubevertex[18].location[2] = size; InitalCubevertex[18].location[3] = 1.0;
	InitalCubevertex[19].location[0] = size; InitalCubevertex[19].location[1] = size; InitalCubevertex[19].location[2] = size; InitalCubevertex[19].location[3] = 1.0;
	InitalCubevertex[20].location[0] = -size; InitalCubevertex[20].location[1] = size; InitalCubevertex[20].location[2] = -size; InitalCubevertex[20].location[3] = 1.0;
	InitalCubevertex[21].location[0] = -size; InitalCubevertex[21].location[1] = -size; InitalCubevertex[21].location[2] = -size; InitalCubevertex[21].location[3] = 1.0;
	InitalCubevertex[22].location[0] = size; InitalCubevertex[22].location[1] = -size; InitalCubevertex[22].location[2] = -size; InitalCubevertex[22].location[3] = 1.0;
	InitalCubevertex[23].location[0] = size; InitalCubevertex[23].location[1] = size; InitalCubevertex[23].location[2] = -size; InitalCubevertex[23].location[3] = 1.0;

	InitalCubevertex[0].normal[0] =1 ; InitalCubevertex[0].normal[1] =0; InitalCubevertex[0].normal[2] =0 ; InitalCubevertex[0].normal[3] = 1.0;
	InitalCubevertex[1].normal[0] =1 ; InitalCubevertex[1].normal[1] =0 ; InitalCubevertex[1].normal[2] =0 ; InitalCubevertex[1].normal[3] = 1.0;
	InitalCubevertex[2].normal[0] =1 ; InitalCubevertex[2].normal[1] =0 ; InitalCubevertex[2].normal[2] =0 ; InitalCubevertex[2].normal[3] = 1.0;
	InitalCubevertex[3].normal[0] =1 ; InitalCubevertex[3].normal[1] =0 ; InitalCubevertex[3].normal[2] =0 ; InitalCubevertex[3].normal[3] = 1.0;
	InitalCubevertex[4].normal[0] =-1 ; InitalCubevertex[4].normal[1] =0 ; InitalCubevertex[4].normal[2] =0 ; InitalCubevertex[4].normal[3] = 1.0;
	InitalCubevertex[5].normal[0] =-1 ; InitalCubevertex[5].normal[1] =0 ; InitalCubevertex[5].normal[2] =0 ; InitalCubevertex[5].normal[3] = 1.0;
	InitalCubevertex[6].normal[0] =-1 ; InitalCubevertex[6].normal[1] =0 ; InitalCubevertex[6].normal[2] =0 ; InitalCubevertex[6].normal[3] = 1.0;
	InitalCubevertex[7].normal[0] =-1 ; InitalCubevertex[7].normal[1] =0 ; InitalCubevertex[7].normal[2] =0 ; InitalCubevertex[7].normal[3] = 1.0;

	InitalCubevertex[8].normal[0] =0 ; InitalCubevertex[8].normal[1] =1 ; InitalCubevertex[8].normal[2] =0 ; InitalCubevertex[8].normal[3] = 1.0;
	InitalCubevertex[9].normal[0] =0 ; InitalCubevertex[9].normal[1] =1 ; InitalCubevertex[9].normal[2] =0 ; InitalCubevertex[9].normal[3] = 1.0;
	InitalCubevertex[10].normal[0] =0 ; InitalCubevertex[10].normal[1] =1 ; InitalCubevertex[10].normal[2] =0 ; InitalCubevertex[10].normal[3] = 1.0;
	InitalCubevertex[11].normal[0] =0 ; InitalCubevertex[11].normal[1] =1 ; InitalCubevertex[11].normal[2] =0 ; InitalCubevertex[11].normal[3] = 1.0;
	InitalCubevertex[12].normal[0] =0 ; InitalCubevertex[12].normal[1] =-1 ; InitalCubevertex[12].normal[2] =0 ; InitalCubevertex[12].normal[3] = 1.0;
	InitalCubevertex[13].normal[0] =0 ; InitalCubevertex[13].normal[1] =-1; InitalCubevertex[13].normal[2] =0 ; InitalCubevertex[13].normal[3] = 1.0;
	InitalCubevertex[14].normal[0] =0 ; InitalCubevertex[14].normal[1] =-1 ; InitalCubevertex[14].normal[2] =0 ; InitalCubevertex[14].normal[3] = 1.0;
	InitalCubevertex[15].normal[0] =0 ; InitalCubevertex[15].normal[1] =-1 ; InitalCubevertex[15].normal[2] =0 ; InitalCubevertex[15].normal[3] = 1.0;

	InitalCubevertex[16].normal[0] =0 ; InitalCubevertex[16].normal[1] =0 ; InitalCubevertex[16].normal[2] =1 ; InitalCubevertex[16].normal[3] = 1.0;
	InitalCubevertex[17].normal[0] =0 ; InitalCubevertex[17].normal[1] =0 ; InitalCubevertex[17].normal[2] =1 ; InitalCubevertex[17].normal[3] = 1.0;
	InitalCubevertex[18].normal[0] =0 ; InitalCubevertex[18].normal[1] =0 ; InitalCubevertex[18].normal[2] =1 ; InitalCubevertex[18].normal[3] = 1.0;
	InitalCubevertex[19].normal[0] =0 ; InitalCubevertex[19].normal[1] =0 ; InitalCubevertex[19].normal[2] =1 ; InitalCubevertex[19].normal[3] = 1.0;
	InitalCubevertex[20].normal[0] =0 ; InitalCubevertex[20].normal[1] =0 ; InitalCubevertex[20].normal[2] =-1 ; InitalCubevertex[20].normal[3] = 1.0;
	InitalCubevertex[21].normal[0] =0 ; InitalCubevertex[21].normal[1] =0 ; InitalCubevertex[21].normal[2] =-1 ; InitalCubevertex[21].normal[3] = 1.0;
	InitalCubevertex[22].normal[0] =0 ; InitalCubevertex[22].normal[1] =0 ; InitalCubevertex[22].normal[2] =-1 ; InitalCubevertex[22].normal[3] = 1.0;
	InitalCubevertex[23].normal[0] =0 ; InitalCubevertex[23].normal[1] =0 ; InitalCubevertex[23].normal[2] =-1 ; InitalCubevertex[23].normal[3] = 1.0;

	for (int i = 0; i < 24; i++) {
		InitalCubevertex[i].color[0] = color[0] ; InitalCubevertex[i].color[1] = color[1]; InitalCubevertex[i].color[2] = color[2];
	}

	InitalCubeindice[0] = 0;   InitalCubeindice[1] = 1;   InitalCubeindice[2] = 2;
	InitalCubeindice[3] = 0;   InitalCubeindice[4] = 2;   InitalCubeindice[5] = 3;
	InitalCubeindice[6] = 4;   InitalCubeindice[7] = 5;   InitalCubeindice[8] = 6;
	InitalCubeindice[9] = 4;   InitalCubeindice[10] = 6;   InitalCubeindice[11] = 7;
	InitalCubeindice[12] = 8;   InitalCubeindice[13] = 9;   InitalCubeindice[14] = 10;
	InitalCubeindice[15] = 8;   InitalCubeindice[16] = 10;   InitalCubeindice[17] = 11;
	InitalCubeindice[18] = 12;   InitalCubeindice[19] = 13;   InitalCubeindice[20] = 14;
	InitalCubeindice[21] = 12;   InitalCubeindice[22] = 14;   InitalCubeindice[23] = 15;
	InitalCubeindice[24] = 16;   InitalCubeindice[25] = 17;   InitalCubeindice[26] = 18;
	InitalCubeindice[27] = 16;   InitalCubeindice[28] = 18;   InitalCubeindice[29] = 19;
	InitalCubeindice[30] = 20;   InitalCubeindice[31] = 21;   InitalCubeindice[32] = 22;
	InitalCubeindice[33] = 20;   InitalCubeindice[34] = 22;   InitalCubeindice[35] = 23;
}

void InitCylinder(float baser, float topr, float height, int numslices, int numstacks, float color[3]) {
	int ncylindervertices = numstacks*numslices;          //number of cylinder vertices
	cylindervertex = new Vertex[ncylindervertices];
	float Dangle = 2 * Pi / (float)(numslices - 1);

	for (int j = 0; j<numstacks; j++) {
		for (int i = 0; i < numslices; i++) {
			int idx = j*numslices + i; // mesh[j][i] 
			float angle = Dangle * i;
			float radius = baser + j* (topr - baser) / (float)(numstacks - 1);
			cylindervertex[idx].location[0] = cylindervertex[idx].normal[0] = cos(angle)*radius;
			cylindervertex[idx].location[1] = cylindervertex[idx].normal[1] = sin(angle)*radius;
			cylindervertex[idx].location[2] = j*1.0*height / (float)(numstacks - 1);
			cylindervertex[idx].location[3] = 1.0;
			cylindervertex[idx].normal[2] = 0.0; cylindervertex[idx].normal[3] = 0.0;
			cylindervertex[idx].color[0] = color[0];
			cylindervertex[idx].color[1] = color[1];
			cylindervertex[idx].color[2] = color[2];
			cylindervertex[idx].color[3] = 1.0;
		}
		//cout << cylindervertex[j*numslices].location[2] << endl;
	}
	// now create the index array 
	ncylinderindices = (numstacks - 1) * 2 * (numslices + 1);
	cylinderindice = new GLubyte[ncylinderindices];
	int n = 0;
	for (int j = 0; j<numstacks - 1; j++) {
		for (int i = 0; i <= numslices; i++) {
			int mi = i % numslices;
			int idx = j*numslices + mi; // mesh[j][mi] 
			int idx2 = (j + 1) * numslices + mi;
			cylinderindice[n++] = idx;
			cylinderindice[n++] = idx2;
		}
	}
}

void InitSphere(float radius, int numslices, int numstacks, float color[3]) {
	int nsphvertices = numstacks*numslices;
	nsphindices = (numstacks - 1) * 2 * (numslices + 1);
	spherevertex = new Vertex[nsphvertices];
	float Dangle = 2 * Pi / (float)(numslices - 1);
	float hangle = Pi / float(numstacks - 1);
	for (int j = 0; j < numstacks; j++) {
		for (int i = 0; i < numslices; i++) {
			int idx = j*numslices + i; // mesh[j][i]
			float angle = Dangle * i;
			float subRadius = radius*sin(hangle*(float(j)));
			float h = radius*cos(hangle*(float(j)));//positive to negative
			spherevertex[idx].location[0] = spherevertex[idx].normal[0] = cos(angle)*subRadius;
			spherevertex[idx].location[1] = spherevertex[idx].normal[1] = sin(angle)*subRadius;
			spherevertex[idx].location[2] = h;//height;
			spherevertex[idx].location[3] = 1.0;
			spherevertex[idx].normal[2] = h; spherevertex[idx].normal[3] = 0.0;
			spherevertex[idx].color[0] = color[0] ;
			spherevertex[idx].color[1] = color[1] ;
			spherevertex[idx].color[2] = color[2] ;
			spherevertex[idx].color[3] = 1.0;
		}
		//cout << spherevertex[j*numslices].location[2] << endl;
	}
	// now create the index array 
	sphereindice = new GLubyte[nsphindices];
	int n = 0;
	for (int j = 0; j<numstacks - 1; j++) {
		for (int i = 0; i <= numslices; i++) {
			int mi = i % numslices;
			int idx = j*numslices + mi; // mesh[j][mi] 
			int idx2 = (j + 1) * numslices + mi;
			sphereindice[n++] = idx;
			sphereindice[n++] = idx2;
		}
	}
}

void InitVBOCircle() {
	float color[3] = { 1, 1, 1 };
	getcirclevertex(1,color );
	glGenBuffers(1, vboHandleCir);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCir[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 25, circlevertex, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up

	for (int i = 0; i < 25; i++) {
		cirseq[i] = i;
	}
	glGenBuffers(1, &indexVBOCir);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCir);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 25, cirseq, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
}

void InitVBOMaze() {
	getmazevertex();
	glGenBuffers(1, vboHandlemaze);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandlemaze[0]);   // bind the first handle
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 162, MAZE, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up

	for (int i = 0; i < 54; i++) {
		mazenum[i] = i;
	}
	glGenBuffers(1, &indexVBOmaze);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOmaze);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 54, mazenum, GL_STATIC_DRAW);  // load the index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
}

void InitVBOCube() {
	float color[3] = { 0.5,0.5,0.5 };
	InitCube(1, color);
	glGenBuffers(1, vboHandleCube);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCube[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 24, InitalCubevertex, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up 

	glGenBuffers(1, &indexVBOCube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 36, InitalCubeindice, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up 
}

void InitVBOCylinder(int ncylslices, int ncylstacks) {
	float color[3] = { 1.0,1.0,1.0 };
	InitCylinder(1, 1, 1, ncylslices, ncylstacks, color);
	int ncylindervertices = ncylstacks*ncylslices;          //number of cylinder vertices
	ncylinderindices = (ncylstacks - 1) * 2 * (ncylslices + 1);

	glGenBuffers(1, vboHandleCyl);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCyl[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * ncylindervertices, cylindervertex, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up

	glGenBuffers(1, &indexVBOCyl);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCyl);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * ncylinderindices, cylinderindice, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
}

void InitVBOSphere(int nsphslices, int nsphstacks) {
	float color[3] = { 1.0, 1.0, 1.0 };
	InitSphere(1, nsphslices, nsphstacks, color);
	int nsphvertices = nsphstacks*nsphslices;
	nsphindices = (nsphstacks - 1) * 2 * (nsphslices + 1);

	glGenBuffers(1, vboHandleSph);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleSph[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nsphvertices, spherevertex, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up

	glGenBuffers(1, &indexVBOSph);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOSph);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * nsphindices, sphereindice, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
}
//////////////////////////////////////////////////////////////////////////////////////////
void draw_maze(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCube[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCube);

	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);  // position 
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32); // color
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16); // normal

	glm::mat4 normal_matrix = glm::inverse(local2eye);
	normal_matrix = glm::transpose(normal_matrix);

	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (char*)NULL + 0);
}

void draw_circle(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCir[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCir);

	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);  // position 
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32); // color
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16); // normal

	glm::mat4 normal_matrix = glm::inverse(local2eye);
	normal_matrix = glm::transpose(normal_matrix);

	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
	glDrawElements(GL_TRIANGLE_FAN, 25, GL_UNSIGNED_BYTE, (char*)NULL + 0);
}

void draw_sphere(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleSph[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOSph);

	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);  // position 
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32); // color
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16); // normal

	glm::mat4 normal_matrix = glm::inverse(local2eye);
	normal_matrix = glm::transpose(normal_matrix);

	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
	glDrawElements(GL_TRIANGLE_STRIP, nsphindices, GL_UNSIGNED_BYTE, (char*)NULL + 0);
}

void draw_cylinder(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCyl[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCyl);

	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);  // position 
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32); // color
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16); // normal

	glm::mat4 normal_matrix = glm::inverse(local2eye);
	normal_matrix = glm::transpose(normal_matrix);

	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix 
	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
	glDrawElements(GL_TRIANGLE_STRIP, ncylinderindices, GL_UNSIGNED_BYTE, (char*)NULL + 0);
}
/////////////////////////////////////////////////////////////
void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (WIRE_FRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glUseProgram(programObject);

	GLuint c0 = glGetAttribLocation(programObject, "position");
	GLuint c1 = glGetAttribLocation(programObject, "color");
	GLuint c2 = glGetAttribLocation(programObject, "normal");
	GLuint m1 = glGetUniformLocation(programObject, "local2clip");
	GLuint m2 = glGetUniformLocation(programObject, "local2eye");
	GLuint m3 = glGetUniformLocation(programObject, "normal_matrix");
	GLuint m4 = glGetUniformLocation(programObject, "world2eye");

	GLuint Ia = glGetUniformLocation(programObject, "light_ambient");
	GLuint Id = glGetUniformLocation(programObject, "light_diffuse");
	GLuint Is = glGetUniformLocation(programObject, "light_specular");
	GLuint Lpos = glGetUniformLocation(programObject, "light_pos");

	GLuint Ka = glGetUniformLocation(programObject, "mat_ambient");
	GLuint Kd = glGetUniformLocation(programObject, "mat_diffuse");
	GLuint Ks = glGetUniformLocation(programObject, "mat_specular");
	GLuint Shine = glGetUniformLocation(programObject, "mat_shine");
	GLuint LightSource= glGetUniformLocation(programObject, "lightSource");

	glUniform4f(Ia, light_ambient[0], light_ambient[1], light_ambient[2], light_ambient[3]);
	glUniform4f(Id, light_diffuse[0], light_diffuse[1], light_diffuse[2], light_diffuse[3]);
	glUniform4f(Is, light_specular[0], light_specular[1], light_specular[2], light_specular[3]);
	glUniform4f(Lpos, light_pos[0], light_pos[1], light_pos[2], light_pos[3]);

	glUniform4f(Ka, mat_ambient[0], mat_ambient[1], mat_ambient[2], mat_ambient[3]);
	glUniform4f(Kd, mat_diffuse[0], mat_diffuse[1], mat_diffuse[2], mat_diffuse[3]);
	glUniform4f(Ks, mat_specular[0], mat_specular[1], mat_specular[2], mat_specular[3]);
	glUniform1f(Shine, mat_shine[0]);


	glEnableVertexAttribArray(c0);
	glEnableVertexAttribArray(c1);
	glEnableVertexAttribArray(c2);

	glm::mat4 projection = glm::perspective(90.0f, 1.0f, .1f, 100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 modelall = glm::mat4(1.0f);
	modelall = glm::rotate(modelall, y_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	modelall = glm::rotate(modelall, x_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	modelall = glm::scale(modelall, glm::vec3(scale_size, scale_size, scale_size));

	glm::mat4 mvp;
	glm::mat4 mv;
	///////////////////light/////////////////////
	glUniform1f(LightSource, lightSource[1]);
	float color[3] = { 1.0, 1.0, 1.0 };
	mat_stack_.push(lightM);
	lightM = glm::scale(lightM, glm::vec3(0.05,0.05, 0.05));
	mvp = projection*view*modelall*lightM;
	mv = view*modelall*lightM;
	SetMaterialColor(Kd, 1, 1, 1, 1);
	draw_sphere(mvp, mv, view, color, c0, c1, c2, m1, m2, m3, m4);
	lightM = mat_stack_.top();
	mat_stack_.pop();

	glUniform1f(LightSource, lightSource[0]);
	/////////////draw maze///////////////////////
	color[0] = 1; color[1] = 0.0; color[2] = 0.8;
	for (int i = 0; i < 160; i += 3) {
		mat_stack_.push(modelM_);
		modelM_ = glm::translate(modelM_, glm::vec3(MAZE[i], MAZE[i + 1], MAZE[i + 2]));
		modelM_ = glm::scale(modelM_, glm::vec3(0.1f, 0.1f, 0.08f));
		mvp = projection*view*modelall*modelM_;
		mv = view*modelall*modelM_;
		SetMaterialColor(Kd, 1.0, 0.0, 0.8, 1);
		draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM_ = mat_stack_.top();
		mat_stack_.pop();
	}

	////////////////candy///////////////////////////
	color[0] = 1; color[1] = 1; color[2] = 0.2;
	mat_stack_.push(modelM_);
	modelM_ = glm::translate(modelM_, glm::vec3(0.9f, 0.5f, -1.0f));
	modelM_ = glm::scale(modelM_, glm::vec3(0.1f, 0.1f, 0.1f));

	mvp = projection*view*modelall*modelM_;
	mv = view*modelall*modelM_;
	SetMaterialColor(Kd, 1.0, 1.0, 0.2, 1);
	draw_sphere(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
	modelM_ = mat_stack_.top();
	mat_stack_.pop();


	///////////////////////draw sun////////////////////////////
	color[0] = 1; color[1] = 0.0; color[2] = 0.0;
	mat_stack_.push(modelM_);
	modelM_ = glm::translate(modelM_, glm::vec3(-0.5f, 1.0f, 1.0f));
	modelM_ = glm::scale(modelM_, glm::vec3(0.3f, 0.3f, 0.3f));

	mvp = projection*view*modelall*modelM_;
	mv = view*modelall*modelM_;
	SetMaterialColor(Kd, 1.0, 0.0, 0.0, 1);
	draw_sphere(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
	modelM_ = mat_stack_.top();
	mat_stack_.pop();
	

	// draw the robot 
	////////////body///////////////////

	color[0] = 0.5; color[1] = 0.5; color[2] = 1.0;
	mat_stack.push(modelM);
	modelM = glm::rotate(modelM, angle0, glm::vec3(0.0f, 0.0f, 1.0f));
	mat_stack.push(modelM);
	modelM = glm::scale(modelM, glm::vec3(3.0f, 3.0f, 2.0f)*b);

	mvp = projection*view*modelall*modelM;
	mv = view*modelall*modelM;
	SetMaterialColor(Kd, 0.5, 0.5, 1.0, 1);
	draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
	modelM = mat_stack.top();
	mat_stack.pop();
	modelM = mat_stack.top();
	mat_stack.pop();
	mat_stack.push(modelM);

	//tooth//
	color[0] = 1; color[1] = 1; color[2] = 1;
	float tooth[] = { 0.25f,-1.6f,2.2f,-0.25f,-1.6f,2.2f };
	for (int i = 0; i < 6; i += 3) {
		modelM = glm::translate(modelM, glm::vec3(tooth[i], tooth[i + 1], tooth[i + 2])*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.2f, 0.25f, 0.2f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 1.0, 1.0, 1.0, 1);
		draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();  mat_stack.pop();
		modelM = mat_stack.top();  mat_stack.pop();
		mat_stack.push(modelM);
	}

	////////////////ARM//////////////////
	float translatearm[] = { 3.8f,1.5f,1.4f,2.0f,2.1f,-3.8f,-1.5f,-1.4f,-2.0f,-2.1f };
	float anglearm[] = { angle1,angle2,angle3,angle4 };
	for (int i = 0, j = 0; i<10, j<4; i += 5, j += 2) {
		///////////lower//////////
		color[0] = 0; color[1] = 0.8; color[2] = 0.8;
		modelM = glm::translate(modelM, glm::vec3(translatearm[i], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, anglearm[j], glm::vec3(0.0f, 0.0f, 1.0f));
		modelM = glm::translate(modelM, glm::vec3(translatearm[i + 1], 0.0f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(1.5f, 0.5f, 0.5f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 0.0, 0.8, 0.8, 1);
		draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();  mat_stack.pop();
		///////upper////////////
		color[0] = 0.6; color[1] = 1; color[2] = 0.2;
		modelM = glm::translate(modelM, glm::vec3(translatearm[i + 2], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, anglearm[j + 1], glm::vec3(0.0f, 0.0f, 1.0f));
		modelM = glm::translate(modelM, glm::vec3(translatearm[i + 3], 0.0f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(2.0f, 0.8f, 1.0f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 0.6, 1.0, 0.2, 1);
		draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();  mat_stack.pop();
		///////hand///////////
		color[0] = 1; color[1] = 0.8; color[2] = 0.8;
		modelM = glm::translate(modelM, glm::vec3(translatearm[i + 4], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, angle9, glm::vec3(0.0f, 0.0f, 1.0f));
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.7f, 1.0f, 1.5f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 1.0, 0.8, 0.8, 1);
		draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();
		mat_stack.pop();
		modelM = mat_stack.top();
		mat_stack.pop();
		mat_stack.push(modelM);
	}

	////////// leg/////////
	float translateleg[] = { 1.9f,2.0f,1.9f,2.0f ,2.0f,  -1.9f,-2.0f,-1.9f,-2.0f ,-2.0f };
	float angleleg[] = { angle5,angle6,angle7,angle8 };
	for (int i = 0, j = 0; i < 10, j<4; i += 5, j += 2) {
		//////////////lower///////////////
		color[0] = 0; color[1] = 0.8; color[2] = 0.8;
		modelM = glm::translate(modelM, glm::vec3(translateleg[i], -2.3f, 0.0f)*b);
		modelM = glm::rotate(modelM, angleleg[j], glm::vec3(0.0f, 0.0f, 1.0f));
		modelM = glm::translate(modelM, glm::vec3(translateleg[i + 1], 0.0f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(2.0f, 0.5f, 0.5f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 0.0, 0.8, 0.8, 1);
		draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();  mat_stack.pop();
		/////////////upper////////////////
		color[0] = 0.6; color[1] = 1; color[2] = 0.2;
		modelM = glm::translate(modelM, glm::vec3(translateleg[i + 2], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, angleleg[j + 1], glm::vec3(0.0f, 0.0f, 1.0f));
		modelM = glm::translate(modelM, glm::vec3(translateleg[i + 3], 0.0f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(2.0f, 0.8f, 1.0f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 0.6, 1.0, 0.2, 1);
		draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();  mat_stack.pop();
		/////////////foot///////////////
		color[0] = 1; color[1] = 0.8; color[2] = 0.8;
		modelM = glm::translate(modelM, glm::vec3(translateleg[i + 4], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, angle9, glm::vec3(0.0f, 0.0f, 1.0f));
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.7f, 1.0f, 1.5f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 1.0, 0.8, 0.8, 1);
		draw_maze(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();
		mat_stack.pop();
		modelM = mat_stack.top();
		mat_stack.pop();
		mat_stack.push(modelM);
	}



	///////////////////draw circle eye,cylinder/////////////////////////////
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCyl[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCyl);
	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32);
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16);
	color[0] = 1; color[1] = 1; color[2] = 1;
	float translateeye[] = { 1.5f, -1.5f };
	for (int i = 0; i <2; i++) {
		modelM = glm::translate(modelM, glm::vec3(translateeye[i], 1.2f, 2.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.5f, 0.5f, 0.5f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 1.0, 1.0, 1.0, 1);
		draw_cylinder(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();
		mat_stack.pop();
		modelM = mat_stack.top();
		mat_stack.pop();
		mat_stack.push(modelM);
	}
	///////////////circle//////////////////////
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCir[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCir);
	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32);
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16);
	color[0] = 1; color[1] = 1; color[2] = 1;
	for (int i = 0; i <2; i++) {
		modelM = glm::translate(modelM, glm::vec3(translateeye[i], 1.2f, 2.5f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.5f, 0.5f, 0.5f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 1.0, 1.0, 1.0, 1);
		draw_circle(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();
		mat_stack.pop();
		modelM = mat_stack.top();
		mat_stack.pop();
		mat_stack.push(modelM);
	}


	//////////////sphere////////////////////////////
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleSph[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOSph);
	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 0);
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32);
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 16);
	color[0] = 0.0; color[1] = 0.0; color[2] = 0.0;
	float translateeyepupil[] = { 1.5f, -1.5f };
	for (int i = 0; i < 2; i++) {
		// eye pupil
		modelM = glm::translate(modelM, glm::vec3(translateeyepupil[i], 1.2f, 2.5f)*b);
		modelM = glm::rotate(modelM, eyeangle, glm::vec3(0.1f, 1.0f, 0.0f));
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.3f, 0.3f, 0.3f)*b);

		mvp = projection*view*modelall*modelM;
		mv = view*modelall*modelM;
		SetMaterialColor(Kd, 0.0, 0.0, 0.0, 1);
		draw_sphere(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
		modelM = mat_stack.top();
		mat_stack.pop();
		modelM = mat_stack.top();
		mat_stack.pop();
		mat_stack.push(modelM);
	}
	//nose
	color[0] = 1; color[1] = 0; color[2] = 0;
	modelM = glm::translate(modelM, glm::vec3(0.0f, 0.0f, 2.0f)*b);
	modelM = glm::rotate(modelM, noseangle, glm::vec3(0.0f, 0.0f, 1.0f));
	mat_stack.push(modelM);
	modelM = glm::scale(modelM, glm::vec3(0.5f, 0.5f, 0.5f)*b);

	mvp = projection*view*modelall*modelM;
	mv = view*modelall*modelM;
	SetMaterialColor(Kd, 1.0, 0.0, 0.0, 1);
	draw_sphere(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
	modelM = mat_stack.top();
	mat_stack.pop();
	modelM = mat_stack.top();
	mat_stack.pop();
	mat_stack.push(modelM);


	glutSwapBuffers();
}


///////////////////////////////////////////////////////////////
void mymouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		press_x = x; press_y = y;
		printf(" click on [%d, %d]!\n", x, y);
		if (button == GLUT_LEFT_BUTTON)
			xform_mode = XFORM_ROTATE;
		else if (button == GLUT_RIGHT_BUTTON)
			xform_mode = XFORM_SCALE;
	}
	else if (state == GLUT_UP) {
		xform_mode = XFORM_NONE;
		printf(" mouse button up! \n");
	}
	glutPostRedisplay();
}
///////////////////////////////////////////////////////////////
void mymotion(int x, int y)
{
	if (xform_mode == XFORM_ROTATE) {
		x_angle += (x - press_x) / 50.0;
		if (x_angle > 180) x_angle -= 360;
		else if (x_angle <-180) x_angle += 360;
		press_x = x;
		//modelM = glm::rotate(modelM, x_angle, glm::vec3(1.0f, 0.0f, 0.0f));

		y_angle += (y - press_y) / 50.0;
		if (y_angle > 180) y_angle -= 360;
		else if (y_angle <-180) y_angle += 360;
		press_y = y;
		//modelM = glm::rotate(modelM, y_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (xform_mode == XFORM_SCALE) {
		float old_size = scale_size;
		scale_size *= (1 + (y - press_y) / 600.0);
		if (scale_size <0) scale_size = old_size;
		press_y = y;
		//modelM = glm::scale(modelM, glm::vec3(1.0f, 1.0f, 1.0f)*scale_size);
	}
	glutPostRedisplay();
}
void move(void)
{
	angle9 += Pi / 5400;
	angle1 += (Pi / 3600);
	if (angle1 >= 3 * Pi / 5 || angle1 <= -3 * Pi / 5) {
		angle1 = -3 * Pi / 5;
	}
	angle3 += (Pi / 3600);
	if (angle3 >= 3 * Pi / 5 || angle3 <= -3 * Pi / 5) {
		angle3 = -3 * Pi / 5;
	}
	angle2 += (Pi / 3600);
	if (angle2 >= 3 * Pi / 5 || angle2 <= -3 * Pi / 5) {
		angle2 = -3 * Pi / 5;
	}
	angle4 += (Pi / 3600);
	if (angle4 >= 3 * Pi / 5 || angle4 <= -3 * Pi / 5) {
		angle4 = -3 * Pi / 5;
	}
	angle5 += -(Pi / 7200);
	if (angle5 >= -Pi / 4 || angle5 <= -3 * Pi / 4) {
		angle5 = -Pi / 4;
	}
	angle6 += -(Pi / 7200);
	if (angle6 >= Pi / 4 || angle6 <= -Pi / 4) {
		angle6 = Pi / 4;
	}
	angle7 += (Pi / 7200);
	if (angle7 <= Pi / 4 || angle7 >= 3 * Pi / 4) {
		angle7 = Pi / 4;
	}
	angle8 += (Pi / 7200);
	if (angle8 >= Pi / 4 || angle8 <= -Pi / 4) {
		angle8 = -Pi / 4;
	}
	glutPostRedisplay();
}
//////////////////////////////////////////////////////////////
void mykey(unsigned char key, int x, int y)
{
	float d_angle = 0;
	if (key == 'w') {
		WIRE_FRAME = !WIRE_FRAME;
	}
	if (key == 'q') exit(1);
	if (key == 't') {
		d_angle += Pi / 180 * 15;
		modelM = glm::rotate(modelM, d_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (key == 'T') {
		d_angle += -Pi / 180 * 15;
		modelM = glm::rotate(modelM, d_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (key == 'x') {
		d_angle += -Pi / 180 * 15;
		modelM = glm::rotate(modelM, d_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (key == 'y') {
		d_angle += -Pi / 180 * 15;
		modelM = glm::rotate(modelM, d_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (key == 'r')
		modelM = glm::translate(modelM, glm::vec3(0.1f, 0.0f, 0.0f)*b);
	if (key == 'l')
		modelM = glm::translate(modelM, glm::vec3(-0.1f, 0.0f, 0.0f)*b);
	if (key == 'f')
		modelM = glm::translate(modelM, glm::vec3(0.0f, 0.1f, 0.0f)*b);
	if (key == 'b')
		modelM = glm::translate(modelM, glm::vec3(0.0f, -0.1f, 0.0f)*b);
	if (key == 'u')
		modelM = glm::translate(modelM, glm::vec3(0.0f, 0.0f, 0.1f)*b);
	if (key == 'd')
		modelM = glm::translate(modelM, glm::vec3(0.0f, 0.0f, -0.1f)*b);

	if (key == 'R') {
		lightM = glm::translate(lightM, glm::vec3(0.5f, 0.0f, 0.0f));
		light_pos[0] += 0.5;
	}
	if (key == 'L') {
		lightM = glm::translate(lightM, glm::vec3(-0.5f, 0.0f, 0.0f));
		light_pos[0] -= 0.5;
	}
	if (key == 'F') {
		lightM = glm::translate(lightM, glm::vec3(0.0f, 0.5f, 0.0f));
		light_pos[1] += 0.5;
	}
	if (key == 'B') {
		lightM = glm::translate(lightM, glm::vec3(0.0f, -0.5f, 0.0f));
		light_pos[1] -= 0.5;
	}
	if (key == 'U') {
		lightM = glm::translate(lightM, glm::vec3(0.0f, 0.0f, 0.5f));
		light_pos[2] += 0.5;
	}
	if (key == 'D') {
		lightM = glm::translate(lightM, glm::vec3(0.0f, 0.0f, -0.5f));
		light_pos[2] -= 0.5;
	}

	if (key == 'o') {
		mat_shine[0] -= 1;
		if (mat_shine[0] < 1) mat_shine[0] = 1;
	}
	if (key == 'p')
		mat_shine[0] += 1;

	if (key == 'O') {
		if (light_diffuse[0] < 1.5) {
			light_diffuse[0] += 0.05;
			light_diffuse[1] += 0.05;
			light_diffuse[2] += 0.05;
			light_diffuse[3] += 0.05;
			light_specular[0] += 0.0625;
			light_specular[1] += 0.0625;
			light_specular[2] += 0.0625;
			light_specular[3] += 0.0625;
		}
	}
	if (key == 'P') {
		if (light_diffuse[0] >0) {
			light_diffuse[0] -= 0.05;
			light_diffuse[1] -= 0.05;
			light_diffuse[2] -= 0.05;
			light_diffuse[3] -= 0.05;
			light_specular[0] -= 0.0625;
			light_specular[1] -= 0.0625;
			light_specular[2] -= 0.0625;
			light_specular[3] -= 0.0625;
		}
	}

	if (key == 'm')
		glutIdleFunc(move);
	if (key == 's')
		glutIdleFunc(NULL);
	if (key == '+')
		b += 0.01;
	if (key == '-')
		b -= 0.01;

	if (key == 'c') {
		glutIdleFunc(NULL);
		modelM = glm::mat4(1.0f);
		modelM = glm::translate(modelM, glm::vec3(-0.71, -0.8, -1.0));
		angle1 = -Pi / 2, angle2 = 0;
		angle3 = Pi / 2, angle4 = 0;
		angle5 = -Pi / 2, angle6 = 0;
		angle7 = Pi / 2, angle8 = 0;
		angle9 = 0;
		b = 0.02;
		x_angle = 0; y_angle = 0;
		float a = 90;
		modelM = glm::rotate(modelM, a, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if (key == '1') {
		angle1 += (Pi / 180 * 15);
		if (angle1 >= 3 * Pi / 5 || angle1 <= -3 * Pi / 5) {
			printf("I am hurting! Back to comfortable position! ");
			angle1 = -3 * Pi / 5;
		}
	}
	if (key == '2') {
		angle2 += (Pi / 180 * 15);
		if (angle2 >= 3 * Pi / 4 || angle2 <= -3 * Pi / 4) {
			printf("I am hurting! Back to comfortable position! ");
			angle2 = -3 * Pi / 4;
		}
	}
	if (key == '3') {
		angle3 += (Pi / 180 * 15);
		if (angle3 >= 3 * Pi / 5 || angle3 <= -3 * Pi / 5) {
			printf("I am hurting! Back to comfortable position! ");
			angle3 = -3 * Pi / 5;
		}
	}
	if (key == '4') {
		angle4 += (Pi / 180 * 15);
		if (angle4 >= 3 * Pi / 4 || angle4 <= -3 * Pi / 4) {
			printf("I am hurting! Back to comfortable position! ");
			angle4 = -3 * Pi / 4;
		}
	}
	if (key == '5') {
		angle5 += (Pi / 180 * 15);
		if (angle5 >= -Pi / 4 || angle5 <= -3 * Pi / 5) {
			printf("I am hurting! Back to comfortable position! ");
			angle5 = -3 * Pi / 5;
		}
	}
	if (key == '6') {
		angle6 += (Pi / 180 * 15);
		if (angle6 >= Pi / 2 || angle6 <= -Pi / 3) {
			printf("I am hurting! Back to comfortable position! ");
			angle6 = -Pi / 3;
		}
	}
	if (key == '7') {
		angle7 += -(Pi / 180 * 15);
		if (angle7 <= Pi / 4 || angle7 >= 3 * Pi / 5) {
			printf("I am hurting! Back to comfortable position! ");
			angle7 = 3 * Pi / 5;
		}
	}
	if (key == '8') {
		angle8 += -(Pi / 180 * 15);
		if (angle8 >= Pi / 2 || angle8 <= -Pi / 3) {
			printf("I am hurting! Back to comfortable position! ");
			angle8 = Pi / 3;
		}
	}
	if (key == '9')
		angle9 += (Pi / 180 * 30);

	glutPostRedisplay();
}
int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);

	glutCreateWindow("Xuerong Hu's lovely robot");
	glutDisplayFunc(display);
	glutMouseFunc(mymouse);
	glutMotionFunc(mymotion);
	glutKeyboardFunc(mykey);

	mat_list.clear();
	// initialize GLEW 
#ifndef __APPLE__
	GLenum err = glewInit();
	if (err != GLEW_OK)  printf(" Error initializing GLEW! \n");
	else printf("Initializing GLEW succeeded!\n");
#endif
#ifdef __APPLE__
#else
	glewInit();
#endif

	int ncylstacks = 10;
	int ncylslices = 20;
	int nsphstacks = 10;
	int nsphslices = 20;
	////////////////////////////////////////////////////////
	InitVBOCircle();
	InitVBOMaze();
	InitVBOCube();
	InitVBOCylinder(ncylslices, ncylstacks);
	InitVBOSphere(nsphslices, nsphstacks);
	///////////////////////////////////////////////////////////
	programObject = SetupGLSL("lab4");
	glutMainLoop();
	delete[] MAZE;
}
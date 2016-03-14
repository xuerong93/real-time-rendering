
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack> 
#include <math.h> 
#include <iostream>
#include <math.h>
#include <ply.h>
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

GLuint programObject;
GLuint SetupGLSL(char*);
int use_texture;

//////////glut mouse control/////////
int press_x, press_y;
int release_x, release_y;
int xform_mode = 0;
float x_angle = 0.0;
float y_angle = 0.0;
float scale_size = 1;
#define XFORM_NONE    0 
#define XFORM_ROTATE  1
#define XFORM_SCALE 2 
bool WIRE_FRAME = false;
float b= 1.0f;   //total scaling
GLuint cubemap_texture;
bool use_gradient = false;
typedef struct{	
	float location[4];
	float normal[4];
	float color[4];
	float texCoord[2];
} myVertex;

/////////////////PLY////////////////////
typedef struct {
	float x, y, z;
	float nx, ny, nz;
}Vertex;
typedef struct {
	unsigned int count;
	unsigned int *vertices;
	float nx, ny, nz;
}Face;
char * string_list[] = {
	"x","y","z","nx","ny","nz","vertex_indices"
};
Vertex** ply_vertices = 0;
Face** ply_faces = 0;
unsigned int ply_vertexcount;
unsigned int ply_facecount;
int ply_vertexnormals = 0;
int ply_facenormals = 0;
myVertex *ply_verts;
GLuint *ply_indices;
int ply_nindices;
float ply_min[3], ply_max[3];

int nsphindices;
int ncylinderindices;
myVertex Squarevertex[4];
myVertex Cubevertex[24];
myVertex circlevertex[25];
myVertex *spherevertex;
myVertex *cylindervertex;
float *MAZE;

GLuint Squareindice[6];
int sq_nindices = 6;
GLubyte Cubeindice[36];
GLubyte cirseq[25];
GLubyte *sphereindice;
GLubyte *cylinderindice;
GLubyte mazenum[54];

GLuint vboHandleCube[1]; GLuint indexVBOCube;
GLuint vboHandleCir[1];GLuint indexVBOCir;
GLuint vboHandleCyl[1];GLuint indexVBOCyl;
GLuint vboHandleSph[1];GLuint indexVBOSph;
GLuint vboHandleMaze[1];GLuint indexVBOMaze;
GLuint vboHandlePly[1]; GLuint indexVBOPly;
GLuint vboHandleSquare[1]; GLuint indexVBOSquare;


float angle0 = Pi / 4;
float angle1 = -Pi / 4, angle2 = -Pi / 3;
float angle3 = -Pi / 4, angle4 = -Pi / 2;
float angle5 = -Pi / 2, angle6 = 0;
float angle7 = Pi / 2, angle8 = 0;
float angle9 = 0;
float  eyeangle = 0, noseangle = 0;
float angx = 0.0; float angy = 0.0; float angz = 0.0;
glm::mat4 modelM = glm::mat4(1.0f);
glm::mat4 modelM_ = glm::mat4(1.0f);
glm::mat4 plyM= glm::mat4(1.0f);
vector<glm::mat4> mat_list;
stack<glm::mat4> mat_stack;
stack<glm::mat4> mat_stack_;

/////////////////////////////////
// Define Light Properties -  Ia, Id, Is, and light position 
//////////////////////////////////
GLfloat light_ambient[4] = { 0.8,0.8,0.8,1 };  //Ia 
GLfloat light_diffuse[4] = { 0.8,0.8,0.8,1 };  //Id
GLfloat light_specular[4] = { 0.8,0.8,0.8,1 };  //Is
GLfloat light_pos[4] = { -0.3,-1.8, 1.3, 1 };
glm::mat4 lightM = glm::translate(glm::mat4(1.0f), glm::vec3(light_pos[0], light_pos[1], light_pos[2]));
/////////////////////////////////
// Define Default Material Properties -  Ka, Kd, Ks, Shininess 
//////////////////////////////////
GLfloat mat_ambient[4] = { 0.5,0.5,0.5,1 };  //Ka 
GLfloat mat_diffuse[4] = { 0.5,0.5,0.5,1 };  //Kd
GLfloat mat_specular[4] = { 1,1,1,1 };  //Ks
GLfloat mat_shine[1] = { 10 };
GLfloat lightSource[2] = {0,1};
/////////////////////////////////////////store ply/////////////////////////////////////////////////
void store_ply(PlyFile* input, Vertex ***vertices, Face ***faces,unsigned int* vertexcount, unsigned int* facecount,int* vertexnormals, int* facenormals) {
	int i, j;

	// go through the element types
	for (i = 0; i < input->num_elem_types; i = i + 1) {
		int count;

		// setup the element for reading and get the element count
		char* element = setup_element_read_ply(input, i, &count);

		// get vertices
		if (strcmp("vertex", element) == 0) {
			*vertices = (Vertex**)malloc(sizeof(Vertex) * count);
			*vertexcount = count;

			// run through the properties and store them
			for (j = 0; j < input->elems[i]->nprops; j = j + 1) {
				PlyProperty* property = input->elems[i]->props[j];
				PlyProperty setup;

				if (strcmp("x", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[0];
					setup.internal_type = Float32;
					setup.offset = offsetof(Vertex, x);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
				}
				else if (strcmp("y", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[1];
					setup.internal_type = Float32;
					setup.offset = offsetof(Vertex, y);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
				}
				else if (strcmp("z", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[2];
					setup.internal_type = Float32;
					setup.offset = offsetof(Vertex, z);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
				}
				else if (strcmp("nx", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[3];
					setup.internal_type = Float32;
					setup.offset = offsetof(Vertex, nx);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
					*vertexnormals = 1;
				}
				else if (strcmp("ny", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[4];
					setup.internal_type = Float32;
					setup.offset = offsetof(Vertex, ny);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
					*vertexnormals = 1;
				}
				else if (strcmp("nz", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[5];
					setup.internal_type = Float32;
					setup.offset = offsetof(Vertex, nz);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
					*vertexnormals = 1;
				}
				// dunno what it is
				else {
					fprintf(stderr, "unknown property type found in %s: %s\n",
						element, property->name);
				}
			}

			// do this if you want to grab the other data
			// list_pointer = get_other_properties_ply
			//                (input, offsetof(Vertex, struct_pointer));

			// copy the data
			for (j = 0; j < count; j = j + 1) {
				(*vertices)[j] = (Vertex*)malloc(sizeof(Vertex));

				get_element_ply(input, (void*)((*vertices)[j]));
			}
		}
		// get faces
		else if (strcmp("face", element) == 0) {
			*faces = (Face**)malloc(sizeof(Face) * count);
			*facecount = count;

			// run through the properties and store them
			for (j = 0; j < input->elems[i]->nprops; j = j + 1) {
				PlyProperty* property = input->elems[i]->props[j];
				PlyProperty setup;

				if (strcmp("vertex_indices", property->name) == 0 &&
					property->is_list == PLY_LIST) {

					setup.name = string_list[6];
					setup.internal_type = Uint32;
					setup.offset = offsetof(Face, vertices);
					setup.count_internal = Uint32;
					setup.count_offset = offsetof(Face, count);

					setup_property_ply(input, &setup);
				}
				else if (strcmp("nx", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[3];
					setup.internal_type = Float32;
					setup.offset = offsetof(Face, nx);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
					*facenormals = 1;
				}
				else if (strcmp("ny", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[4];
					setup.internal_type = Float32;
					setup.offset = offsetof(Face, ny);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
					*facenormals = 1;
				}
				else if (strcmp("nz", property->name) == 0 &&
					property->is_list == PLY_SCALAR) {

					setup.name = string_list[5];
					setup.internal_type = Float32;
					setup.offset = offsetof(Face, nz);
					setup.count_internal = 0;
					setup.count_offset = 0;

					setup_property_ply(input, &setup);
					*facenormals = 1;
				}
				// dunno what it is
				else {
					fprintf(stderr, "unknown property type found in %s: %s\n",
						element, property->name);
				}
			}

			// do this if you want to grab the other data
			// list_pointer = get_other_properties_ply
			//                (input, offsetof(Face, struct_pointer));

			// copy the data
			for (j = 0; j < count; j = j + 1) {
				(*faces)[j] = (Face*)malloc(sizeof(Face));

				get_element_ply(input, (void*)((*faces)[j]));
			}
		}
		// who knows?
		else {
			fprintf(stderr, "unknown element type found: %s\n", element);
		}
	}

	// if you want to grab the other data do this
	// get_other_element_ply(input);
}
////////////////////////////read images////////////////////////////////////////////////////////////
GLubyte checkImage[256][256][4];
GLubyte readImage[400][400][4];
GLubyte texImage[256][256][4];
GLubyte texImage1[256][256][4];
GLubyte texImage2[256][256][4];
GLubyte texImage3[256][256][4];
GLubyte texImage4[256][256][4];
GLubyte texImage5[256][256][4];
GLubyte texImage6[256][256][4];
//GLubyte gradientImage[256][256][4];

char fname1[] = "ac_rt.ppm";//  "bromene-bay_rt.ppm";// "qiao2.ppm";//////positive x
char fname2[] = "ac_lf.ppm";//  "bromene-bay_lf.ppm";// "qiao4.ppm";////negative x
char fname3[] = "ac_up.ppm";//  "bromene-bay_up.ppm";// "sky.ppm";//////positive y
char fname4[] = "ac_dn.ppm";//  "bromene-bay_dn.ppm";// "ground.ppm";///negative y
char fname5[] = "ac_ft.ppm";//  "bromene-bay_ft.ppm";// "qiao1.ppm";/////////positive z
char fname6[] = "ac_bk.ppm";//  "bromene-bay_bk.ppm";// "qiao3.ppm";//////negative z
void read_Image(char* fname){
	FILE* file = fopen(fname, "r");

	int height, width, ccv;
	char header[100];
	fscanf(file, "%s %d %d %d", header, &width, &height, &ccv);

	printf("%s %d %d %d\n", header, width, height, ccv);
	int r, g, b;

	for (int i = height - 1; i >= 0; i--)
		for (int j = 0; j<width; j++)
		{
			fscanf(file, "%d %d %d", &r, &g, &b);
			readImage[i][j][0] = (GLubyte)r;
			readImage[i][j][1] = (GLubyte)g;
			readImage[i][j][2] = (GLubyte)b;
			readImage[i][j][3] = 255;
		}

	for (int i = 0; i<256; i++)
		for (int j = 0; j<256; j++) {
			if (i<height && j <width) {
				texImage[i][j][0] = readImage[i][j][0]; texImage[i][j][1] = readImage[i][j][1];
				texImage[i][j][2] = readImage[i][j][2];	texImage[i][j][3] = 255;
			}
			else {
				texImage[i][j][0] = 0; 	texImage[i][j][1] = 0; 	texImage[i][j][2] = 0;
				texImage[i][j][3] = 255;
			}
		}

	////////////////////////////////////
	//// Use central difference to calculate the gradients 
	//for (int i = 0; i<256; i++)
	//	for (int j = 0; j<256; j++) {
	//		gradientImage[i][j][0] = (unsigned char)((texImage[(i + 1) % 256][j][0] -
	//			texImage[(i - 1) % 256][j][0]) / 2.0 + 128);
	//		gradientImage[i][j][1] = (unsigned char)((texImage[i][(j + 1) % 256][0] -
	//			texImage[i][(j - 1) % 256][0]) / 2.0 + 128);
	//		gradientImage[i][j][2] = 20;
	//		gradientImage[i][j][3] = 255;
	//	}
	fclose(file);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//float  getmazevertex() {
//	int maze[10][10] =//1 can go through
//	{ { 0,0,0,0,0,0,0,0,0,0 },
//	{ 1,1,1,0,1,1,1,0,1,0 },
//	{ 0,1,1,0,1,1,1,0,1,0 },
//	{ 0,1,1,1,1,0,0,1,1,0 },
//	{ 0,1,0,0,0,1,1,1,1,0 },
//	{ 0,1,1,1,0,1,1,1,0,0 },
//	{ 0,1,0,1,1,1,0,1,1,0 },
//	{ 0,1,0,0,0,1,0,0,1,0 },
//	{ 0,0,1,1,1,1,0,1,1,0 },
//	{ 0,0,0,0,0,0,0,1,0,0 }
//	};
//
//	int cnt = 0;
//	for (int i = 0; i < 10; i++) {
//		for (int j = 0; j < 10; j++) {
//			if (maze[i][j] == 0) {
//				cnt++;
//			}
//		}
//	}
//	cout << "cnt=" << cnt << endl;
//	MAZE = new float[cnt * 3];
//	int m = 0;
//	for (int i = 0; i < 10; i++) {
//		for (int j = 0; j < 10; j++) {
//			if (maze[i][j] == 0) {
//				m += 3;
//				MAZE[m - 3] = float(i) / 5 - 0.9;
//				MAZE[m - 2] = float(j) / 5 - 0.9;
//				MAZE[m - 1] = -0.2;
//			}
//		}
//	}
//	return *MAZE;
//}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//void getcirclevertex(float radius, float color[3]) {
//	float x = 0.0;
//	float y = 0.0;
//	float z = 0.0;
//
//	for (int i = 0; i < 25; i++) {
//		x = radius * cos(2 * Pi / 24 * i);
//		y = radius * sin(2 * Pi / 24 * i);
//		z = 0;
//		circlevertex[i].location[0] = x; circlevertex[i].location[1] = y; circlevertex[i].location[2] = z; circlevertex[i].location[3] = 1.0;
//		circlevertex[i].normal[0] = x; circlevertex[i].normal[1] = y; circlevertex[i].normal[2] = 0.0; circlevertex[i].normal[3] = 0.0;
//		circlevertex[i].color[0] = color[0]; circlevertex[i].color[1] = color[1]; circlevertex[i].color[2] = color[2]; circlevertex[i].color[3] = 1.0;
//	}
//}
//
/////////////////////////////////////////////////////////////////////
//void makeCheckImage()
//{
//	for (int i = 0; i<256; i++)
//		for (int j = 0; j<256; j++) {
//			int c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
//			texImage[i][j][0] = (GLubyte)c;
//			texImage[i][j][1] = (GLubyte)c;
//			texImage[i][j][2] = (GLubyte)c;
//			texImage[i][j][3] = (GLubyte)255;
//		};
//}
////////////////////////////////////initialnize texture////////////////////////////////////////////////////////////
void Init_texture(char* fname1, char* fname2, char* fname3,char* fname4, char* fname5, char* fname6){//int tex) {
	GLuint renderTex;
	//GLuint gradientTex;

	//if (tex == 1)
	//	makeCheckImage();
	//else 
	read_Image(fname1);
	glGenTextures(1, &renderTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	read_Image(fname2);
	glGenTextures(1, &renderTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	read_Image(fname3);
	glGenTextures(1, &renderTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	read_Image(fname4);
	glGenTextures(1, &renderTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	read_Image(fname5);
	glGenTextures(1, &renderTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	read_Image(fname6);
	glGenTextures(1, &renderTex);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

	//glGenTextures(1, &gradientTex);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, gradientTex);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, gradientImage);
	////  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	////  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
////////////////////////////////////initialize cubemap////////////////////////////////////
void Init_cubemap() {
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &cubemap_texture);
	printf("cubemap %d\n", cubemap_texture);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP,cubemap_texture);
	

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage2);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage1);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage5);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage6);
	//for (int i = 0; i<6; i++) {
	//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,GL_RGBA, 256,256, 0, GL_RGBA, GL_UNSIGNED_BYTE,texImage); 
	//}
}
void InitSquare(float size,float color[3]) {
	Squarevertex[0].location[0] = -size; Squarevertex[0].location[1] = -size; Squarevertex[0].location[2] = 0; Squarevertex[0].location[3] = 1.0;
	Squarevertex[1].location[0] = size; Squarevertex[1].location[1] = -size; Squarevertex[1].location[2] = 0; Squarevertex[1].location[3] = 1.0;
	Squarevertex[2].location[0] = size; Squarevertex[2].location[1] = size; Squarevertex[2].location[2] = 0; Squarevertex[2].location[3] = 1.0;
	Squarevertex[3].location[0] = -size; Squarevertex[3].location[1] = size; Squarevertex[3].location[2] = 0; Squarevertex[3].location[3] = 1.0;

	Squarevertex[0].normal[0] = 0; Squarevertex[0].normal[1] = 0; Squarevertex[0].normal[2] = 1; Squarevertex[0].normal[3] = 0;
	Squarevertex[1].normal[0] = 0; Squarevertex[1].normal[1] = 0; Squarevertex[1].normal[2] = 1; Squarevertex[1].normal[3] = 0;
	Squarevertex[2].normal[0] = 0; Squarevertex[2].normal[1] = 0; Squarevertex[2].normal[2] = 1; Squarevertex[2].normal[3] = 0;
	Squarevertex[3].normal[0] = 0; Squarevertex[3].normal[1] = 0; Squarevertex[3].normal[2] = 1; Squarevertex[3].normal[3] = 0;

	for (int i = 0; i < 4; i++) {
		Squarevertex[i].color[0] = color[0]; Squarevertex[i].color[1] = color[1]; Squarevertex[i].color[2] = color[2];
	}

	Squarevertex[0].texCoord[0] = 0; Squarevertex[0].texCoord[1] = 0; 	Squarevertex[1].texCoord[0] = 1; Squarevertex[1].texCoord[1] = 0;
	Squarevertex[2].texCoord[0] = 1; Squarevertex[2].texCoord[1] = 1;	Squarevertex[3].texCoord[0] = 0; Squarevertex[3].texCoord[1] = 1;


	Squareindice[0] = 0;   Squareindice[1] = 1;   Squareindice[2] = 2;			Squareindice[3] = 2;   Squareindice[4] = 3;   Squareindice[5] = 0;
}
void InitCube(float size, float color[3]) {    
	Cubevertex[0].location[0] = size; Cubevertex[0].location[1] = -size; Cubevertex[0].location[2] = size; Cubevertex[0].location[3] = 1.0;
	Cubevertex[1].location[0] = size; Cubevertex[1].location[1] = -size; Cubevertex[1].location[2] = -size; Cubevertex[1].location[3] = 1.0;
	Cubevertex[2].location[0] = size; Cubevertex[2].location[1] = size; Cubevertex[2].location[2] = -size; Cubevertex[2].location[3] = 1.0;
	Cubevertex[3].location[0] = size; Cubevertex[3].location[1] = size; Cubevertex[3].location[2] = size; Cubevertex[3].location[3] = 1.0;//////positive x
	Cubevertex[4].location[0] = -size; Cubevertex[4].location[1] = -size; Cubevertex[4].location[2] = size; Cubevertex[4].location[3] = 1.0;
	Cubevertex[5].location[0] = -size; Cubevertex[5].location[1] = -size; Cubevertex[5].location[2] = -size; Cubevertex[5].location[3] = 1.0;
	Cubevertex[6].location[0] = -size; Cubevertex[6].location[1] = size; Cubevertex[6].location[2] = -size; Cubevertex[6].location[3] = 1.0;
	Cubevertex[7].location[0] = -size; Cubevertex[7].location[1] = size; Cubevertex[7].location[2] = size; Cubevertex[7].location[3] = 1.0;/////////////negative x
	
	Cubevertex[8].location[0] = -size; Cubevertex[8].location[1] = size; Cubevertex[8].location[2] = size; Cubevertex[8].location[3] = 1.0;
	Cubevertex[9].location[0] = -size; Cubevertex[9].location[1] = size; Cubevertex[9].location[2] = -size; Cubevertex[9].location[3] = 1.0;
	Cubevertex[10].location[0] = size; Cubevertex[10].location[1] = size; Cubevertex[10].location[2] = -size; Cubevertex[10].location[3] = 1.0;
	Cubevertex[11].location[0] = size; Cubevertex[11].location[1] = size; Cubevertex[11].location[2] = size; Cubevertex[11].location[3] = 1.0;//////////////////////positive y
	Cubevertex[12].location[0] = -size; Cubevertex[12].location[1] = -size; Cubevertex[12].location[2] = size; Cubevertex[12].location[3] = 1.0;
	Cubevertex[13].location[0] = -size; Cubevertex[13].location[1] = -size; Cubevertex[13].location[2] = -size; Cubevertex[13].location[3] = 1.0;
	Cubevertex[14].location[0] = size; Cubevertex[14].location[1] = -size; Cubevertex[14].location[2] = -size; Cubevertex[14].location[3] = 1.0;
	Cubevertex[15].location[0] = size; Cubevertex[15].location[1] = -size; Cubevertex[15].location[2] = size; Cubevertex[15].location[3] = 1.0;///////////////////////////////negative y

	Cubevertex[16].location[0] = -size; Cubevertex[16].location[1] = size; Cubevertex[16].location[2] = size; Cubevertex[16].location[3] = 1.0;
	Cubevertex[17].location[0] = -size; Cubevertex[17].location[1] = -size; Cubevertex[17].location[2] = size; Cubevertex[17].location[3] = 1.0;
	Cubevertex[18].location[0] = size; Cubevertex[18].location[1] = -size; Cubevertex[18].location[2] = size; Cubevertex[18].location[3] = 1.0;
	Cubevertex[19].location[0] = size; Cubevertex[19].location[1] = size; Cubevertex[19].location[2] = size; Cubevertex[19].location[3] = 1.0;////////////////positive z
	Cubevertex[20].location[0] = -size; Cubevertex[20].location[1] = size; Cubevertex[20].location[2] = -size; Cubevertex[20].location[3] = 1.0;
	Cubevertex[21].location[0] = -size; Cubevertex[21].location[1] = -size; Cubevertex[21].location[2] = -size; Cubevertex[21].location[3] = 1.0;
	Cubevertex[22].location[0] = size; Cubevertex[22].location[1] = -size; Cubevertex[22].location[2] = -size; Cubevertex[22].location[3] = 1.0;
	Cubevertex[23].location[0] = size; Cubevertex[23].location[1] = size; Cubevertex[23].location[2] = -size; Cubevertex[23].location[3] = 1.0;////////negative z

	Cubevertex[0].normal[0] =1 ; Cubevertex[0].normal[1] =0; Cubevertex[0].normal[2] =0 ; Cubevertex[0].normal[3] = 0.0;
	Cubevertex[1].normal[0] =1 ; Cubevertex[1].normal[1] =0 ; Cubevertex[1].normal[2] =0 ; Cubevertex[1].normal[3] = 0.0;
	Cubevertex[2].normal[0] =1 ; Cubevertex[2].normal[1] =0 ; Cubevertex[2].normal[2] =0 ; Cubevertex[2].normal[3] = 0.0;
	Cubevertex[3].normal[0] =1 ; Cubevertex[3].normal[1] =0 ; Cubevertex[3].normal[2] =0 ; Cubevertex[3].normal[3] = 0.0;
	Cubevertex[4].normal[0] =-1 ; Cubevertex[4].normal[1] =0 ; Cubevertex[4].normal[2] =0 ; Cubevertex[4].normal[3] = 0.0;
	Cubevertex[5].normal[0] =-1 ; Cubevertex[5].normal[1] =0 ; Cubevertex[5].normal[2] =0 ; Cubevertex[5].normal[3] = 0.0;
	Cubevertex[6].normal[0] =-1 ; Cubevertex[6].normal[1] =0 ; Cubevertex[6].normal[2] =0 ; Cubevertex[6].normal[3] = 0.0;
	Cubevertex[7].normal[0] =-1 ; Cubevertex[7].normal[1] =0 ; Cubevertex[7].normal[2] =0 ; Cubevertex[7].normal[3] = 0.0;

	Cubevertex[8].normal[0] =0 ; Cubevertex[8].normal[1] =1 ; Cubevertex[8].normal[2] =0 ; Cubevertex[8].normal[3] = 0.0;
	Cubevertex[9].normal[0] =0 ; Cubevertex[9].normal[1] =1 ; Cubevertex[9].normal[2] =0 ; Cubevertex[9].normal[3] = 0.0;
	Cubevertex[10].normal[0] =0 ; Cubevertex[10].normal[1] =1 ; Cubevertex[10].normal[2] =0 ; Cubevertex[10].normal[3] = 0.0;
	Cubevertex[11].normal[0] =0 ; Cubevertex[11].normal[1] =1 ; Cubevertex[11].normal[2] =0 ; Cubevertex[11].normal[3] = 0.0;
	Cubevertex[12].normal[0] =0 ; Cubevertex[12].normal[1] =-1 ; Cubevertex[12].normal[2] =0 ; Cubevertex[12].normal[3] = 0.0;
	Cubevertex[13].normal[0] =0 ; Cubevertex[13].normal[1] =-1; Cubevertex[13].normal[2] =0 ; Cubevertex[13].normal[3] = 0.0;
	Cubevertex[14].normal[0] =0 ; Cubevertex[14].normal[1] =-1 ; Cubevertex[14].normal[2] =0 ; Cubevertex[14].normal[3] = 0.0;
	Cubevertex[15].normal[0] =0 ; Cubevertex[15].normal[1] =-1 ; Cubevertex[15].normal[2] =0 ; Cubevertex[15].normal[3] = 0.0;

	Cubevertex[16].normal[0] =0 ; Cubevertex[16].normal[1] =0 ; Cubevertex[16].normal[2] =1 ; Cubevertex[16].normal[3] = 0.0;
	Cubevertex[17].normal[0] =0 ; Cubevertex[17].normal[1] =0 ; Cubevertex[17].normal[2] =1 ; Cubevertex[17].normal[3] = 0.0;
	Cubevertex[18].normal[0] =0 ; Cubevertex[18].normal[1] =0 ; Cubevertex[18].normal[2] =1 ; Cubevertex[18].normal[3] = 0.0;
	Cubevertex[19].normal[0] =0 ; Cubevertex[19].normal[1] =0 ; Cubevertex[19].normal[2] =1 ; Cubevertex[19].normal[3] = 0.0;
	Cubevertex[20].normal[0] =0 ; Cubevertex[20].normal[1] =0 ; Cubevertex[20].normal[2] =-1 ; Cubevertex[20].normal[3] = 0.0;
	Cubevertex[21].normal[0] =0 ; Cubevertex[21].normal[1] =0 ; Cubevertex[21].normal[2] =-1 ; Cubevertex[21].normal[3] = 0.0;
	Cubevertex[22].normal[0] =0 ; Cubevertex[22].normal[1] =0 ; Cubevertex[22].normal[2] =-1 ; Cubevertex[22].normal[3] = 0.0;
	Cubevertex[23].normal[0] =0 ; Cubevertex[23].normal[1] =0 ; Cubevertex[23].normal[2] =-1 ; Cubevertex[23].normal[3] = 0.0;

	for (int i = 0; i < 24; i++) {
		Cubevertex[i].color[0] = color[0] ; Cubevertex[i].color[1] = color[1]; Cubevertex[i].color[2] = color[2];
	}

	Cubevertex[0].texCoord[0] = 0; Cubevertex[0].texCoord[1] = 1; 	Cubevertex[1].texCoord[0] = 0; Cubevertex[1].texCoord[1] = 0;
	Cubevertex[2].texCoord[0] = 1; Cubevertex[2].texCoord[1] = 0;	Cubevertex[3].texCoord[0] = 1; Cubevertex[3].texCoord[1] = 1;/////positive x
	Cubevertex[4].texCoord[0] = 0; Cubevertex[4].texCoord[1] = 1;	Cubevertex[5].texCoord[0] = 0; Cubevertex[5].texCoord[1] = 0;
	Cubevertex[6].texCoord[0] = 1; Cubevertex[6].texCoord[1] = 0;	Cubevertex[7].texCoord[0] = 1; Cubevertex[7].texCoord[1] = 1;////////negative x

	Cubevertex[8].texCoord[0] = 0; Cubevertex[8].texCoord[1] = 1;	Cubevertex[9].texCoord[0] = 0; Cubevertex[9].texCoord[1] = 0;
	Cubevertex[10].texCoord[0] = 1; Cubevertex[10].texCoord[1] = 0;	Cubevertex[11].texCoord[0] = 1; Cubevertex[11].texCoord[1] = 1;/////positive y
	Cubevertex[12].texCoord[0] = 0; Cubevertex[12].texCoord[1] = 1;	Cubevertex[13].texCoord[0] = 0; Cubevertex[13].texCoord[1] = 0;
	Cubevertex[14].texCoord[0] = 1; Cubevertex[14].texCoord[1] = 0;	Cubevertex[15].texCoord[0] = 1; Cubevertex[15].texCoord[1] = 1;////////negative y

	Cubevertex[16].texCoord[0] = 0; Cubevertex[16].texCoord[1] = 1;	Cubevertex[17].texCoord[0] = 0; Cubevertex[17].texCoord[1] = 0;
	Cubevertex[18].texCoord[0] = 1; Cubevertex[18].texCoord[1] = 0;	Cubevertex[19].texCoord[0] = 1; Cubevertex[19].texCoord[1] = 1;/////positive z
	Cubevertex[20].texCoord[0] = 0; Cubevertex[20].texCoord[1] = 1;	Cubevertex[21].texCoord[0] = 0; Cubevertex[21].texCoord[1] = 0;
	Cubevertex[22].texCoord[0] = 1; Cubevertex[22].texCoord[1] = 0;	Cubevertex[23].texCoord[0] = 1; Cubevertex[23].texCoord[1] = 1;////////negative z

	Cubeindice[0] = 0;   Cubeindice[1] = 1;   Cubeindice[2] = 2;			Cubeindice[3] = 0;   Cubeindice[4] = 2;   Cubeindice[5] = 3;
	Cubeindice[6] = 4;   Cubeindice[7] = 5;   Cubeindice[8] = 6;			Cubeindice[9] = 4;   Cubeindice[10] = 6;   Cubeindice[11] = 7;
	Cubeindice[12] = 8;   Cubeindice[13] = 9;   Cubeindice[14] = 10;		Cubeindice[15] = 8;   Cubeindice[16] = 10;   Cubeindice[17] = 11;
	Cubeindice[18] = 12;   Cubeindice[19] = 13;   Cubeindice[20] = 14;	Cubeindice[21] = 12;   Cubeindice[22] = 14;   Cubeindice[23] = 15;
	Cubeindice[24] = 16;   Cubeindice[25] = 17;   Cubeindice[26] = 18;	Cubeindice[27] = 16;   Cubeindice[28] = 18;   Cubeindice[29] = 19;
	Cubeindice[30] = 20;   Cubeindice[31] = 21;   Cubeindice[32] = 22;	Cubeindice[33] = 20;   Cubeindice[34] = 22;   Cubeindice[35] = 23;
}
//void InitCylinder(float baser, float topr, float height, int numslices, int numstacks, float color[3]) {
//	int ncylindervertices = numstacks*numslices;          //number of cylinder vertices
//	cylindervertex = new myVertex[ncylindervertices];
//	float Dangle = 2 * Pi / (float)(numslices - 1);
//
//	for (int j = 0; j<numstacks; j++) {
//		for (int i = 0; i < numslices; i++) {
//			int idx = j*numslices + i; // mesh[j][i] 
//			float angle = Dangle * i;
//			float radius = baser + j* (topr - baser) / (float)(numstacks - 1);
//			cylindervertex[idx].location[0] = cylindervertex[idx].normal[0] = cos(angle)*radius;
//			cylindervertex[idx].location[1] = cylindervertex[idx].normal[1] = sin(angle)*radius;
//			cylindervertex[idx].location[2] = j*1.0*height / (float)(numstacks - 1);
//			cylindervertex[idx].location[3] = 1.0;
//			cylindervertex[idx].normal[2] = 0.0; cylindervertex[idx].normal[3] = 0.0;
//			cylindervertex[idx].color[0] = color[0];
//			cylindervertex[idx].color[1] = color[1];
//			cylindervertex[idx].color[2] = color[2];
//			cylindervertex[idx].color[3] = 1.0;
//		}
//		//cout << cylindervertex[j*numslices].location[2] << endl;
//	}
//	// now create the index array 
//	ncylinderindices = (numstacks - 1) * 2 * (numslices + 1);
//	cylinderindice = new GLubyte[ncylinderindices];
//	int n = 0;
//	for (int j = 0; j<numstacks - 1; j++) {
//		for (int i = 0; i <= numslices; i++) {
//			int mi = i % numslices;
//			int idx = j*numslices + mi; // mesh[j][mi] 
//			int idx2 = (j + 1) * numslices + mi;
//			cylinderindice[n++] = idx;
//			cylinderindice[n++] = idx2;
//		}
//	}
//}
//
void InitSphere(float radius, int numslices, int numstacks, float color[3]) {
	int nsphvertices = numstacks*numslices;
	nsphindices = (numstacks - 1) * 2 * (numslices + 1);
	spherevertex = new myVertex[nsphvertices];
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
void InitVBOPly(float min[3],float max[3]) {
	int ply_nvertices = ply_vertexcount;
	ply_nindices = ply_facecount * 3;

	ply_verts = new myVertex[ply_nvertices];
	ply_indices = new GLuint[ply_nindices];
	
	min[0] = min[1] = min[2] = 999999999;
	max[0] = max[1] = max[2] = -999999999;

	for (int i = 0; i < ply_vertexcount; i++) {
		ply_verts[i].location[0] = ply_vertices[i]->x;
		ply_verts[i].location[1] = ply_vertices[i]->y;
		ply_verts[i].location[2] = ply_vertices[i]->z;
		ply_verts[i].location[3] = 1.0;

		if (ply_verts[i].location[0] < min[0]) min[0] = ply_verts[i].location[0];
		if (ply_verts[i].location[1] < min[1]) min[1] = ply_verts[i].location[1];
		if (ply_verts[i].location[2] < min[2]) min[2] = ply_verts[i].location[2];

		if (ply_verts[i].location[0] > max[0]) max[0] = ply_verts[i].location[0];
		if (ply_verts[i].location[1] > max[1]) max[1] = ply_verts[i].location[1];
		if (ply_verts[i].location[2] > max[2]) max[2] = ply_verts[i].location[2];

		ply_verts[i].normal[0] = ply_vertices[i]->nx;
		ply_verts[i].normal[1] = ply_vertices[i]->ny;
		ply_verts[i].normal[2] = ply_vertices[i]->nz;
		ply_verts[i].normal[3] = 0.0;

		ply_verts[i].color[0] = 1.0;
		ply_verts[i].color[1] = 1.0;
		ply_verts[i].color[2] = 1.0;
		ply_verts[i].color[3] = 1.0;
	}

	for (int i = 0; i < ply_facecount; i++) {
		if (ply_faces[i]->count != 3) printf("No!\n");
		ply_indices[i * 3] = ply_faces[i]->vertices[0];
		ply_indices[i * 3 + 1] = ply_faces[i]->vertices[1];
		ply_indices[i * 3 + 2] = ply_faces[i]->vertices[2];
	}

	glGenBuffers(1, vboHandlePly);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandlePly[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex) * ply_nvertices, ply_verts, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up

	glGenBuffers(1, &indexVBOPly);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOPly);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * ply_nindices, ply_indices, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
}
void InitVBOSquare() {
	float color[3] = { 1,1,1 };
	InitSquare(0.5, color);
	glGenBuffers(1, vboHandleSquare);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleSquare[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex) * 4, Squarevertex, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up 

	glGenBuffers(1, &indexVBOSquare);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOSquare);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, Squareindice, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up 
}
void InitVBOCube() {
	float color[3] = { 0.5,0.5,0.5 };
	InitCube(1, color);
	glGenBuffers(1, vboHandleCube);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCube[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex) * 24, Cubevertex, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up 

	glGenBuffers(1, &indexVBOCube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 36, Cubeindice, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up 
}
//
//void InitVBOMaze() {
//	getmazevertex();
//	glGenBuffers(1, vboHandleMaze);   // create an interleaved VBO object
//	glBindBuffer(GL_ARRAY_BUFFER, vboHandleMaze[0]);   // bind the first handle
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 162, MAZE, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up
//
//	for (int i = 0; i < 54; i++) {
//		mazenum[i] = i;
//	}
//	glGenBuffers(1, &indexVBOMaze);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOMaze);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 54, mazenum, GL_STATIC_DRAW);  // load the index data
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
//}
//void InitVBOCircle() {
//	float color[3] = { 1, 1, 1 };
//	getcirclevertex(1, color);
//	glGenBuffers(1, vboHandleCir);   // create an interleaved VBO object
//	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCir[0]);   // bind the first handle 
//	glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex) * 25, circlevertex, GL_STATIC_DRAW); // allocate space and copy the position data over
//	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up
//
//	for (int i = 0; i < 25; i++) {
//		cirseq[i] = i;
//	}
//	glGenBuffers(1, &indexVBOCir);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCir);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 25, cirseq, GL_STATIC_DRAW);  // load the index data 
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
//}
//void InitVBOCylinder(int ncylslices, int ncylstacks) {
//	float color[3] = { 1.0,1.0,1.0 };
//	InitCylinder(1, 1, 1, ncylslices, ncylstacks, color);
//	int ncylindervertices = ncylstacks*ncylslices;          //number of cylinder vertices
//	ncylinderindices = (ncylstacks - 1) * 2 * (ncylslices + 1);
//
//	glGenBuffers(1, vboHandleCyl);   // create an interleaved VBO object
//	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCyl[0]);   // bind the first handle 
//	glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex) * ncylindervertices, cylindervertex, GL_STATIC_DRAW); // allocate space and copy the position data over
//	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up
//
//	glGenBuffers(1, &indexVBOCyl);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCyl);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * ncylinderindices, cylinderindice, GL_STATIC_DRAW);  // load the index data 
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
//}
//
void InitVBOSphere(int nsphslices, int nsphstacks) {
	float color[3] = { 1.0, 1.0, 1.0 };
	InitSphere(1, nsphslices, nsphstacks, color);
	int nsphvertices = nsphstacks*nsphslices;
	nsphindices = (nsphstacks - 1) * 2 * (nsphslices + 1);

	glGenBuffers(1, vboHandleSph);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleSph[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(myVertex) * nsphvertices, spherevertex, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up

	glGenBuffers(1, &indexVBOSph);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOSph);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * nsphindices, sphereindice, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up
}

//////////////////////////////////////////////////////////////////////////////////////////
void draw_Ply(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4, GLuint m5) {
	glBindBuffer(GL_ARRAY_BUFFER, vboHandlePly[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOPly);

	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 0);  // position 
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 32); // color
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 16); // normal

	glm::mat4 eye2world = glm::inverse(world2eye);
	glm::mat4 normal_matrix = glm::inverse(local2eye);
	normal_matrix = glm::transpose(normal_matrix);

	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
	glUniformMatrix4fv(m5, 1, GL_FALSE, (float*)&eye2world[0][0]);
	glDrawElements(GL_TRIANGLES, ply_nindices, GL_UNSIGNED_INT, (char*)NULL + 0);
}
void draw_Square(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint c3, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleSquare[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOSquare);
	glEnableVertexAttribArray(c3);
	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 0);  // position 
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 32); // color
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 16); // normal
	glVertexAttribPointer(c3, 2, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 48);//tex
																						 //glm::mat4 eye2world = glm::inverse(world2eye);
	glm::mat4 normal_matrix = glm::inverse(local2eye);
	normal_matrix = glm::transpose(normal_matrix);

	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
	//glUniformMatrix4fv(m5, 1, GL_FALSE, (float*)&eye2world[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (char*)NULL + 0);
}
void draw_Cube_envmap(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4, GLuint m5) {
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCube[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCube);

	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 0);  // position 
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 32); // color
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 16); // normal

	glm::mat4 eye2world = glm::inverse(world2eye);
	glm::mat4 normal_matrix = glm::inverse(local2eye);
	normal_matrix = glm::transpose(normal_matrix);

	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
	glUniformMatrix4fv(m5, 1, GL_FALSE, (float*)&eye2world[0][0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (char*)NULL + 0);
}
void draw_sphere(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4, GLuint m5) {
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleSph[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOSph);

	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 0);  // position 
	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 32); // color
	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 16); // normal

	glm::mat4 eye2world = glm::inverse(world2eye);
	glm::mat4 normal_matrix = glm::inverse(local2eye);
	normal_matrix = glm::transpose(normal_matrix);

	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
	//glUniformMatrix4fv(m5, 1, GL_FALSE, (float*)&eye2world[0][0]);
	glDrawElements(GL_TRIANGLE_STRIP, nsphindices, GL_UNSIGNED_BYTE, (char*)NULL + 0);
}
//void draw_Cube(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
//	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCube[0]);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCube);
//
//	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 0);  // position 
//	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 32); // color
//	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 16); // normal
//
//	//glm::mat4 eye2world = glm::inverse(world2eye);
//	glm::mat4 normal_matrix = glm::inverse(local2eye);
//	normal_matrix = glm::transpose(normal_matrix);
//
//	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
//	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
//	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
//	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
//	//glUniformMatrix4fv(m5, 1, GL_FALSE, (float*)&eye2world[0][0]);
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (char*)NULL + 0);
////}
//void draw_maze(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint c3, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
//	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCube[0]);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCube);
//	glEnableVertexAttribArray(c3);
//	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 0);  // position 
//	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 32); // color
//	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 16); // normal
//	glVertexAttribPointer(c3, 2, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 48);//tex
//	//glm::mat4 eye2world = glm::inverse(world2eye);
//	glm::mat4 normal_matrix = glm::inverse(local2eye);
//	normal_matrix = glm::transpose(normal_matrix);
//
//	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
//	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
//	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
//	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
//	//glUniformMatrix4fv(m5, 1, GL_FALSE, (float*)&eye2world[0][0]);
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (char*)NULL + 0);
//}
//void draw_circle(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
//	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCir[0]);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCir);
//
//	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 0);  // position 
//	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 32); // color
//	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 16); // normal
//
//	//glm::mat4 eye2world = glm::inverse(world2eye);
//	glm::mat4 normal_matrix = glm::inverse(local2eye);
//	normal_matrix = glm::transpose(normal_matrix);
//
//	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
//	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
//	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix
//	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
//	//glUniformMatrix4fv(m5, 1, GL_FALSE, (float*)&eye2world[0][0]);
//	glDrawElements(GL_TRIANGLE_FAN, 25, GL_UNSIGNED_BYTE, (char*)NULL + 0);
//}
//void draw_cylinder(const glm::mat4 &local2clip, const glm::mat4 &local2eye, const glm::mat4 &world2eye, float color[3], GLuint c0, GLuint c1, GLuint c2, GLuint m1, GLuint m2, GLuint m3, GLuint m4) {
//	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCyl[0]);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCyl);
//
//	glVertexAttribPointer(c0, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 0);  // position 
//	glVertexAttribPointer(c1, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 32); // color
//	glVertexAttribPointer(c2, 4, GL_FLOAT, GL_FALSE, sizeof(myVertex), (char*)NULL + 16); // normal
//
//	//glm::mat4 eye2world = glm::inverse(world2eye);
//	glm::mat4 normal_matrix = glm::inverse(local2eye);
//	normal_matrix = glm::transpose(normal_matrix);
//
//	glUniformMatrix4fv(m1, 1, GL_FALSE, (float*)&local2clip[0][0]);   // pass the local2clip matrix
//	glUniformMatrix4fv(m2, 1, GL_FALSE, (float*)&local2eye[0][0]);   // pass the local2eye matrix
//	glUniformMatrix4fv(m3, 1, GL_FALSE, (float*)&normal_matrix[0][0]);   // pass the local2eye matrix 
//	glUniformMatrix4fv(m4, 1, GL_FALSE, (float*)&world2eye[0][0]);
//	//glUniformMatrix4fv(m5, 1, GL_FALSE, (float*)&eye2world[0][0]);
//	glDrawElements(GL_TRIANGLE_STRIP, ncylinderindices, GL_UNSIGNED_BYTE, (char*)NULL + 0);
//}


/////////////////////////////////////////////////////////////
void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (WIRE_FRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glUseProgram(programObject);

	GLuint c0 = glGetAttribLocation(programObject, "position");	GLuint c1 = glGetAttribLocation(programObject, "color");
	GLuint c2 = glGetAttribLocation(programObject, "normal");GLuint c3 = glGetAttribLocation(programObject, "texCoord");

	GLuint m1 = glGetUniformLocation(programObject, "local2clip");	GLuint m2 = glGetUniformLocation(programObject, "local2eye");
	GLuint m3 = glGetUniformLocation(programObject, "normal_matrix");	GLuint m4 = glGetUniformLocation(programObject, "world2eye");
	GLuint m5 = glGetUniformLocation(programObject, "eye2world");

	GLuint Ia = glGetUniformLocation(programObject, "light_ambient");	GLuint Id = glGetUniformLocation(programObject, "light_diffuse");
	GLuint Is = glGetUniformLocation(programObject, "light_specular");	GLuint Lpos = glGetUniformLocation(programObject, "light_pos");

	GLuint Ka = glGetUniformLocation(programObject, "mat_ambient");	GLuint Kd = glGetUniformLocation(programObject, "mat_diffuse");
	GLuint Ks = glGetUniformLocation(programObject, "mat_specular");GLuint Shine = glGetUniformLocation(programObject, "mat_shine");
	GLuint LightSource= glGetUniformLocation(programObject, "lightSource");

	GLuint b1 = glGetUniformLocation(programObject, "use_texture");
	use_texture = 0;
	glUniform1i(b1, use_texture);

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
	glm::mat4 view = glm::lookAt(glm::vec3( 0, 0, 1.5), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 modelall = glm::mat4(1.0f);
	modelall = glm::rotate(modelall, y_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	modelall = glm::rotate(modelall, x_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	modelall = glm::scale(modelall, glm::vec3(scale_size, scale_size, scale_size));

	glm::mat4 mvp;
	glm::mat4 mv;
	///////////////////light sphere/////////////////////
	glUniform1f(LightSource, lightSource[1]);
	float color[3] = { 1.0, 0.0, 1.0 };
	mat_stack_.push(lightM);
	lightM = glm::scale(lightM, glm::vec3(0.04,0.04, 0.04));
	mvp = projection*view*modelall*lightM;
	mv = view*modelall*lightM;
	SetMaterialColor(Kd, 1, 1, 1, 1);
	draw_sphere(mvp, mv, view, color, c0, c1, c2, m1, m2, m3, m4,m5);
	lightM = mat_stack_.top();
	mat_stack_.pop();

	glUniform1f(LightSource, lightSource[0]);
////////////////////cube room//////////////////////////////
	float ang180 = Pi;
	//Init_texture(fname6);
	use_texture = 1;
	glUniform1i(b1, use_texture);
	GLuint tex_loc = glGetUniformLocation(programObject, "Tex1");
	glUniform1i(tex_loc, 5);
	SetMaterialColor(Kd, 1, 1, 1, 1);
	glm::mat4 roomM = glm::mat4(1.0f);
	roomM = glm::translate(modelall, glm::vec3(0, 0, -1.5));///////////////////negavtive z
	roomM = glm::scale(roomM, glm::vec3(3, 3, 1));
	mvp = projection*view*roomM;
	mv = view*roomM;
	draw_Square(mvp, mv, view, color, c0, c1, c2, c3, m1, m2, m3, m4);

	//Init_texture(fname5);
	use_texture = 1;
	glUniform1i(b1, use_texture);
	tex_loc = glGetUniformLocation(programObject, "Tex1");
	glUniform1i(tex_loc, 4);
	roomM = glm::translate(modelall, glm::vec3(0, 0, 1.5));////////positive z
	roomM = glm::rotate(roomM, ang180, glm::vec3(0.0f, 1.0f, 0.0f));
	roomM = glm::scale(roomM, glm::vec3(3, 3, 1));
	mvp = projection*view*roomM;
	mv = view*roomM;
	draw_Square(mvp, mv, view, color, c0, c1, c2, c3, m1, m2, m3, m4);

	float ang1 = Pi/2;
	//Init_texture(fname2);
	use_texture = 1;
	glUniform1i(b1, use_texture);
	tex_loc = glGetUniformLocation(programObject, "Tex1");
	glUniform1i(tex_loc, 1);
	roomM = glm::rotate(modelall, ang1, glm::vec3(0.0f, 1.0f, 0.0f)); /////////////////negative x////////////
	roomM = glm::translate(roomM, glm::vec3(0, 0, -1.5));
	roomM = glm::scale(roomM, glm::vec3(3, 3, 1));
	mvp = projection*view*roomM;
	mv = view*roomM;
	draw_Square(mvp, mv, view, color, c0, c1, c2, c3, m1, m2, m3, m4);

	//Init_texture(fname1);
	use_texture = 1;
	glUniform1i(b1, use_texture);
	tex_loc = glGetUniformLocation(programObject, "Tex1");
	glUniform1i(tex_loc, 0);
	roomM = glm::rotate(modelall, -ang1, glm::vec3(0.0f, 1.0f, 0.0f));/////////////////////////////positive x
	roomM = glm::translate(roomM, glm::vec3(0, 0, -1.5));
	roomM = glm::scale(roomM, glm::vec3(3, 3, 1));
	mvp = projection*view*roomM;
	mv = view*roomM;
	draw_Square(mvp, mv, view, color, c0, c1, c2, c3, m1, m2, m3, m4);

	//Init_texture(fname3);
	use_texture = 1;
	glUniform1i(b1, use_texture);
	tex_loc = glGetUniformLocation(programObject, "Tex1");
	glUniform1i(tex_loc, 2);
	roomM = glm::rotate(modelall, -ang1, glm::vec3(1.0f, 0.0f, 0.0f));/////////positive y
	roomM = glm::translate(roomM, glm::vec3(0, 0, 1.5));
	roomM = glm::rotate(roomM, ang180, glm::vec3(0.0f, 1.0f, 0.0f));
	roomM = glm::scale(roomM, glm::vec3(3, 3, 1));
	mvp = projection*view*roomM;
	mv = view*roomM;
	draw_Square(mvp, mv, view, color, c0, c1, c2, c3, m1, m2, m3, m4);
	
	//Init_texture (fname4);
	use_texture = 1;
	glUniform1i(b1, use_texture);
	tex_loc = glGetUniformLocation(programObject, "Tex1");
	glUniform1i(tex_loc, 3);
	roomM = glm::rotate(modelall, ang1, glm::vec3(1.0f, 0.0f, 0.0f));///////negative y
	roomM = glm::translate(roomM, glm::vec3(0, 0, 1.5));
	roomM = glm::rotate(roomM, -ang180, glm::vec3(0.0f, 1.0f, 0.0f));
	roomM = glm::scale(roomM, glm::vec3(3, 3, 1));
	mvp = projection*view*roomM;
	mv = view*roomM;
	draw_Square(mvp, mv, view, color, c0, c1, c2, c3, m1, m2, m3, m4);


	///////////////draw maze///////////////////////
	//Init_texture(fname1);
	//use_texture = 1;
	//glUniform1i(b1, use_texture);
	//tex_loc = glGetUniformLocation(programObject, "Tex1");
	//glUniform1i(tex_loc, 0);
	//color[0] = 1; color[1] = 0.0; color[2] = 0.8;
	//for (int i = 0; i < 160; i += 3) {
	//	mat_stack_.push(modelM_);
	//	modelM_ = glm::translate(modelM_, glm::vec3(MAZE[i], MAZE[i + 1], MAZE[i + 2]));
	//	modelM_ = glm::scale(modelM_, glm::vec3(0.1f, 0.1f,0.08f));
	//	mvp = projection*view*modelall*modelM_;
	//	mv = view*modelall*modelM_;
	//	SetMaterialColor(Kd, 1.0, 0.0, 0.8, 1);
	//	draw_maze(mvp, mv, view, color, c0, c1, c2, c3, m1, m2, m3, m4);
	//	modelM_ = mat_stack_.top();
	//	mat_stack_.pop();
	//}

	//////////////////candy///////////////////////////
	//use_texture = 0;
	//glUniform1i(b1, use_texture);
	//color[0] = 1; color[1] = 1; color[2] = 0.2;
	//mat_stack_.push(modelM_);
	//modelM_ = glm::translate(modelM_, glm::vec3(0.9f, 0.5f, -0.2f));
	//modelM_ = glm::scale(modelM_, glm::vec3(0.1f, 0.1f, 0.1f));

	//mvp = projection*view*modelall*modelM_;
	//mv = view*modelall*modelM_;
	//SetMaterialColor(Kd, 1.0, 1.0, 0.2, 1);
	//draw_sphere(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4);
	//modelM_ = mat_stack_.top();
	//mat_stack_.pop();


	///////////////////////draw sun////////////////////////////
	use_texture = 0;
	glUniform1i(b1, use_texture);
	color[0] = 1; color[1] = 0.0; color[2] = 0.0;
	mat_stack_.push(modelM_);
	modelM_ = glm::translate(modelM_, glm::vec3(-1.0f, 0.7f, -0.5f));
	modelM_ = glm::scale(modelM_, glm::vec3(0.3f, -0.2f, 0.3f));

	mvp = projection*view*modelall*modelM_;
	mv = view*modelall*modelM_;
	SetMaterialColor(Kd, 1.0, 0.0, 0.0, 1);
	draw_sphere(mvp,mv,view,color,c0,c1,c2,m1,m2,m3,m4,m5);
	modelM_ = mat_stack_.top();
	mat_stack_.pop();
	

	///////////////////draw ply model/////////////////////////////////////////////
	use_texture = 0;
	glUniform1i(b1, use_texture);
	//tex_loc = glGetUniformLocation(programObject, "cubMap");
	//glUniform1i(tex_loc, 1);

	float ply_center[3], ply_size[3];
	ply_center[0] = (ply_min[0] + ply_max[0]) / 2.0;
	ply_center[1] = (ply_min[1] + ply_max[1]) / 2.0;
	ply_center[2] = (ply_min[2] + ply_max[2]) / 2.0;

	ply_size[0] = (ply_max[0] - ply_min[0]);
	ply_size[1] = (ply_max[1] - ply_min[1]);
	ply_size[2] = (ply_max[2] - ply_min[2]);

	mat_stack.push(plyM);
	plyM = glm::translate(plyM, glm::vec3(-1 * ply_center[0], -1 * ply_center[1], -1 * ply_center[2]));
	plyM = glm::translate(plyM, glm::vec3(0.2, -1.4, -0.7));
	float ang = -Pi/180*15;
	plyM = glm::rotate(plyM, ang, glm::vec3(0.0f, 0.0f, 1.0f));
	plyM = glm::scale(plyM, glm::vec3(0.3 / ply_size[0], 0.3 / ply_size[1], 0.3 / ply_size[2]));
	plyM = glm::scale(plyM, glm::vec3(7,3,3)*b);
	mvp = projection*view*modelall*plyM;
	mv = view*modelall*plyM;
	SetMaterialColor(Kd,1,1,0,1.0);
	draw_Ply(mvp, mv, view, color, c0, c1, c2, m1, m2, m3, m4, m5);
	plyM = mat_stack.top();
	mat_stack.pop();

	glDisableClientState(GL_VERTEX_ARRAY);
	

	//////////////////////////cube reflection //////////////////
	use_texture = 2;
	glUniform1i(b1, use_texture);
	tex_loc = glGetUniformLocation(programObject, "cubeMap");
	glUniform1i(tex_loc, 6);
	glm::mat4 cubeM = glm::mat4(1.0f);
	mat_stack.push(cubeM);
	cubeM = glm::translate(cubeM, glm::vec3(0.2, 0.8, -0.7));
	cubeM = glm::rotate(cubeM,angx,glm::vec3(1.0f,0.0f,0.0f));
	cubeM = glm::rotate(cubeM, angy, glm::vec3(0.0f, 1.0f, 0.0f));
	cubeM = glm::rotate(cubeM, angz, glm::vec3(0.0f, 0.0f, 1.0f));
	cubeM = glm::scale(cubeM, glm::vec3(0.4, 0.4, 0.4)*b);
	mvp = projection*view*modelall*cubeM;
	mv = view*modelall*cubeM;
	SetMaterialColor(Kd, 1, 0.5, 0.5, 0.5);
	draw_Cube_envmap(mvp, mv, view, color, c0, c1, c2, m1, m2, m3, m4, m5);
	cubeM = mat_stack.top();mat_stack.pop();

	mat_stack.push(cubeM);
	cubeM = glm::translate(cubeM, glm::vec3(-1.1, -0.4, 0.8));
	cubeM = glm::scale(cubeM, glm::vec3(0.2, 0.2, 0.2));
	mvp = projection*view*modelall*cubeM;
	mv = view*modelall*cubeM;
	SetMaterialColor(Kd, 1, 0.5, 0.5, 0.5);
	draw_sphere(mvp, mv, view, color, c0, c1, c2, m1, m2, m3, m4, m5);
	cubeM = mat_stack.top();mat_stack.pop();

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
/////////////////////////////////////////////////////////////////
void move(void)
{
	angx += Pi / 18000 * 15;
	angy += Pi / 18000 * 15;
	angz += Pi / 18000 * 15;
	//angle9 += Pi / 5400;
	//angle1 += (Pi / 3600);
	//if (angle1 >= 3 * Pi / 5 || angle1 <= -3 * Pi / 5) {
	//	angle1 = -3 * Pi / 5;
	//}
	//angle3 += (Pi / 3600);
	//if (angle3 >= 3 * Pi / 5 || angle3 <= -3 * Pi / 5) {
	//	angle3 = -3 * Pi / 5;
	//}
	//angle2 += (Pi / 3600);
	//if (angle2 >= 3 * Pi / 5 || angle2 <= -3 * Pi / 5) {
	//	angle2 = -3 * Pi / 5;
	//}
	//angle4 += (Pi / 3600);
	//if (angle4 >= 3 * Pi / 5 || angle4 <= -3 * Pi / 5) {
	//	angle4 = -3 * Pi / 5;
	//}
	//angle5 += -(Pi / 7200);
	//if (angle5 >= -Pi / 4 || angle5 <= -3 * Pi / 4) {
	//	angle5 = -Pi / 4;
	//}
	//angle6 += -(Pi / 7200);
	//if (angle6 >= Pi / 4 || angle6 <= -Pi / 4) {
	//	angle6 = Pi / 4;
	//}
	//angle7 += (Pi / 7200);
	//if (angle7 <= Pi / 4 || angle7 >= 3 * Pi / 4) {
	//	angle7 = Pi / 4;
	//}
	//angle8 += (Pi / 7200);
	//if (angle8 >= Pi / 4 || angle8 <= -Pi / 4) {
	//	angle8 = -Pi / 4;
	//}
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
		plyM = glm::rotate(plyM, d_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (key == 'T') {
		d_angle += -Pi / 180 * 15;
		plyM = glm::rotate(plyM, d_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (key == 'x') {
		d_angle += -Pi / 180 * 15;
		plyM = glm::rotate(plyM, d_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (key == 'y') {
		d_angle += -Pi / 180 * 15;
		plyM = glm::rotate(plyM, d_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (key == 'r')
		plyM = glm::translate(plyM, glm::vec3(0.1f, 0.0f, 0.0f));
	if (key == 'l')
		plyM = glm::translate(plyM, glm::vec3(-0.1f, 0.0f, 0.0f));
	if (key == 'f')
		plyM = glm::translate(plyM, glm::vec3(0.0f, 0.1f, 0.0f));
	if (key == 'b')
		plyM = glm::translate(plyM, glm::vec3(0.0f, -0.1f, 0.0f));
	if (key == 'u')
		plyM = glm::translate(plyM, glm::vec3(0.0f, 0.0f, 0.1f));
	if (key == 'd')
		plyM = glm::translate(plyM, glm::vec3(0.0f, 0.0f, -0.1f));

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
	//	modelm = glm::mat4(1.0f);
	//	modelm = glm::translate(modelm, glm::vec3(-0.71, -0.8, -0.2));
	//	angle1 = -pi / 2, angle2 = 0;
	//	angle3 = pi / 2, angle4 = 0;
	//	angle5 = -pi / 2, angle6 = 0;
	//	angle7 = pi / 2, angle8 = 0;
	//	angle9 = 0;
		b = 1.0;
		x_angle = 0; y_angle = 0;
	//	float a = 90;
	//	modelm = glm::rotate(modelm, a, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	/*if (key == '1') {
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
*/
	glutPostRedisplay();
}
int main(int argc, char** argv){
	PlyFile* input;
	FILE* fp = fopen("newdragon.ply", "r");
	input = read_ply(fp);

	store_ply(input, &ply_vertices, &ply_faces, &ply_vertexcount, &ply_facecount, &ply_vertexnormals, &ply_facenormals);
	close_ply(input);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);

	glutCreateWindow("Xuerong Hu's Lab 5");
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

	InitVBOPly(ply_min, ply_max);
	printf(" ply bound: [%f %f %f]:[%f %f %f]\n", ply_min[0],ply_min[1],ply_min[3],ply_max[0],ply_max[1],ply_max[2]);

	int ncylstacks = 10;	int ncylslices = 20;
	int nsphstacks = 10;	int nsphslices = 20;
	////////////////////////////////////////////////////////
	//InitVBOCircle();
	//InitVBOMaze();
	InitVBOCube();
	InitVBOSquare();
	//InitVBOCylinder(ncylslices, ncylstacks);
	InitVBOSphere(nsphslices, nsphstacks);
	//////////////////////////////////////////////////////////
	read_Image(fname1);
	for (int i=0;i<256; i++) {
		for (int j=0;j<256; j++) {
			for (int k=0;k<4; k++) {
				texImage1[i][j][k]=texImage[i][j][k];
			}
		}
	}
	read_Image(fname2);
	for (int i = 0; i<256; i++) {
		for (int j = 0; j<256; j++) {
			for (int k = 0; k<4; k++) {
				texImage2[i][j][k] = texImage[i][j][k];
			}
		}
	}
	read_Image(fname3);
	for (int i = 0; i<256; i++) {
		for (int j = 0; j<256; j++) {
			for (int k = 0; k<4; k++) {
				texImage3[i][j][k] = texImage[i][j][k];
			}
		}
	}
	read_Image(fname4);
	for (int i = 0; i<256; i++) {
		for (int j = 0; j<256; j++) {
			for (int k = 0; k<4; k++) {
				texImage4[i][j][k] = texImage[i][j][k];
			}
		}
	}
	read_Image(fname5);
	for (int i = 0; i<256; i++) {
		for (int j = 0; j<256; j++) {
			for (int k = 0; k<4; k++) {
				texImage5[i][j][k] = texImage[i][j][k];
			}
		}
	}
	read_Image(fname6);
	for (int i = 0; i<256; i++) {
		for (int j = 0; j<256; j++) {
			for (int k = 0; k<4; k++) {
				texImage6[i][j][k] = texImage[i][j][k];
			}
		}
	}

	///////////////////////////////////////////////////////////
	programObject = SetupGLSL("lab5");
	Init_texture(fname1,fname2,fname3,fname4,fname5,fname6);
	Init_cubemap();
	glutMainLoop();
	delete[] MAZE;
}
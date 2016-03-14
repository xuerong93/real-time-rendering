
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack> 
#include <math.h> 
#include <iostream>
#define Pi 3.1415926

using namespace std;
#include <glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h> 
#include <OpenGL/gl.h> 
#else 
#include <GL/glut.h> 
#include <GL/gl.h> 

#endif

#include<glm.hpp> 
#include<gtx/transform.hpp> 


float b = 0.8f;
float vertices[] = { -0.05, -0.05, 0.0,  // first triangle
0.05, -0.05, 0.0,
0.05, 0.05, 0.0,
-0.05, 0.05, 0.0, };
float circlevertex[75];
float roundrec[249];
//float MAZE[138];
float *MAZE;



GLubyte tindices[6];
GLubyte cirseq[25];
GLubyte roundrecnum[83];
GLubyte mazenum[54];

GLuint vboHandle[1];   // a VBO that contains interleaved positions and colors 
GLuint indexVBO;

GLuint vboHandleCir[1];
GLuint indexVBOCir;

GLuint vboHandlemaze[1];
GLuint indexVBOmaze;

float angle0 = Pi / 4;
float angle1 = -Pi / 2, angle2 = -Pi / 2;
float angle3 = -Pi / 2, angle4 = -Pi / 2;
float angle5 = -Pi / 2, angle6 = 0;
float angle7 = Pi / 2, angle8 = 0;
float angle9 = 0;
float  eyeangle = 0, noseangle = 0;

glm::mat4 modelM = glm::mat4(1.0f);

glm::mat4 modelM_ = glm::mat4(1.0f);

vector<glm::mat4> mat_list;
stack<glm::mat4> mat_stack;
stack<glm::mat4> mat_stack_;
/////////////////////////////////////////////////////////////////////////////////////////////////////
float * getmazevertex() {
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
				MAZE[m - 3] = float(i) / 5-0.9;
				MAZE[m - 2] = float(j) / 5-0.9;
				MAZE[m - 1] = 0;
				cout << MAZE[m - 3] << "  " << MAZE[m - 2] << "  " << MAZE[m - 1] << endl;
				//printf("MAZE[m]");
			}
		}
	}
	return MAZE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void getcirclevertex() {
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;

	for (int i = 0; i < 25; i++) {
		x += 0.02 * cos(2 * Pi / 24 * i);
		y += 0.02 * sin(2 * Pi / 24 * i);
		z += 0;
		circlevertex[3 * i] = x;
		circlevertex[3 * i + 1] = y;
		circlevertex[3 * i + 2] = z;
	}
}


//////////////////////////////////////////////////////////////////////////////////
// create VBO objects and send the triangle vertices/colors to the graphics card
// 
void InitVBO() {
	glGenBuffers(1, vboHandle);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up 

	glGenBuffers(1, vboHandleCir);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCir[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 75, circlevertex, GL_STATIC_DRAW); // allocate space and copy the position data over
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up


	glGenBuffers(1, vboHandlemaze);   // create an interleaved VBO object
	glBindBuffer(GL_ARRAY_BUFFER, vboHandlemaze[0]);   // bind the first handle
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 162, MAZE, GL_STATIC_DRAW); // allocate space and copy the position data over
																			  //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 138, MAZE, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);   // clean up


	tindices[0] = 0;   tindices[1] = 1;   tindices[2] = 2;
	tindices[3] = 0;   tindices[4] = 2;   tindices[5] = 3;
	glGenBuffers(1, &indexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, tindices, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up 

	for (int i = 0; i < 25; i++) {
		cirseq[i] = i;
	}
	glGenBuffers(1, &indexVBOCir);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCir);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 25, cirseq, GL_STATIC_DRAW);  // load the index data 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up

	for (int i = 0; i < 54; i++) {
		mazenum[i] = i;
	}
	glGenBuffers(1, &indexVBOmaze);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOmaze);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 54, mazenum, GL_STATIC_DRAW);  // load the index data
																						   //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 46, mazenum, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  // clean up

}

//////////////////////////////////////////////////////////////////////////////////////////

void draw_square(const glm::mat4 &m, float color[3]) {

	glLoadMatrixf(&m[0][0]);
	//glLoadMatrixf((GLfloat*) m);
	glPointSize(10);
	glColor3f(color[0], color[1], color[2]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (char*)NULL + 0);
	//  glDrawElements(GL_POINTS, 6, GL_UNSIGNED_BYTE, (char*)NULL+0);
}

void draw_circle(const glm::mat4 &m, float color[3]) {
	glLoadMatrixf(&m[0][0]);
	glPointSize(10);
	glColor3f(color[0], color[1], color[2]);
	glDrawElements(GL_TRIANGLE_FAN, 25, GL_UNSIGNED_BYTE, (char*)NULL + 0);
}
void draw_maze(const glm::mat4 &m, float color[3]) {//const glm::mat4 &m,
	glLoadMatrixf(&m[0][0]);
	glPointSize(20);
	glColor3f(color[0], color[1], color[2]);
	glDrawElements(GL_POINTS, 54, GL_UNSIGNED_BYTE, (char*)NULL + 0);

}

void move(void)
{
	eyeangle += Pi / 3600;
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
/////////////////////////////////////////////////////////////
void display()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY); // enable the vertex array 
	 //////////////////draw maze//////////////////
	glBindBuffer(GL_ARRAY_BUFFER, vboHandlemaze[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOmaze);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	float color[3];
	color[0] = 1; color[1] = 0; color[2] = 0.8;
	mat_stack_.push(modelM_);
	draw_maze(modelM_, color);
	modelM_ = mat_stack_.top();
	mat_stack_.pop();



	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	// the following code tells OpenGL how to walk through the vertex array 
	glVertexPointer(3, GL_FLOAT, 0, 0); //number of coordinates per vertex (3 here), type of the coordinates, 
										// stride between consecutive vertices, and pointers to the first coordinate
	vector <glm::mat4>::const_iterator mi;
	////////////body///////////////////
	//float color[3];
	color[0] = 0.5; color[1] = 0.5; color[2] = 1;
	mat_stack.push(modelM);
	modelM = glm::rotate(modelM, angle0, glm::vec3(0.0f, 0.0f, 1.0f));
	mat_stack.push(modelM);
	modelM = glm::scale(modelM, glm::vec3(3.0f, 3.0f, 1.0f)*b);
	draw_square(modelM, color);
	modelM = mat_stack.top();
	mat_stack.pop();
	modelM = mat_stack.top();
	mat_stack.pop();
	mat_stack.push(modelM);


	//tooth//
	color[0] = 1; color[1] = 1; color[2] = 1;
	float tooth[] = { 0.015f,-0.08f,0.0f,-0.015f,-0.08f,0.0f };
	for (int i = 0; i < 6; i += 3) {
		modelM = glm::translate(modelM, glm::vec3(tooth[i], tooth[i + 1], tooth[i + 2])*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.2f, 0.3f, 0.0f)*b);
		draw_square(modelM, color);
		modelM = mat_stack.top();  mat_stack.pop();
		modelM = mat_stack.top();  mat_stack.pop();
		mat_stack.push(modelM);
	}


	////////////////ARM//////////////////
	//mat_stack.push(modelM);
	float translatearm[] = { .2f,0.05f,0.08f,0.05f,0.1f,-.2f,-0.05f,-0.08f,-0.05f,-0.1f };
	float anglearm[] = { angle1,angle2,angle3,angle4 };
	for (int i = 0, j = 0; i<10, j<4; i += 5, j += 2) {
		///////////lower//////////
		color[0] = 0; color[1] = 0.8; color[2] = 0.8;
		modelM = glm::translate(modelM, glm::vec3(translatearm[i], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, anglearm[j], glm::vec3(0.0f, 0.0f, 1.0f));
		modelM = glm::translate(modelM, glm::vec3(translatearm[i + 1], 0.0f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(1.3f, 0.3f, 1.0f)*b);
		draw_square(modelM, color);
		modelM = mat_stack.top();  mat_stack.pop();
		///////upper////////////
		color[0] = 0.6; color[1] = 1; color[2] = 0.2;
		modelM = glm::translate(modelM, glm::vec3(translatearm[i + 2], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, anglearm[j + 1], glm::vec3(0.0f, 0.0f, 1.0f));
		modelM = glm::translate(modelM, glm::vec3(translatearm[i + 3], 0.0f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(1.6f, .6f, 1.0f)*b);
		draw_square(modelM, color);
		modelM = mat_stack.top();  mat_stack.pop();
		///////hand///////////
		color[0] = 1; color[1] = 0.8; color[2] = 0.8;
		modelM = glm::translate(modelM, glm::vec3(translatearm[i + 4], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, angle9, glm::vec3(0.0f, 0.0f, 1.0f));
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.5f, 0.73f, 1.0f)*b);
		draw_square(modelM, color);
		modelM = mat_stack.top();
		mat_stack.pop();
		modelM = mat_stack.top();
		mat_stack.pop();
		mat_stack.push(modelM);
	}

	////////// leg/////////
	float translateleg[] = { 0.07f,0.05f,0.1f,0.05f ,0.1f, -0.07f,-0.05f,-0.1f,-0.05f ,-0.1f };
	float angleleg[] = { angle5,angle6,angle7,angle8 };
	for (int i = 0, j = 0; i < 10, j<4; i += 5, j += 2) {
		//////////////lower///////////////
		color[0] = 0; color[1] = 0.8; color[2] = 0.8;
		modelM = glm::translate(modelM, glm::vec3(translateleg[i], -0.15f, 0.0f)*b);
		modelM = glm::rotate(modelM, angleleg[j], glm::vec3(0.0f, 0.0f, 1.0f));
		modelM = glm::translate(modelM, glm::vec3(translateleg[i + 1], 0.0f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(1.3f, 0.3f, 1.0f)*b);
		draw_square(modelM, color);
		modelM = mat_stack.top();  mat_stack.pop();
		/////////////upper////////////////
		color[0] = 0.6; color[1] = 1; color[2] = 0.2;
		modelM = glm::translate(modelM, glm::vec3(translateleg[i + 2], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, angleleg[j + 1], glm::vec3(0.0f, 0.0f, 1.0f));
		modelM = glm::translate(modelM, glm::vec3(translateleg[i + 3], 0.0f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(2.0f, .6f, 1.0f)*b);
		draw_square(modelM, color);
		modelM = mat_stack.top();  mat_stack.pop();
		/////////////foot///////////////
		color[0] = 1; color[1] = 0.8; color[2] = 0.8;
		modelM = glm::translate(modelM, glm::vec3(translateleg[i + 4], 0.0f, 0.0f)*b);
		modelM = glm::rotate(modelM, angle9, glm::vec3(0.0f, 0.0f, 1.0f));
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.5f, 0.73f, 1.0f)*b);
		draw_square(modelM, color);
		modelM = mat_stack.top();
		mat_stack.pop();
		modelM = mat_stack.top();
		mat_stack.pop();
		mat_stack.push(modelM);
	}


	///////////////////draw circle eye/////////////////////////////
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCir[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOCir);


	// the following code tells OpenGL how to walk through the vertex array 
	glVertexPointer(3, GL_FLOAT, 0, 0); //number of coordinates per vertex (3 here), type of the coordinates, 
										// stride between consecutive vertices, and pointers to the first coordinate


	color[0] = 1; color[1] = 1; color[2] = 0.2;
	mat_stack_.push(modelM_);
	modelM_ = glm::translate(modelM_, glm::vec3(0.9f, 0.4f, 0.0f));
	draw_circle(modelM_, color);
	modelM_ = mat_stack_.top();
	mat_stack_.pop();
    ///////////eyes//////////
	color[0] = 1; color[1] = 1; color[2] = 1;
	float translateeye[] = { 0.06f, -0.06f };
	for (int i = 0; i <2; i++) {
		//right eye
		modelM = glm::translate(modelM, glm::vec3(translateeye[i], 0.03f, 0.0f)*b);
		mat_stack.push(modelM);
		modelM = glm::scale(modelM, glm::vec3(0.5f, 0.5f, 0.0f)*b);
		draw_circle(modelM, color);
		modelM = mat_stack.top();
		mat_stack.pop();
		modelM = mat_stack.top();
		mat_stack.pop();
		mat_stack.push(modelM);
	}

	color[0] = 0; color[1] = 0; color[2] = 0;
	float translateeyepupil[] = { 0.06f, -0.06f };
	for (int i = 0; i < 2; i++) {
	// eye pupil
	modelM = glm::translate(modelM, glm::vec3(translateeyepupil[i], 0.07f, 0.0f)*b);
	modelM = glm::rotate(modelM, eyeangle, glm::vec3(0.0f, 0.0f, 1.0f));
	mat_stack.push(modelM);
	modelM = glm::scale(modelM, glm::vec3(0.25f, 0.25f, 0.0f)*b);
	draw_circle(modelM, color);
	modelM = mat_stack.top();
	mat_stack.pop();
	modelM = mat_stack.top();
	mat_stack.pop();
	mat_stack.push(modelM);
	}


	//nose
	color[0] = 1; color[1] = 0; color[2] = 0;
	modelM = glm::translate(modelM, glm::vec3(0.0f, -0.02f, 0.0f)*b);
	modelM = glm::rotate(modelM, noseangle, glm::vec3(0.0f, 0.0f, 1.0f));
	mat_stack.push(modelM);
	modelM = glm::scale(modelM, glm::vec3(0.25f, 0.25f, 0.0f)*b);
	draw_circle(modelM, color);
	modelM = mat_stack.top();
	mat_stack.pop();
	modelM = mat_stack.top();
	mat_stack.pop();
	mat_stack.push(modelM);


	///////////////////////////////////////////////////////////
	color[0] = 0; color[1] = 1; color[2] = 0;

	printf(" ****\n");
	for (mi = mat_list.begin(); mi != mat_list.end(); mi++) {
		printf(" hello!\n");
		//draw_square((*mi), color);
	}

	glDisableClientState(GL_VERTEX_ARRAY);

	glutSwapBuffers();

}
void mymouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
		printf(" click on [%d, %d]!\n", x, y);
	else if (state == GLUT_UP)
		printf(" mouse button up! \n");

	glutPostRedisplay();
}
///////////////////////////////////////////////////////////////

void mykey(unsigned char key, int x, int y)
{
	float d_angle = 0;
	if (key == 'q') exit(1);
	if (key == 't') {
		d_angle += Pi / 180 * 15;
		modelM = glm::rotate(modelM, d_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (key == 'T') {
		d_angle += -Pi / 180 * 15;
		modelM = glm::rotate(modelM, d_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (key == 'r')
		modelM = glm::translate(modelM, glm::vec3(0.1f, 0.0f, 0.0f)*b);
	if (key == 'l')
		modelM = glm::translate(modelM, glm::vec3(-0.1f, 0.0f, 0.0f)*b);
	if (key == 'f')
		modelM = glm::translate(modelM, glm::vec3(0.0f, 0.1f, 0.0f)*b);
	if (key == 'b')
		modelM = glm::translate(modelM, glm::vec3(0.0f, -0.1f, 0.0f)*b);
	//modelM =  translate(modelM, 0,-.1,0);
	if (key == 'e')
		eyeangle += Pi / 180 * 15;
	if (key == 'm')
		glutIdleFunc(move);
	if (key == 's')
		glutIdleFunc(NULL);
	if (key == '+')
		b += 0.1;
	if (key == '-')
		b -= 0.1;

	if (key == 'n') {
	 noseangle += -Pi / 180 * 20;
		if (noseangle<-Pi / 4 || noseangle>Pi / 4)
			noseangle = Pi / 4;
	}
	if (key == 'c') {
		glutIdleFunc(NULL);
		modelM = glm::mat4(1.0f);
		modelM = glm::translate(modelM, glm::vec3(-0.71, -0.7, 0.0));
		angle1 = -Pi / 2, angle2 = -Pi / 2;
		angle3 = -Pi / 2, angle4 = -Pi / 2;
		angle5 = -Pi / 2, angle6 = 0;
		angle7 = Pi / 2, angle8 = 0;
		angle9 = 0;
		eyeangle = 0;
		noseangle = 0;
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
	glutKeyboardFunc(mykey);

	mat_list.clear();

#ifdef __APPLE__
#else
	glewInit();
#endif
	/////////////////////////////////////////////////////////
	getmazevertex();
	///////////////////////////////////////////////////////
	getcirclevertex();

	///////////////////////////////////////////////////////////
	InitVBO();
	glutMainLoop();
	delete[] MAZE;
}
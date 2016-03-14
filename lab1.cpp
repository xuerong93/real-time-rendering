/*
This lab is to read file "iris.csv" and use the data to plot in 16 different subplots
what I have done: 
1.Display all the sixteen plots in the scatterplot matrix. Add a square box around each scatterplot
2.Color the points in the scatterplots based on their species: Red for Setosa, Green for Versicolor, and Blue for Virginica
3.Display 10 equally spaced tick marks in X and Y sides of each box
4.Allow the user to resize the window and your scatterplots should resize accordingly
5.Draw a circle for each point instead of a point
6.Show text along the X and Y axes to indicate the variables used for the plots */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>
//#include <windows.h>
#include <math.h>
using namespace std;
//#define MAX_CHAR   128
#define Pi 3.1415926536
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

int width, height;
std::vector<float> v1, v2, v3, v4;
std::vector<int> v0;

float max_v1 = -10000, min_v1 = 10000;
float max_v2 = -10000, min_v2 = 10000;
float max_v3 = -10000, min_v3 = 10000;
float max_v4 = -10000, min_v4 = 10000;

float scale_v1, scale_v2, scale_v3, scale_v4;

////////////////////////////////////////////
/*void drawString(const char* str) {    // to write in scatterplots
static int isFirstCall = 1;
static GLuint lists;
if (isFirstCall) {
isFirstCall = 0;
lists = glGenLists(MAX_CHAR);
wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
}
for (; *str != '\0'; ++str)
glCallList(lists + *str);
}*/
void drawString(float x, float y, char* text) {
	glRasterPos2f(x, y);
	for (; *text != '\0'; text++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *text);
	}
}


void plotcircle(float x, float y, float radius) {        //to draw circle instead of pixel
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	int n = 10;
	for (int i = 0; i <= n; i++) {
		glVertex2f(x + radius*cos(2 * Pi / n*i), y + radius*sin(2 * Pi / n*i));
	}
	glEnd();
}



void display()
{
	int size = v1.size();
	printf("size = %d\n", size);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0, 0, 0);     //glColor* have been deprecated in OpenGL 3


	float width_sub = glutGet(GLUT_WINDOW_WIDTH) / 4;
	float height_sub = glutGet(GLUT_WINDOW_HEIGHT) / 4;
	//int x = width_sub / 60;
	//glPointSize(x);
	float *a;
	a = new float[size * 4];
	for (int i = 0; i < size; i++) {                           //array to store points in NDC
		a[i] = (v1[i] - min_v1) * 1.8 / scale_v1 - 0.9;
		a[i + size] = (v2[i] - min_v2) * 1.8 / scale_v2 - 0.9;
		a[i + size * 2] = (v3[i] - min_v3) * 1.8 / scale_v3 - 0.9;
		a[i + size * 3] = (v4[i] - min_v4) * 1.8 / scale_v4 - 0.9;
	}

	for (int j = 0; j < 16; j++) {
		float Imin = (j / 4)*width_sub;
		float Jmin = (3 - j % 4)*height_sub;
		glViewport(Imin, Jmin, width_sub, height_sub);//location of viewport

		glBegin(GL_LINE_LOOP);               //draw scatter plot box
		glColor3f(0, 0, 0);                 //the box is black
		glVertex2f(-0.9, -0.9);
		glVertex2f(-0.9, 0.91);
		glVertex2f(0.91, 0.91);
		glVertex2f(0.91, -0.9);
		glEnd();

		for (int i = 0; i < 10; i++) {    // tick marks in X and Y
			glBegin(GL_LINES);
			glVertex2f(-0.9 + 0.18*i, -0.9);
			glVertex2f(-0.9 + 0.18*i, -0.95);
			glEnd();
			char b[10];
			drawString(-0.88 + 0.18*i, -0.999, _itoa(i, b, 10));
			glBegin(GL_LINES);
			glVertex2f(-0.9, -0.9 + 0.18*i);
			glVertex2f(-0.95, -0.9 + 0.18*i);
			glEnd();
			drawString(-0.99, -0.88 + 0.18*i, _itoa(i, b, 10));
		}


		if (j / 4 == j % 4 && j / 4 == 0) {                //WRITE TEXT in scatterplot
		    //glRasterPos2f(-0.5, 0.0f);
			//drawString("sepal length");
			drawString(-0.35f, 0.0f, "sepal length");
			glutSwapBuffers();
			continue;
		}
		else if (j / 4 == j % 4 && j / 4 == 1) {
			//glRasterPos2f(-0.5, 0.0f);
			//drawString("sepal width");
			drawString(-0.35f, 0.0f, "sepal width");
			glutSwapBuffers();
			continue;
		}
		else if (j / 4 == j % 4 && j / 4 == 2) {
			//glRasterPos2f(-0.5, 0.0f);
			//drawString("petal length");
			drawString(-0.35f, 0.0f, "petal length");
			glutSwapBuffers();
			continue;
		}
		else if (j / 4 == j % 4 && j / 4 == 3) {
			//glRasterPos2f(-0.5, 0.0f);
			//drawString("petal width");
			drawString(-0.35f, 0.0f, "petal width");
			glutSwapBuffers();
			continue;
		}



		//glEnable(GL_POLYGON_SMOOTH);
		//glBegin(GL_POINTS);//draw points
		for (int i = 0; i < size; i++) {
			if (v0[i] == 1) {
				glColor3f(1, 0, 0);
			}
			else if (v0[i] == 2) {
				glColor3f(0, 1, 0);
			}
			else if (v0[i] == 3) {
				glColor3f(0, 0, 1);
			}
			plotcircle(a[i + j / 4 * size], a[i + j % 4 * size], (width_sub / 7000));
		}
		//glEnd();
	}
	delete[] a;
	glFlush();
}


int main(int argc, char** argv) {

	char name[40];
	float f1, f2, f3, f4;
	//	float *d1, *d2, *d3, *d4;

	if (argc != 4) {
		printf(" usage: %s width height csv_file\n", argv[0]);//width,height are in pixels
		exit(1);
	}

	width = atoi(argv[1]);
	height = atoi(argv[2]);

	printf(" width %d  height %d  file %s \n", width, height, argv[3]);

	FILE* fp = fopen(argv[3], "r");
	if (fp == NULL) {
		printf("Error happened");
		exit(1);
	}

	int nameNum = 0;
	int cnt = 0;
	char n1[2048];
	string namenow = "";
	if (fscanf(fp, "%[^\n]\n", n1) == 1) cout << n1 << endl;     //read the first line
	while (fscanf(fp, "%[^,],%f,%f,%f,%f \n", name, &f1, &f2, &f3, &f4) == 5) {     //read the description and data
		if (namenow != name) {
			nameNum++;
			cout << nameNum << "  " << name << endl;
		}

		namenow = name;

		if (f1 < min_v1) min_v1 = f1;
		if (f1 > max_v1) max_v1 = f1;

		if (f2 < min_v2) min_v2 = f2;
		if (f2 > max_v2) max_v2 = f2;

		if (f3 < min_v3) min_v3 = f3;
		if (f3 > max_v3) max_v3 = f3;

		if (f4 < min_v4) min_v4 = f4;
		if (f4 > max_v4) max_v4 = f4;

		v0.push_back(nameNum);
		v1.push_back(f1);
		v2.push_back(f2);
		v3.push_back(f3);
		v4.push_back(f4);
		cnt++;
	}
	scale_v1 = max_v1 - min_v1;
	scale_v2 = max_v2 - min_v2;
	scale_v3 = max_v3 - min_v3;
	scale_v4 = max_v4 - min_v4;

	// init glut drawing window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(width, height);
	glutCreateWindow("Xuerong Hu's lab1");


	glutDisplayFunc(display);

	/*
	glutReshapeFunc(resize);
	glutMouseFunc(mymouse);
	glutKeyboardFunc(mykey);

	// create menu
	glutCreateMenu(mymenu);
	glutAddMenuEntry("Point", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	*/

	// enter the event loop
	glutMainLoop();

}

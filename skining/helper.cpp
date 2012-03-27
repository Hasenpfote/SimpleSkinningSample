#include <string.h>
#include <GL/glew.h>
//#include <GL/glut.h>
#include <GL/freeglut.h>
#include "helper.h"

void Helper::renderString(int x, int y, const char* string){
	const size_t len = strlen(string);
	glColor3f(1.0, 1.0, 1.0);
	glWindowPos2f(x, y);
	for(size_t i = 0; i < len; i++){
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string);
	  string++;
	}
}

void Helper::renderCircle(int x,int y, float radius){
	for(float th1 = 0.0;  th1 <= 360.0;  th1 = th1 + 1.0){
		float th2 = th1 + 10.0;
		float th1_rad = th1 / 180.0f * PI; 
		float th2_rad = th2 / 180.0f * PI;

		float x1 = radius * cos(th1_rad);
		float y1 = radius * sin(th1_rad);
		float x2 = radius * cos(th2_rad);
		float y2 = radius * sin(th2_rad);

		glBegin(GL_LINES);   
			glVertex2f(x1+x, y1+y);
			glVertex2f(x2+x, y2+y);
		glEnd();
	}
}

void Helper::renderCoord(const Matrix44* m, float length){
	glPushMatrix();
		glMultMatrixf(*m);
		glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 1.0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glColor3f(1.0f, 0.0f, 0.0);
			glVertex3f(length, 0.0f, 0.0f);
			glColor3f(1.0f, 1.0f, 1.0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0);
			glVertex3f(0.0f, length, 0.0f);
			glColor3f(1.0f, 1.0f, 1.0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glColor3f(0.0f, 0.0f, 1.0);
			glVertex3f(0.0f, 0.0f, length);
		glEnd();
	glPopMatrix();
}

void Helper::renderJoint(const Matrix44* m, float length){
	glPushMatrix();
		glMultMatrixf(*m);
		glColor3f(1.0f, 1.0f, 1.0);
		renderCircle(0, 0, length*0.5f);
		glPushMatrix();
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			renderCircle(0, 0, length*0.5f);
		glPopMatrix();
		glPushMatrix();
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			renderCircle(0, 0, length*0.5f);
		glPopMatrix();
		glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 1.0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glColor3f(1.0f, 0.0f, 0.0);
			glVertex3f(length, 0.0f, 0.0f);
			glColor3f(1.0f, 1.0f, 1.0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0);
			glVertex3f(0.0f, length, 0.0f);
			glColor3f(1.0f, 1.0f, 1.0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glColor3f(0.0f, 0.0f, 1.0);
			glVertex3f(0.0f, 0.0f, length);
		glEnd();
	glPopMatrix();
}

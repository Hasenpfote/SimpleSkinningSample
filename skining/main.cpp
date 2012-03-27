#include <iostream>
#include <GL/glew.h>
//#include <GL/glut.h>
#include <GL/freeglut.h>
#include "glsl.h"
#include "crc32.h"
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"
#include "skelton.h"
#include "helper.h"

using namespace mathematics;

// シェーダ0の各要素の位置
GLint loc0_skinningMatrices_0;
GLint loc0_skinningMatrices_1;
GLint loc0_weights;
GLint loc0_matrixIndices;
GLint loc0_numJoints;

// シェーダ1の各要素の位置
GLint loc1_jointRot_0;
GLint loc1_jointRot_1;
GLint loc1_jointPos_0;
GLint loc1_jointPos_1;
GLint loc1_weights;
GLint loc1_matrixIndices;
GLint loc1_numJoints;

const int MAX_JOINTS = 2;
struct Vertex{
    float r, g, b, a;
    float nx, ny, nz;
    float x, y, z;
	float weights[MAX_JOINTS];
	float matrixIndices[MAX_JOINTS];
	float numJoints;
};
// Quad geometry
Vertex g_quadVertices[] =
{
	// r    g    b    a      nx   ny  nz     x    y    z      w0   w1    mi0   mi1    nj
	{ 1.0f,0.0f,0.0f,1.0f,  0.0f,0.0f,1.0, -1.0f,0.0f,0.0f,  1.0f,0.0f,  0.0f,0.0f,  1.0f }, // Quad 0
	{ 1.0f,0.0f,0.0f,1.0f,  0.0f,0.0f,1.0,  1.0f,0.0f,0.0f,  1.0f,0.0f,  0.0f,0.0f,  1.0f },
	{ 1.0f,0.0f,0.0f,1.0f,  0.0f,0.0f,1.0,  1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 1.0f,0.0f,0.0f,1.0f,  0.0f,0.0f,1.0, -1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },

	{ 0.0f,1.0f,0.0f,1.0f,  0.0f,0.0f,1.0, -1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f }, // Quad 1
	{ 0.0f,1.0f,0.0f,1.0f,  0.0f,0.0f,1.0,  1.0f,2.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 0.0f,1.0f,0.0f,1.0f,  0.0f,0.0f,1.0,  1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 0.0f,1.0f,0.0f,1.0f,  0.0f,0.0f,1.0, -1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },

	{ 0.0f,0.0f,1.0f,1.0f,  0.0f,0.0f,1.0, -1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f }, // Quad 2
	{ 0.0f,0.0f,1.0f,1.0f,  0.0f,0.0f,1.0,  1.0f,4.0f,0.0f,  0.5f,0.5f,  0.0f,1.0f,  2.0f },
	{ 0.0f,0.0f,1.0f,1.0f,  0.0f,0.0f,1.0,  1.0f,6.0f,0.0f,  1.0f,0.0f,  1.0f,0.0f,  1.0f },
	{ 0.0f,0.0f,1.0f,1.0f,  0.0f,0.0f,1.0, -1.0f,6.0f,0.0f,  1.0f,0.0f,  1.0f,0.0f,  1.0f }
};

GLuint g_boneDisplayList = -1;

// GLSLテスト
#define USE_SHADER
#ifdef USE_SHADER
static Glsl glsl0;
static Glsl glsl1;
#endif

// カメラ
static float fov = 45.0f;
static float cam_pos_z = 20.0f;

// 関節
typedef struct tagJoint{
	tagJoint* child;
	tagJoint* sibling;
	Matrix44 bind_pose;
	Matrix44 inv_bind_pose;
	JointPose local_pose;
	Matrix44 global_pose;
	Matrix44 skinning;
}Joint;

static Joint joint[2];

// スケルトンの更新
static void updateSekelton(Joint* joint, Joint* parent){
	// global_poseの更新
	joint->local_pose.getMatrix(&joint->global_pose);
	Matrix44Mul(&joint->global_pose, &joint->bind_pose, &joint->global_pose);
	if(parent){
		Matrix44Mul(&joint->global_pose, &parent->global_pose, &joint->global_pose);
	}
	// skinning
	Matrix44Mul(&joint->skinning, &joint->global_pose, &joint->inv_bind_pose);

	if(joint->child)
		updateSekelton(joint->child, joint);
	if(joint->sibling)
		updateSekelton(joint->sibling, parent);
}

// スケルトンの描画
static void renderSkelton(Joint* joint){
	// 関節の描画
	Helper::renderJoint(&joint->global_pose, 0.5f);
	if(joint->child){
		// 骨の描画
		glPushMatrix();
			glMultMatrixf(joint->global_pose);
			glColor3f(1.0f, 1.0f, 1.0);
			glCallList(g_boneDisplayList);
		glPopMatrix();
		renderSkelton(joint->child);
	}
	if(joint->sibling)
		renderSkelton(joint->sibling);
}

// 関節
typedef struct tagJoint2{
	tagJoint2* child;
	tagJoint2* sibling;
	JointPose bind_pose;
	JointPose inv_bind_pose;
	JointPose local_pose;
	JointPose global_pose;
	JointPose skinning;
}Joint2;

static Joint2 joint2[2];

static void updateSekelton2(Joint2* joint, Joint2* parent){
	joint->global_pose.transform(joint->local_pose, joint->bind_pose);
	if(parent){
		joint->global_pose.transform(parent->global_pose);
	}
	joint->skinning.transform(joint->inv_bind_pose, joint->global_pose);

	if(joint->child)
		updateSekelton2(joint->child, joint);
	if(joint->sibling)
		updateSekelton2(joint->sibling, parent);
}

static void renderSkelton2(Joint2* joint){
	Matrix44 global_pose;
	joint->global_pose.getMatrix(&global_pose);
	// 関節の描画
	Helper::renderJoint(&global_pose, 0.5f);
	if(joint->child){
		// 骨の描画
		glPushMatrix();
			glMultMatrixf(global_pose);
			glColor3f(1.0f, 1.0f, 1.0);
			glCallList(g_boneDisplayList);
		glPopMatrix();
		renderSkelton2(joint->child);
	}
	if(joint->sibling)
		renderSkelton2(joint->sibling);
}

/**
 * 初期化
 */
static bool init(void){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
//	glFrontFace(GL_CW);
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glCullFace(GL_FRONT);
	glDisable(GL_LIGHTING);
//	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glActiveTexture(GL_TEXTURE0);

#ifdef USE_SHADER
	if(!glsl0.create("shader/skining.vert", "shader/simple.frag"))
		return false;
	if(!glsl1.create("shader/skining2.vert", "shader/simple.frag"))
		return false;
#endif
	loc0_skinningMatrices_0 = glGetUniformLocation(glsl0.getProgram(), "skinningMatrices[0]");
	loc0_skinningMatrices_1 = glGetUniformLocation(glsl0.getProgram(), "skinningMatrices[1]");
    loc0_weights            = glGetAttribLocation(glsl0.getProgram(), "weights");
    loc0_matrixIndices      = glGetAttribLocation(glsl0.getProgram(), "matrixIndices");
    loc0_numJoints          = glGetAttribLocation(glsl0.getProgram(), "numJoints");

	loc1_jointRot_0    = glGetUniformLocation(glsl1.getProgram(), "jointRot[0]");
	loc1_jointRot_1    = glGetUniformLocation(glsl1.getProgram(), "jointRot[1]");
	loc1_jointPos_0    = glGetUniformLocation(glsl1.getProgram(), "jointPos[0]");
	loc1_jointPos_1    = glGetUniformLocation(glsl1.getProgram(), "jointPos[1]");
	loc1_weights       = glGetAttribLocation(glsl1.getProgram(), "weights");
    loc1_matrixIndices = glGetAttribLocation(glsl1.getProgram(), "matrixIndices");
    loc1_numJoints     = glGetAttribLocation(glsl1.getProgram(), "numJoints");

	g_boneDisplayList = glGenLists(1);
	glNewList( g_boneDisplayList, GL_COMPILE );
	{
		glBegin( GL_LINE_STRIP );
		{
			glVertex3f( 0.0f, 0.0f, 0.0f);	
			glVertex3f(-0.2f, 0.2f,-0.2f);
			glVertex3f( 0.2f, 0.2f,-0.2f);
			glVertex3f( 0.0f, 3.0f, 0.0f); // Bone length = 3.0f
			glVertex3f(-0.2f, 0.2f,-0.2f);
			glVertex3f(-0.2f, 0.2f, 0.2f);
			glVertex3f( 0.0f, 0.0f, 0.0f);
			glVertex3f( 0.2f, 0.2f,-0.2f);
			glVertex3f( 0.2f, 0.2f, 0.2f);
			glVertex3f( 0.0f, 0.0f, 0.0f);
			glVertex3f(-0.2f, 0.2f, 0.2f);
			glVertex3f( 0.0f, 3.0f, 0.0f); // Bone length = 3.0f
			glVertex3f( 0.2f, 0.2f, 0.2f);
			glVertex3f(-0.2f, 0.2f, 0.2f);
		}
		glEnd();
	}
	glEndList();

	// スケルトンの構築(行列バージョン)
	joint[1].child = NULL;
	joint[1].sibling = NULL;
	Matrix44Translation(&joint[1].bind_pose, 0.0f, 3.0f, 0.0f);
	Matrix44Inverse(&joint[1].inv_bind_pose, &joint[1].bind_pose);
	joint[1].local_pose.translation.set(0.0f, 0.0f, 0.0f); 
	joint[1].local_pose.rotation.set(1.0f, 0.0f, 0.0f, 0.0f); 
	joint[1].local_pose.scale = 1.0f;

	joint[0].child = &joint[1];
	joint[0].sibling = NULL;
	Matrix44Identity(&joint[0].bind_pose);
	Matrix44Inverse(&joint[0].inv_bind_pose, &joint[0].bind_pose);
	joint[0].local_pose.translation.set(0.0f, 0.0f, 0.0f); 
	joint[0].local_pose.rotation.set(1.0f, 0.0f, 0.0f, 0.0f); 
	joint[0].local_pose.scale = 1.0f;

	// スケルトンの構築(SQTバージョン)
	joint2[1].child = NULL;
	joint2[1].sibling = NULL;
	QuaternionRotation(&joint2[1].bind_pose.rotation, &Vector3(0.0f, 1.0f, 0.0f), 0.0f);
	joint2[1].bind_pose.translation.set(0.0f, 3.0f, 0.0f);
	joint2[1].inv_bind_pose.inverse(joint2[1].bind_pose);
	QuaternionRotation(&joint2[1].local_pose.rotation, &Vector3(0.0f, 1.0f, 0.0f), 0.0f);
	joint2[1].local_pose.translation.set(0.0f, 0.0f, 0.0f);

	joint2[0].child = &joint2[1];
	joint2[0].sibling = NULL;
	QuaternionRotation(&joint2[0].bind_pose.rotation, &Vector3(0.0f, 1.0f, 0.0f), 0.0f);
	joint2[0].bind_pose.translation.set(0.0f, 0.0f, 0.0f);
	joint2[0].inv_bind_pose.inverse(joint2[0].bind_pose);
	QuaternionRotation(&joint2[0].local_pose.rotation, &Vector3(0.0f, 0.0f, 1.0f), 0.0f);
	joint2[0].local_pose.translation.set(0.0f, 0.0f, 0.0f);

	return true;
}

/**
 * 解放
 */
void release(){
	glDeleteLists(g_boneDisplayList, 1);
}

// ベーシックなスキニング
static void render_skin_mesh0(){
	// スケルトンの更新
	updateSekelton(&joint[0], NULL);

	// skinninged meshの描画
	glUseProgram(glsl0.getProgram());
	glUniformMatrix4fv(loc0_skinningMatrices_0, 1, false, joint[0].skinning);
	glUniformMatrix4fv(loc0_skinningMatrices_1, 1, false, joint[1].skinning);

	int nIndex;
	int nIndexOffset;
	float fWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	float fNumJoints[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	float fMatrixIndices[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	// For each quad...
	for(int j = 0; j < 3; j++){
		nIndexOffset = j * 4;
		glBegin(GL_QUADS);
		{
			// Render the quad's four vertices...
			for(int i = 0; i < 4; i++){
				nIndex = i + nIndexOffset;
				// 重み
				fWeights[0] = g_quadVertices[nIndex].weights[0];
				fWeights[1] = g_quadVertices[nIndex].weights[1];
                glVertexAttrib4fv(loc0_weights, fWeights);
				// スキニング行列へのインデクス
				fMatrixIndices[0] = g_quadVertices[nIndex].matrixIndices[0];
				fMatrixIndices[1] = g_quadVertices[nIndex].matrixIndices[1];
                glVertexAttrib4fv(loc0_matrixIndices, fMatrixIndices);
				// 関節の数
				fNumJoints[0] = g_quadVertices[nIndex].numJoints; 
                glVertexAttrib4fv(loc0_numJoints, fNumJoints);
				glColor4f(g_quadVertices[nIndex].r, g_quadVertices[nIndex].g, g_quadVertices[nIndex].b, g_quadVertices[nIndex].a);
//				glNormal3f(g_quadVertices[nIndex].nx, g_quadVertices[nIndex].ny, g_quadVertices[nIndex].nz);
				glVertex3f(g_quadVertices[nIndex].x, g_quadVertices[nIndex].y, g_quadVertices[nIndex].z);
			}
		}
		glEnd();
	}
	glUseProgram(0);

	// スケルトンの描画
	renderSkelton(&joint[0]);
}

// 過程がSQT
static void render_skin_mesh1(){
	// skinninged meshの描画
	Matrix44 matSkinning0, matSkinning1;
	joint2[0].skinning.getMatrix(&matSkinning0);
	joint2[1].skinning.getMatrix(&matSkinning1);

	glUseProgram(glsl0.getProgram());
	glUniformMatrix4fv(loc0_skinningMatrices_0, 1, false, matSkinning0);
	glUniformMatrix4fv(loc0_skinningMatrices_1, 1, false, matSkinning1);

	int nIndex;
	int nIndexOffset;
	float fWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	float fNumJoints[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	float fMatrixIndices[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	// For each quad...
	for(int j = 0; j < 3; j++){
		nIndexOffset = j * 4;
		glBegin(GL_QUADS);
		{
			// Render the quad's four vertices...
			for(int i = 0; i < 4; i++){
				nIndex = i + nIndexOffset;
				// 重み
				fWeights[0] = g_quadVertices[nIndex].weights[0];
				fWeights[1] = g_quadVertices[nIndex].weights[1];
                glVertexAttrib4fv(loc0_weights, fWeights);
				// スキニング行列へのインデクス
				fMatrixIndices[0] = g_quadVertices[nIndex].matrixIndices[0];
				fMatrixIndices[1] = g_quadVertices[nIndex].matrixIndices[1];
                glVertexAttrib4fv(loc0_matrixIndices, fMatrixIndices);
				// 関節の数
				fNumJoints[0] = g_quadVertices[nIndex].numJoints; 
                glVertexAttrib4fv(loc0_numJoints, fNumJoints);
				glColor4f(g_quadVertices[nIndex].r, g_quadVertices[nIndex].g, g_quadVertices[nIndex].b, g_quadVertices[nIndex].a);
//				glNormal3f(g_quadVertices[nIndex].nx, g_quadVertices[nIndex].ny, g_quadVertices[nIndex].nz);
				glVertex3f(g_quadVertices[nIndex].x, g_quadVertices[nIndex].y, g_quadVertices[nIndex].z);
			}
		}
		glEnd();
	}
	glUseProgram(0);

	// スケルトンの描画
	renderSkelton2(&joint2[0]);
}

// SQT
static void render_skin_mesh2(){
	// skinninged meshの描画
	glUseProgram(glsl1.getProgram());

	float rot[4];
	float pos[4];

	rot[0] = joint2[0].skinning.rotation.x;
	rot[1] = joint2[0].skinning.rotation.y;
	rot[2] = joint2[0].skinning.rotation.z;
	rot[3] = joint2[0].skinning.rotation.w;
	pos[0] = joint2[0].skinning.translation.x;
	pos[1] = joint2[0].skinning.translation.y;
	pos[2] = joint2[0].skinning.translation.z;
	pos[3] = 1.0f;

	glUniform4fv(loc1_jointRot_0, 1, rot);
	glUniform4fv(loc1_jointPos_0, 1, pos);

	rot[0] = joint2[1].skinning.rotation.x;
	rot[1] = joint2[1].skinning.rotation.y;
	rot[2] = joint2[1].skinning.rotation.z;
	rot[3] = joint2[1].skinning.rotation.w;
	pos[0] = joint2[1].skinning.translation.x;
	pos[1] = joint2[1].skinning.translation.y;
	pos[2] = joint2[1].skinning.translation.z;
	pos[3] = 1.0f;

	glUniform4fv(loc1_jointRot_1, 1, rot);
	glUniform4fv(loc1_jointPos_1, 1, pos);

	int nIndex;
	int nIndexOffset;
	float fWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	float fNumJoints[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	float fMatrixIndices[4] = { 0.0f, 0.0f, 0.0f, 0.0f};
	// For each quad...
	for(int j = 0; j < 3; j++){
		nIndexOffset = j * 4;
		glBegin(GL_QUADS);
		{
			// Render the quad's four vertices...
			for(int i = 0; i < 4; i++){
				nIndex = i + nIndexOffset;
				// 重み
				fWeights[0] = g_quadVertices[nIndex].weights[0];
				fWeights[1] = g_quadVertices[nIndex].weights[1];
                glVertexAttrib4fv(loc1_weights, fWeights);
				// スキニング行列へのインデクス
				fMatrixIndices[0] = g_quadVertices[nIndex].matrixIndices[0];
				fMatrixIndices[1] = g_quadVertices[nIndex].matrixIndices[1];
                glVertexAttrib4fv(loc1_matrixIndices, fMatrixIndices);
				// 関節の数
				fNumJoints[0] = g_quadVertices[nIndex].numJoints; 
                glVertexAttrib4fv(loc1_numJoints, fNumJoints);
				glColor4f(g_quadVertices[nIndex].r, g_quadVertices[nIndex].g, g_quadVertices[nIndex].b, g_quadVertices[nIndex].a);
//				glNormal3f(g_quadVertices[nIndex].nx, g_quadVertices[nIndex].ny, g_quadVertices[nIndex].nz);
				glVertex3f(g_quadVertices[nIndex].x, g_quadVertices[nIndex].y, g_quadVertices[nIndex].z);
			}
		}
		glEnd();
	}
	glUseProgram(0);

	// スケルトンの描画
	renderSkelton2(&joint2[0]);
}
#if 0
static void render_quat(){
	static float w0 = 0.0f;
	const float w1 = 1.0f - w0;

	Quaternion q0, q1, ql, q2, q3;
	QuaternionRotation(&q0, &Vector3(0.0f, 0.0f, 1.0f),  0.0f);
	QuaternionRotation(&q1, &Vector3(0.0f, 0.0f, 1.0f),370.0f);
	QuaternionLerp(&ql, &q0, &q1, 0.5f);
//	QuaternionSlerp(&ql, &q0, &q1, w0);

	w0 += 0.01f;
	if(w0 > 1.0){
		w0 = 0.0f;
	}

	// 円
	glColor3f(1.0f, 1.0f, 1.0);
	render_circle(1.0f, 0.0f, 0.0f);

	// 各クォータニオン	
	Matrix44 mat;
	Matrix44From(&mat, &q0);
	glPushMatrix();
		glMultMatrixf(mat);
		glColor3f(1.0f, 0.0f, 0.0);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glEnd();
	glPopMatrix();
	Matrix44From(&mat, &q1);
	glPushMatrix();
		glMultMatrixf(mat);
		glColor3f(0.0f, 1.0f, 0.0);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glEnd();
	glPopMatrix();
	Matrix44From(&mat, &ql);
	glPushMatrix();
		glMultMatrixf(mat);
		glColor3f(0.0f, 0.0f, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glEnd();
	glPopMatrix();
}
#endif
/**
 * GLUT用コールバック
 */
static void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, cam_pos_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// スケルトンの更新
	updateSekelton2(&joint2[0], NULL);

	Matrix44 trans;
	//
	Matrix44Translation(&trans, -5.0f, 0.0f, 0.0f);
	glPushMatrix();
		glMultMatrixf(trans);
		render_skin_mesh0();
	glPopMatrix();
	//
	Matrix44Translation(&trans, 0.0f, 0.0f, 0.0f);
	glPushMatrix();
		glMultMatrixf(trans);
		render_skin_mesh1();
	glPopMatrix();
	//
	Matrix44Translation(&trans, 5.0f, 0.0f, 0.0f);
	glPushMatrix();
		glMultMatrixf(trans);
		render_skin_mesh2();
	glPopMatrix();

	glutSwapBuffers();
}

/**
 * GLUT用コールバック
 */
static void idle(void){
	glutPostRedisplay();
}

/**
 * GLUT用コールバック
 */
static void resize(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#if 1
	gluPerspective((double)fov, (double)w / (double)h, 1.0, 100.0);
#else
	const double hw = (double)w * 0.5f * 0.03; 
	const double hh = (double)h * 0.5f * 0.03;  
	glOrtho(-hw, hw, -hh, hh, 1.0, 100.0);
#endif
	glMatrixMode(GL_MODELVIEW);
}

static int begin_pos_x;
static int begin_pos_y;
static int clicked;
/**
 * GLUT用コールバック
 */
void mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_DOWN){
			clicked = GLUT_LEFT_BUTTON;
			begin_pos_x = x;
			begin_pos_y = y;
		}
		else
		if(state == GLUT_UP){
			clicked = -1;
		}
	}
	else
	if(button == GLUT_RIGHT_BUTTON){
		if(state == GLUT_DOWN){
			clicked = GLUT_RIGHT_BUTTON;
			begin_pos_x = x;
			begin_pos_y = y;
		}
		else
		if(state == GLUT_UP){
			clicked = -1;
		}
	}
}

/**
 * GLUT用コールバック
 */
void motion(int x, int y){
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float dx = (float)(x - begin_pos_x) / vp[2];
	float dy = (float)(begin_pos_y - y) / vp[3];
	float sq = sqrtf(dx*dx+dy*dy);
	if(sq > 0.0f){
		Quaternion q;
		QuaternionRotation(&q, &Vector3(dy/sq, dx/sq, 0.0f), sq * 10.0f);
		
		if(clicked == GLUT_LEFT_BUTTON){
			QuaternionMul(&joint[1].local_pose.rotation, &joint[1].local_pose.rotation, &q);
			QuaternionMul(&joint2[1].local_pose.rotation, &joint2[1].local_pose.rotation, &q);
		}
		else
		if(clicked == GLUT_RIGHT_BUTTON){
			QuaternionMul(&joint[0].local_pose.rotation, &joint[0].local_pose.rotation, &q);
			QuaternionMul(&joint2[0].local_pose.rotation, &joint2[0].local_pose.rotation, &q);
		}
	}
}

/**
 * FREEGLUT用コールバック
 */
void MouseWheel(int wheel_number, int direction, int x, int y){
	cam_pos_z -= (float)direction * 0.1f;
	if(cam_pos_z < 1.0f)
		cam_pos_z = 1.0f;
}

/**
 * エントリ
 */
int main(int argc, char *argv[]){
	// initialize GLUT
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 480);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("ColladaLoader"); // argv[0]
	// initialize GLEW
    GLenum glew_error;
	glew_error = glewInit();
	if(glew_error != GLEW_OK){
		std::cerr << "error: " << glewGetErrorString( glew_error ) << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "OGL  version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// register callback
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(MouseWheel);
	if(!init())
		return EXIT_FAILURE;
	glutMainLoop();
	return EXIT_SUCCESS;
}
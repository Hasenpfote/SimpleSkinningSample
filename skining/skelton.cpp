#include "skelton.h"

/**
 * �p���̕ϊ�
 * @param p �p��
 * ���݂̎p����p��K�p
 */
void JointPose::transform(const JointPose& p){
	QuaternionMul(&rotation, &rotation, &p.rotation);
	Vector3Rotation(&translation, &translation, &p.rotation);
	translation += p.translation;
}
/**
 * �p���̕ϊ�
 * @param p1 �p��1
 * @param p2 �p��2
 * p1��p2��K�p
 */
void JointPose::transform(const JointPose& p1, const JointPose& p2){
	QuaternionMul(&rotation, &p1.rotation, &p2.rotation);
	Vector3Rotation(&translation, &p1.translation, &p2.rotation);
	translation += p2.translation;
}
/**
 * �p���̋t�ϊ�
 * ���݂̎p���̋t�ϊ�
 */
void JointPose::inverse(){
	QuaternionConjugate(&rotation, &rotation);
	Vector3Rotation(&translation, &translation, &rotation);
	translation = -translation;
}
/**
 * �p���̋t�ϊ�
 * @param p �p��
 * p�̋t�ϊ�
 */
void JointPose::inverse(const JointPose& p){
	QuaternionConjugate(&rotation, &p.rotation);
	Vector3Rotation(&translation, &p.translation, &rotation);
	translation = -translation;
}

void JointPose::getMatrix(Matrix44* out){
	Matrix44From(out, &rotation);
	(*out)(0,3) = translation.x;
	(*out)(1,3) = translation.y;
	(*out)(2,3) = translation.z;
}

#pragma once
#include "vector.h"
#include "matrix.h"
#include "quaternion.h"
using namespace mathematics;

// 右手座標系のためクォータニオンの積の順序に注意
class JointPose{
public:
	void transform(const JointPose& p);
	void transform(const JointPose& p1, const JointPose& p2);
	void inverse();
	void inverse(const JointPose& p);
	void getMatrix(Matrix44* out);
public:
	Quaternion rotation;
	Vector3 translation;
	float scale;
};

#if 0
class Joint{
public:
	Joint* child;
	Joint* sibling;
	JointPose bind;
	JointPose inv_bind;
	JointPose local;
	JointPose global;
	JointPose skinning;
};
class Joint{
public:
	Joint* child;
	Joint* sibling;
	Matrix44 bind;
	Matrix44 inv_bind;
	JointPose local;
	Matrix44 global;
	Matrix44 skinning;
};
#endif
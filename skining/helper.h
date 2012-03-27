#pragma once
#include "mathematics.h"
#include "matrix.h"

using namespace mathematics;

class Helper{
public:
	static void renderString(int x, int y, const char* string);
	static void renderCircle(int x, int y, float radius);
	static void renderCoord(const Matrix44* m, float length = 1.0f);
	static void renderJoint(const Matrix44* m, float length = 1.0f);

};
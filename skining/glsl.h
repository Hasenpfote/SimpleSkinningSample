#pragma once
#include <GL/glew.h>

class Glsl{
public:
	Glsl();
	~Glsl();
	bool create(const char* vert, const char* frag);
	GLuint getProgram(){ return program; }

private:
	bool read(GLuint shader, const char* filename);
	bool createVertexShader(GLuint* shader, const char* vert);
	bool createFragmentShader(GLuint* shader, const char* frag);
#ifdef _DEBUG
	void printShaderInfoLog(GLuint shader);
	void printProgramInfoLog(GLuint program);
#endif // _DEBUG
private:
	GLuint program;
};
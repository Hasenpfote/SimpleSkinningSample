#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "glsl.h"

Glsl::Glsl(){
	program = 0;
}

Glsl::~Glsl(){
	if(program != 0)
		glDeleteProgram(program);
}

bool Glsl::create(const char* vert, const char* frag){
	GLuint vertShader = 0;
	GLuint fragShader = 0;
	if(!createVertexShader(&vertShader, vert)){
		return false;
	}
	if(!createFragmentShader(&fragShader, frag)){
		glDeleteShader(vertShader);
		return false;
	}

	program = glCreateProgram();
	if(program == 0){
		Log_e("could not create program.\n");
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		return false;
	}
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
#ifdef _DEBUG
	printProgramInfoLog(program);
#endif // _DEBUG
	if(linked == GL_FALSE){
		Log_e("failed to link.\n");
		glDetachShader(program, vertShader);
		glDetachShader(program, fragShader);
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(program);
		program = 0;
		return false;
	}
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
}

bool Glsl::read(GLuint shader, const char* filename){
	bool result = false;
	FILE* fp = fopen(filename, "rb");
	if(fp == NULL){
		perror(filename);
		return result;
	}
	fseek(fp, 0L, SEEK_END);
	const GLint length = (GLint)ftell(fp);
	GLchar* source = (GLchar*)malloc(length);
	if(source == NULL){
		fclose(fp);
		Log_e("could not allocate memory.\n");
		return result;
	}
	fseek(fp, 0L, SEEK_SET);

	bool ret = (fread((void*)source, 1, length, fp) == (size_t)length);
	fclose(fp);

	if(ret){
		glShaderSource(shader, 1, (const GLchar**)&source, &length);
		result = true;
	}
	else{
		Log_e("could not read file(%s).\n", filename);
	}
	free((void*)source);

	return result;
}

bool Glsl::createVertexShader(GLuint* shader, const char* vert){
	(*shader) = glCreateShader(GL_VERTEX_SHADER);
	if((*shader) == 0){
		Log_e("could not create vertex shader.\n");
		return false;
	}
	if(!read((*shader), vert)){
		glDeleteShader((*shader));
		return false;
	}
	glCompileShader((*shader));
	GLint compiled;
	glGetShaderiv((*shader), GL_COMPILE_STATUS, &compiled);
#ifdef _DEBUG
	printShaderInfoLog((*shader));
#endif // _DEBUG
	if(compiled == GL_FALSE) {
		Log_e("failed to compile in vertex shader.\n");
		glDeleteShader((*shader));
		return false;
	}
	return true;
}

bool Glsl::createFragmentShader(GLuint* shader, const char* frag){
	(*shader) = glCreateShader(GL_FRAGMENT_SHADER);
	if((*shader) == 0){
		Log_e("could not create fragment shader.\n");
		return false;
	}
	if(!read((*shader), frag)){
		glDeleteShader((*shader));
		return false;
	}
	glCompileShader((*shader));
	GLint compiled;
	glGetShaderiv((*shader), GL_COMPILE_STATUS, &compiled);
#ifdef _DEBUG
	printShaderInfoLog((*shader));
#endif // _DEBUG
	if(compiled == GL_FALSE) {
		Log_e("failed to compile in fragment shader.\n");
		glDeleteShader((*shader));
		return false;
	}
	return true;
}

#ifdef _DEBUG
void Glsl::printShaderInfoLog(GLuint shader){
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);
	if(bufSize > 1){
		GLchar* infoLog;
		infoLog = (GLchar*)malloc(bufSize);
		if(infoLog != NULL){
			GLsizei length;
			glGetShaderInfoLog(shader, bufSize, &length, infoLog);
			fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
			free((void*)infoLog);
		}
		else{
			Log_e("could not allocate memory.\n");
		}
	}
}

void Glsl::printProgramInfoLog(GLuint program){
	GLsizei bufSize;
	glGetShaderiv(program, GL_INFO_LOG_LENGTH, &bufSize);
	if(bufSize > 1){
		GLchar* infoLog;
		infoLog = (GLchar*)malloc(bufSize);
		if(infoLog != NULL){
			GLsizei length;
			glGetProgramInfoLog(program, bufSize, &length, infoLog);
			fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
			free((void*)infoLog);
		}
		else{
			Log_e("could not allocate memory.\n");
		}
	}
}
#endif // _DEBUG
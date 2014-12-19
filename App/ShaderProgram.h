#pragma once
#include <vector>

#include "gl_includes.h"
#include "Shader.h"
#include <memory>
#include <iostream>
class ShaderProgram
{
public:
	ShaderProgram(void);
	~ShaderProgram(void);

	/**
		Attempts to compile the shader program.
		Log information and compile status are given by:
		getCompileLog and getCompileStatus respectively.
	*/
	void compile(void);

	/**
		Adds a shader to the program.
	*/
	void addShader(std::shared_ptr<Shader> arg0) 
	{ 
		this->shaders.push_back(arg0);
		glAttachShader(this->index, arg0->getIndex()); 
	};

	/**
		Gets the index of the shader program.
	*/
	GLuint getIndex(void) const { return this->index; };

	/**
		Returns the compile status of the most recent call to compile.
		If no call has been made, returns default GLuint.
	*/
	GLuint getCompileStatus(void) const { return this->compileStatus; };

	/**
		Returns the compile log of the most recetn call to compile.
		If no call has been made, returns empty string ("\0").
	*/
	char * const getCompileLog(void) const { return this->compileLog; };


	/**
		Sets a uniform variable for the program, using the corresponding function.
		Func is a function of the type glUniform*
		params should be ALL of the parameters you wish to provide to the uniform function.
	*/
	template <class Func, typename... Params>
	void setUniform(Func call, const std::string& name, Params... params)
	{
		// uses the program we're in
		
		glUseProgram(this->index);

		// grab the uniform location

		GLint loc = glGetUniformLocation(this->index, name.c_str());

		// call the uniform function with the right parameters

		call(loc, params...);
	}



private:

	std::vector<std::shared_ptr<Shader>> shaders;
	GLuint index;
	GLint compileStatus;
	char* compileLog;
	
};


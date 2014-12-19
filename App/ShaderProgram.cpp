#include "ShaderProgram.h"


ShaderProgram::ShaderProgram(void)
{
	this->compileLog = nullptr;

	// grab a program id
	this->index = glCreateProgram();
}


ShaderProgram::~ShaderProgram(void)
{
	// get rid of any allocated memory for the compile log
	if(this->compileLog != nullptr)
		delete [] this->compileLog;
	this->compileLog = nullptr;

	// detach all the shaders
	for(auto iter = this->shaders.begin(); iter != shaders.end(); iter++)
	{
		glDetachShader(this->index, (*iter)->getIndex());
	}

	// tell opengl to free up the space we were using
	glDeleteProgram(this->index);
}

void ShaderProgram::compile(void)
{
	// link the program
	glLinkProgram(this->index);

	// check to see if it worked
	glGetProgramiv(this->index, GL_LINK_STATUS, &this->compileStatus);
	
	if(this->compileStatus == GL_FALSE)
	{
		// grab the size of the log and then grab the log
		GLint bufferSize;
		glGetProgramiv(this->index, GL_INFO_LOG_LENGTH, &bufferSize);
		bufferSize++;
		if(this->compileLog != nullptr)
		{
			delete [] compileLog;
			compileLog = nullptr;
		}
		this->compileLog = new char [bufferSize];
		GLint logLength;
		glGetProgramInfoLog(this->index, bufferSize, &logLength, this->compileLog);

	} else
	{
		// delete the log
		if(this->compileLog != nullptr)
		{
			delete [] compileLog;
			compileLog = nullptr;
		}
		this->compileLog = nullptr;
	}
	// if we have a log print it for debugging purposes
	if(this->compileLog != nullptr)
		std::cout << this->compileLog;
}

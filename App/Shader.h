#pragma once

#include "gl_includes.h"

#include <string>
#include <memory>

class Shader
{
public:
	Shader(GLuint type):type_(type)
	{
		this->shaderIndex_ = glCreateShader(this->type_);
	}

	~Shader(void)
	{
		glDeleteShader(this->shaderIndex_);
	}

	GLuint getIndex(void) const { return this->shaderIndex_; };
	GLuint getType(void) const { return this->type_; };
private:
	GLuint shaderIndex_;
	GLuint type_;
	friend std::shared_ptr<Shader> make_shader(const std::string&, GLuint);
};

std::shared_ptr<Shader> make_shader(const std::string& fileData, GLuint shaderType);

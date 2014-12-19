#include "Shader.h"
#include <iostream>

std::shared_ptr<Shader> make_shader(const std::string& fileData, GLuint shaderType)
{

	if(fileData != "")
	{
		auto ret = std::make_shared<Shader>(shaderType);

		auto data = fileData.data();
		glShaderSource(ret->shaderIndex_, 1, &data, nullptr);
		glCompileShader(ret->shaderIndex_);

		// grab the compile log if it fails
		GLint rivResults;

		glGetShaderiv(ret->shaderIndex_, GL_COMPILE_STATUS, &rivResults);

		if(rivResults == GL_FALSE)
		{
			
			glGetShaderiv(ret->shaderIndex_, GL_INFO_LOG_LENGTH, &rivResults);
			auto log = new char [rivResults];
			GLint logLength;
			glGetShaderInfoLog(ret->shaderIndex_, rivResults, &logLength, log);
			std::cout << "in shader.cpp" << std::endl;
			std::cout << fileData << std::endl;
			std::cout << log;
			delete [] log;
			
			return std::shared_ptr<Shader>();
		}
		return ret;
	} else
	{
		return std::shared_ptr<Shader>();
	}


}
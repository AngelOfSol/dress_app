#pragma once
#include "gl_includes.h"

template <GLenum First, GLenum... Rest>
struct GLStateOn
	: public GLStateOn<First>, public GLStateOn<Rest...>
{
};

template <GLenum Flag>
struct GLStateOn<Flag>
{
public:
	GLStateOn(void)
	{
		glGetBooleanv(Flag, &this->wasSet_);
		if(this->wasSet_ == GL_FALSE)
			glEnable(Flag);
	};
	~GLStateOn(void)
	{
		if(this->wasSet_ == GL_FALSE)
			glDisable(Flag);
	};
private:
	GLboolean wasSet_;
};

template <GLenum First, GLenum... Rest>
struct GLStateOff
	: public GLStateOff<First>, public GLStateOff<Rest...>
{
};

template <GLenum Flag>
struct GLStateOff<Flag>
{
public:
	GLStateOff(void)
	{
		glGetBooleanv(Flag, &this->wasSet_);
		if(this->wasSet_ == GL_TRUE)
			glDisable(Flag);
	};
	~GLStateOff(void)
	{
		if(this->wasSet_ == GL_TRUE)
			glEnable(Flag);
	};
private:
	GLboolean wasSet_;
};
/*
{
	GLStateOn<GL_DEPTH_TEST> depthOn;
	{
		GLStateOn<GL_TEXTURE_2D> textureOn;
		texturedObject.draw();
	}
	{
		GLStateOff<GL_DEPTH_TEST> depthOff;
		{
			GLStateOn<GL_BLEND> blendOn;
			transparentObject.draw();
		}
		text.draw();
	}
}
*/
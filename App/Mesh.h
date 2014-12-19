#pragma once

#include <vector>

#include "gl_includes.h"

/**
	A class for storing and building mesh data.
	Also draws mesh data.
	Does not contain shader program data.
*/

template <class VertexDataType>
class Mesh
{
public:
	Mesh(int vpd = 1):built(false), verticesPerData(vpd)
	{
	}
	virtual ~Mesh(void)
	{
		this->deleteBuffers();
	}
	
	/**
		A method to build the vertex data into a format acceptable to OpenGL.
		Pre-condition:
			Some number of T have been pushed into the mesh.
		Post-condition:
			If built, vao and vbo contain indicies for the proper opengl data.
			Otherwise built is false.
	*/
	virtual void buildMesh() = 0;

	/**
		Pushes vertex data onto the back of the vertex array.  Insertion order guarenteed unless modified by buildMesh.
	*/
	template <typename... Params>
	void pushVertex(Params... params) { this->vertexData_.emplace_back(params...); };
	void pushVertex(const VertexDataType& data) { this->vertexData_.push_back(data);};

	std::vector<VertexDataType>& data() { return this->vertexData_; };

	/**
		Draws a mesh using the specifed draw type.
		aStart -> the offset from which to start drawing
		aCount -> the amount to draw, with 0 being as many as possible
		fromBack -> starting from the back in reverse order
	*/
	virtual void draw(GLenum drawType, GLuint aStart = 0, GLuint aCount = 0, bool fromBack = false, bool multiplyVertexPerData = true) const
	{
		// make sure we have data to draw

		if(built)
		{
			// binds the vertex array so we can attempt to draw

			glBindVertexArray(this->vao);

			// initailize local copies of arguments ofr modification

			unsigned int count = aCount;
			unsigned int start = aStart;

			// if count is 0, then we know caller wants all fo the data

			if(count == 0)
			{
				count = this->vertexData_.size();
			}

			// if fromBack is true, we need to subtract the count from the size to get the new starting point

			if(fromBack)
			{
				start = this->vertexData_.size() - count;
			}

			// if this would put us out of range, break out early

			if(count + start > this->vertexData_.size())
				return;
			
			// draw

			glDrawArrays(drawType, start, count * (multiplyVertexPerData ? this->verticesPerData : 1));
		}
	};
	void clear()
	{
		this->arrayData_.clear();
		this->vertexData_.clear();
		this->deleteBuffers();
		this->built = false;
	}
protected:
	int verticesPerData;
	/**
		If buffers are allocated to OpenGL, this removes them.
	*/
	void deleteBuffers()
	{
		if(this->vao != 0)
		{
			glDeleteVertexArrays(1, &this->vao);
			glDeleteBuffers(1, &this->vbo);

			this->vao = 0;
			this->vbo = 0;

			this->arrayData_.clear();
		}
	};
	/**
		Generate a new vertex array and vertex buffer array.
	*/
	void genBuffers()
	{
		
		glGenVertexArrays(1, &this->vao);

		glGenBuffers(1, &this->vbo);

	};
	/**
		Binds the buffers for use.
	*/
	void bindBuffers()
	{
		
		glBindVertexArray(this->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	};
	std::vector<VertexDataType> vertexData_;
	std::vector<GLfloat> arrayData_;
	GLuint vao;
	GLuint vbo;
	bool built;

};


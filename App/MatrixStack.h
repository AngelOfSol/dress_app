#pragma once

#include "glm_includes.h"
#include <stack>
#include <functional>
#include <memory>

namespace rmg_com
{

/**
	Simple class for storing transformation matrices in a row.
*/
class MatrixStack
{
public:
	MatrixStack(void);
	~MatrixStack(void);

	/**
		Pushes either the current matrix on to the stack.
	*/
	void push();
	/**
		Right multiplies the current matrix by arg0. Pushes that onto the stack.
	*/
	void push(const glm::mat4&);

	/**
		Set's the current matrix to the most recent matrix, and pops it off the stack.
	*/
	void pop();

	/**
		Returns reference to the current matrix.
	*/
	glm::mat4& get() { return this->currentMatrix_; };

	const glm::mat4& get() const { return this->currentMatrix_; };


private:
	glm::mat4 currentMatrix_;
	std::stack<glm::mat4> history_;
	friend class Transform;
};


	
class Transform
{
public:
	Transform(MatrixStack& stackRef, glm::mat4 transformData);
	~Transform();
private:
	MatrixStack& ref_;
};


};

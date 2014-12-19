#include "MatrixStack.h"

namespace rmg_com
{

Transform::Transform(MatrixStack& ref, glm::mat4 transform):ref_(ref)
{
	ref.push(transform);
}

Transform::~Transform()
{
	this->ref_.pop();
}


MatrixStack::MatrixStack(void)
{
}


MatrixStack::~MatrixStack(void)
{
}

void MatrixStack::push()
{
	this->history_.push(this->currentMatrix_);
}

void MatrixStack::push(const glm::mat4& lhs)
{
	this->push();
	this->currentMatrix_ = this->currentMatrix_ * lhs;
}

void MatrixStack::pop()
{
	this->currentMatrix_ = this->history_.top();
	this->history_.pop();
}


};
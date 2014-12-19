#include "GUIElement.h"
#include <string>

GUIElement::GUIElement(OpenGLHandler& ref):gl_(ref)
{
}


GUIElement::~GUIElement(void)
{
}

glm::vec2& GUIElement::pos()
{
	return this->pos_;
}
glm::vec2 GUIElement::pos() const
{
	return this->pos_;
}
int GUIElement::getNewID()
{
	return GUIElement::idCounter++;
}
int GUIElement::idCounter = 0;
/*
std::ostream& GUIElement::write(std::ostream& out) const
{
	out << this->pos_.x << std::endl;
	out << this->pos_.y << std::endl;

	return out;
}

std::ostream& operator <<(std::ostream& out, const GUIElement& ref)
{
	return ref.write(out);
}
*/
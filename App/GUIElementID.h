#pragma once
#include "GUIElement.h"
#include <memory>
template <class Derived>
class GUIElementID
	: public GUIElement
{
public:
	using ptr = std::shared_ptr<Derived>;
	GUIElementID(OpenGLHandler& ref):GUIElement(ref)
	{
	}
	~GUIElementID(void)
	{
	}
	int getClassID(void) const override
	{
		return GUIElementID::id;
	}
	static const int id;
private:

};
template<class T>
const int GUIElementID<T>::id = GUIElement::getNewID();
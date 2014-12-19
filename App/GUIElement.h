#pragma once
#include "glm_includes.h"
#include <fstream>
class OpenGLHandler;
class GUIElement
{
public:
	GUIElement(OpenGLHandler&);
	~GUIElement(void);
	virtual void draw(void) const = 0;
	virtual void update(int) = 0;
	virtual glm::vec2& pos();
	virtual glm::vec2 pos() const;
	virtual int getClassID() const = 0;
	static int getNewID();
	static int idCounter;
protected:
	OpenGLHandler& gl_;
	glm::vec2 pos_;
};

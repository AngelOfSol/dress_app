#pragma once
#include "OpenGLHandler.h"
#include <queue>
class KeyboardInput
{
public:
	enum SPECIAL
	{
		ENTER = 1,
		BACKSPACE = 2
	};

	KeyboardInput(OpenGLHandler&);
	~KeyboardInput(void);
	bool ready(void) const;
	char get(void);
	void update(void);
private:
	OpenGLHandler& source_;
	std::queue<char> output_;
	OpenGLHandler::Keys oldKeys_;
};


#pragma once
#include "KeyboardInput.h"
#include "OpenGLHandler.h"
class TextEntry
{
public:
	TextEntry(OpenGLHandler&, std::string default = "");
	~TextEntry(void);
	void update(int elapsed);
	std::string data(void) const;
	bool& focus(void);
	unsigned int& freeze(void);
	void setOnEnter(std::function<bool(std::string)> callback);
	std::string& filter();
	std::string filter() const;
private:
	bool focus_;
	KeyboardInput stream_;
	std::string data_;
	std::string filter_;
	unsigned int cursorTime_;
	unsigned int freeze_;
	bool cursorOn_;
	std::function<bool(std::string)> onEnter_;
};


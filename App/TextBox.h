#pragma once
#include "Box.h"
#include "TextEntry.h"
#include "GUIElement.h"
#include "fileio.h"
class TextBox
	: public GUIElementID<TextBox>
{
public:
	TextBox(OpenGLHandler&, std::string font, glm::vec2 pos, int width, std::string default);
	~TextBox(void);
	void update(int elapsed) override;
	void draw(void) const override;
	glm::vec2& pos(void) override;
	glm::vec2 pos(void) const override;
	glm::vec2 size(void) const;
	std::string font(void) const;
	std::string& filter(void);
	std::string filter(void) const;
	std::string data(void) const;
	int width(void) const;
	glm::vec2 padding() const;
	std::string default() const;
private:
	enum 
	{
		NONE,
		HOVER,
		FOCUS
	} state_;
	std::shared_ptr<Box> box_;
	TextEntry text_;
	std::string font_;
	int width_;
	glm::vec2 padding_;
	std::string default_;
	friend std::shared_ptr<TextBox> make_textbox(OpenGLHandler&, std::string font, glm::vec2 pos, int width, std::string = "");
};

std::shared_ptr<TextBox> read_text_box(std::istream&, OpenGLHandler&);
std::ostream& write_text_box(std::ostream&, const TextBox& ref);
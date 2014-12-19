#pragma once
#include "button.h"
#include "GUIElementID.h"

class TextButton :
	public GUIElementID<TextButton>
{
public:
	TextButton(OpenGLHandler&, std::string font, std::string label);
	~TextButton(void);
	std::string label() const;
	void draw() const override;
	void update(int) override;
	glm::vec2 size(void) const;
	std::string font() const;
	std::function<void(bool)>& onPress();
private:
	std::shared_ptr<Button> button_;
	glm::vec2 padding_;
	std::string label_;
	std::string font_;
	friend std::shared_ptr<TextButton> make_textbutton(OpenGLHandler& ref, std::string font, std::string label, glm::vec2 pos);
};


std::shared_ptr<TextButton> read_text_button(std::istream&, OpenGLHandler&);
std::ostream& write(std::ostream&, const TextButton& ref);
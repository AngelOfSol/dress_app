#pragma once
#include "TextBox.h"
#include "GUIElementID.h"
class LabeledTextbox
	: public GUIElementID<LabeledTextbox>
{
public:
	LabeledTextbox(OpenGLHandler& ref, std::string font, glm::vec2 pos, int width, std::string default);
	~LabeledTextbox(void);
	void update(int) override;
	void draw(void) const override;
	std::string& label();
	std::string label() const;
	std::string font() const;
	std::string& filter();
	std::string filter() const;
	std::string data(void) const;
	std::string default(void) const;
	int width() const;
private:
	std::shared_ptr<TextBox> text_;
	std::string label_;
	friend std::shared_ptr<LabeledTextbox> make_labeled_textbox(OpenGLHandler& ref, std::string font, std::string label, glm::vec2 pos, int width, std::string = "");
};


std::shared_ptr<LabeledTextbox> read_labeled_textbox(std::istream&, OpenGLHandler&);
std::ostream& write(std::ostream&, const LabeledTextbox& ref);
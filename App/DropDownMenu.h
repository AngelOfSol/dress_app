#pragma once
#include "GUIElementID.h"
#include <vector>
#include <string>
#include "Box.h"
#include "Button.h"
class DropDownMenu
	: public GUIElementID<DropDownMenu>
{
public:
	DropDownMenu(OpenGLHandler&, glm::vec2 pos, std::string font, int width);
	~DropDownMenu(void);
	void update(int) override;
	void draw(void) const override;
	std::vector<std::string>& choices();
	const std::vector<std::string>& choices() const;
	std::string currentChoice() const;
	glm::vec2& pos() override;
	glm::vec2 pos() const override;
	int width() const;
	std::string font() const;
private:
	glm::vec2 padding_;
	std::vector<std::string> choices_;
	int currentChoice_;
	int hoveredChoice_;
	std::shared_ptr<Box> selected_;
	std::shared_ptr<Box> hovered_;
	std::shared_ptr<Button> button_;
	std::shared_ptr<Box> choiceBox_;
	std::string font_;
	int width_;
	enum
	{
		NONE
		, FOCUS
	} state_;

	friend std::shared_ptr<DropDownMenu> make_dropdown(OpenGLHandler&, glm::vec2 pos, std::string font, int width);
};

std::shared_ptr<DropDownMenu> read_dropdown(std::istream&, OpenGLHandler&);
std::ostream& write(std::ostream&, const DropDownMenu& ref);
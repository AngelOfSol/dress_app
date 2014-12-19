#pragma once
#include "GUIElementID.h"
#include "Box.h"
#include <functional>
class Button
	: public GUIElementID<Button>
{
public:
	Button(OpenGLHandler&, glm::vec2 size);
	~Button(void);
	glm::vec2& pos(void) override;
	glm::vec2 pos(void) const override;
	glm::vec2& size(void);
	glm::vec2 size(void) const;
	void update(int) override;
	void draw() const override;
	bool pressed() const;
	void off();
	void on();
	std::function<void(bool)>& onPress();
private:
	enum
	{
		NONE,
		HOVER,
		PRESSED
	} state_;
	std::shared_ptr<Box> box_;
	std::function<void(bool)> onPress_;

	friend std::shared_ptr<Button> make_button(OpenGLHandler& ref, glm::vec2 pos, glm::vec2 size);
};

std::shared_ptr<Button> read_button(std::istream&, OpenGLHandler&);
std::ostream& write(std::ostream&, const Button& ref);
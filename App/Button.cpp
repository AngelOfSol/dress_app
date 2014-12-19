#include "Button.h"
#include "collision.h"
#include "OpenGLHandler.h"

Button::Button(OpenGLHandler& ref, glm::vec2 size):GUIElementID(ref), state_(Button::NONE)
{
	this->onPress_ = [] (bool) {};
	this->box_ = make_box(ref, glm::vec2(), size);
}


Button::~Button(void)
{
}


glm::vec2& Button::pos(void)
{
	return this->box_->pos();
}
glm::vec2 Button::pos(void) const
{
	return this->box_->pos();
}

glm::vec2& Button::size(void)
{
	return this->box_->size();
}
glm::vec2 Button::size(void) const
{
	return this->box_->size();
}
std::function<void(bool)>& Button::onPress()
{
	return this->onPress_;
}
bool Button::pressed() const
{
	return this->state_ == Button::PRESSED;
}

void Button::off()
{
	auto mouse = this->gl_.mouse();
	auto center = this->pos() + this->size() / 2.0f;
	bool mouseOver = isColliding(mouse, center, this->size());
	if (mouseOver)
		this->state_ = Button::HOVER;
	else
		this->state_ = Button::NONE;
	this->onPress_(false);
}
void Button::on()
{
	this->state_ = Button::PRESSED;
	this->onPress_(true);
}

void Button::update(int elapsed)
{
	auto mouse = this->gl_.mouse();
	auto center = this->pos() + this->size() / 2.0f;

	bool mouseOver = isColliding(mouse, center, this->size());

	switch(this->state_)
	{
	case Button::NONE:
		this->box_->color() = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
		if(mouseOver)
			this->state_ = Button::HOVER;
		break;
	case Button::HOVER:
		this->box_->color() = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
		if(!mouseOver)
		{
			this->state_ = Button::NONE;
		}
		else if(mouseOver && mouse.left)
		{
			this->state_ = Button::PRESSED;
			this->onPress_(true);
		}
		break;
	case Button::PRESSED:
		this->box_->color() = glm::vec4(0.35f, 0.35f, 0.35f, 1.0f);
		if(!mouse.left)
		{
			this->state_ = Button::NONE;
			this->onPress_(false);
		}
		break;
	}
}
void Button::draw(void) const
{
	this->box_->draw();
	
}
std::shared_ptr<Button> make_button(OpenGLHandler& ref, glm::vec2 pos, glm::vec2 size)
{
	auto ret = std::make_shared<Button>(ref, size);

	ret->pos() = pos;

	return ret;
}
using namespace jck::file::binary;

std::shared_ptr<Button> read_button(std::istream& in, OpenGLHandler& handler)
{
	glm::vec2 pos;
	glm::vec2 size;
	read(in, pos);
	read(in, size);
	return make_button(handler, pos, size);
}
std::ostream& write(std::ostream& out, const Button& ref)
{
	write(out, ref.pos());
	write(out, ref.size());
	return out;
}
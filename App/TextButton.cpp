#include "TextButton.h"
#include "OpenGLHandler.h"

TextButton::TextButton(OpenGLHandler& ref, std::string font, std::string label)
	: GUIElementID(ref)
	, label_(label)
	, font_(font)
	, padding_(20.0f, 20.0f)
{
	glm::vec2 size;
	size.x = label.size() * ref.getFont(font).width + this->padding_.x;
	size.y = ref.getFont(font).width + this->padding_.y;
	this->button_ = make_button(ref, glm::vec2(), size);
}
std::string TextButton::label(void) const
{
	return this->label_;
}
std::string TextButton::font(void) const
{
	return this->font_;
}
glm::vec2 TextButton::size(void) const
{
	return this->button_->size();
}

std::function<void(bool)>& TextButton::onPress()
{
	return this->button_->onPress();
}

TextButton::~TextButton(void)
{
}
void TextButton::update(int elapsed)
{
	this->button_->pos() = this->pos();
	this->button_->update(elapsed);
}
void TextButton::draw(void) const 
{
	//this->button_->draw();
	this->gl_.useFont(this->font_);
	this->gl_.write(this->label_, this->pos() + this->padding_ / 2.0f);
}

std::shared_ptr<TextButton> make_textbutton(OpenGLHandler& ref, std::string font, std::string label, glm::vec2 pos)
{
	auto ret = std::make_shared<TextButton>(ref, font, label);
	ret->pos() = pos;
	return ret;
}

using namespace jck::file::binary;
std::shared_ptr<TextButton> read_text_button(std::istream& in, OpenGLHandler& handler)
{
	std::string font;
	std::string label;
	glm::vec2 pos;
	
	read(in, font);
	read(in, label);
	read(in, pos);

	return make_textbutton(handler, font, label, pos);
};
std::ostream& write(std::ostream& out, const TextButton& ref)
{
	write(out, ref.font());
	write(out, ref.label());
	write(out, ref.pos());
	return out;
};
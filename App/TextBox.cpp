#include "TextBox.h"


TextBox::TextBox(OpenGLHandler& ref, std::string font, glm::vec2 pos, int width, std::string default) :GUIElementID(ref), text_(ref, default), font_(font), width_(width), state_(TextBox::NONE), padding_(20.0f, 20.0f)
{
	this->default_ = default;
	glm::vec2 sizes;
	sizes.x = this->width_ * this->gl_.getFont(this->font_).width + this->padding_.x;
	sizes.y = this->gl_.getFont(this->font_).width + this->padding_.y;
	this->box_ = make_box(ref, pos, sizes);
	this->text_.focus() = true;
	this->text_.freeze() = this->width_;
	this->text_.setOnEnter([this] (std::string)
	{
		this->state_ = TextBox::NONE;
		return false;
	});
}


TextBox::~TextBox(void)
{
}
glm::vec2& TextBox::pos(void)
{
	return this->box_->pos();
}
glm::vec2 TextBox::pos(void) const
{
	return this->box_->pos();
}

glm::vec2 TextBox::size(void) const
{
	return this->box_->size();
}
glm::vec2 TextBox::padding(void) const
{
	return this->padding_;
}
std::string TextBox::font(void) const
{
	return this->font_;
}
std::string& TextBox::filter(void) 
{
	return this->text_.filter();
}
std::string TextBox::filter(void) const
{
	return this->text_.filter();
}
int TextBox::width(void) const
{
	return this->width_;
}
std::string TextBox::data(void) const
{
	return this->text_.data();
}
std::string TextBox::default(void) const
{
	return this->default_;
}

void TextBox::update(int elapsed)
{
	auto mouse = this->gl_.mouse();
	auto center = this->pos() + this->size() / 2.0f;
	auto dist = (center) - mouse;
	dist.x = abs(dist.x);
	dist.y = abs(dist.y);

	bool mouseOver = dist.x <= this->size().x / 2.0f && dist.y <= this->size().y / 2.0f;

	switch(this->state_)
	{
	case TextBox::NONE:
		this->box_->color() = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
		this->text_.focus() = false;
		if(mouseOver)
			this->state_ = TextBox::HOVER;
		break;
	case TextBox::HOVER:
		this->box_->color() = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
		this->text_.focus() = false;
		if(!mouseOver)
			this->state_ = TextBox::NONE;
		else if(mouseOver && mouse.left)
			this->state_= TextBox::FOCUS;
		break;
	case TextBox::FOCUS:
		this->box_->color() = glm::vec4(0.35f, 0.35f, 0.35f, 1.0f);
		this->text_.focus() = true;
		if(!mouseOver && mouse.left)
			this->state_ = TextBox::NONE;
		break;
	}
	this->text_.update(elapsed);
}
void TextBox::draw(void) const
{
	this->box_->draw();
	
	this->gl_.useFont(this->font_);
	this->gl_.write(this->text_.data().substr(0, this->width_), this->pos() + this->padding_ / 2.0f);
}

std::shared_ptr<TextBox> make_textbox(OpenGLHandler& ref, std::string font, glm::vec2 pos, int width, std::string default)
{
	return std::make_shared<TextBox>(ref, font, pos, width, default);
}

using namespace jck::file::binary;
std::shared_ptr<TextBox> read_text_box(std::istream& in, OpenGLHandler& handler)
{
	std::string font;
	glm::vec2 pos;
	int width;
	std::string filter;
	std::string default;

	read(in, font);
	read(in, pos);
	read(in, width);
	read(in, filter);
	read(in, default);

	auto ret = make_textbox(handler, font, pos, width, default);
	ret->filter() = filter;

	return ret;
};
std::ostream& write(std::ostream& out, const TextBox& ref)
{
	write(out, ref.font());
	write(out, ref.pos());
	write(out, ref.width());
	write(out, ref.filter());
	write(out, ref.default());
	return out;
};
#include "LabeledTextbox.h"


LabeledTextbox::LabeledTextbox(OpenGLHandler& ref, std::string font, glm::vec2 pos, int width, std::string default) :GUIElementID(ref)
{
	this->text_ = make_textbox(ref, font, pos, width, default);
	this->pos() = pos;
}


LabeledTextbox::~LabeledTextbox(void)
{
}

std::string& LabeledTextbox::label()
{
	return this->label_;
}
std::string LabeledTextbox::label() const
{
	return this->label_;
}
std::string LabeledTextbox::font() const
{
	return this->text_->font();
}
int LabeledTextbox::width() const
{
	return this->text_->width();
}
std::string& LabeledTextbox::filter()
{
	return this->text_->filter();
}
std::string LabeledTextbox::filter() const
{
	return this->text_->filter();
}
std::string LabeledTextbox::data(void) const
{
	return this->text_->data();
}
std::string LabeledTextbox::default(void) const
{
	return this->text_->default();
}


void LabeledTextbox::update(int elapsed)
{
	this->text_->pos() = this->pos();
	this->text_->update(elapsed);
}

void LabeledTextbox::draw(void) const
{
	this->text_->draw();
	this->gl_.write(this->label_, this->pos() + glm::vec2(0, this->text_->padding().y / 2.0f), false);
}

std::shared_ptr<LabeledTextbox> make_labeled_textbox(OpenGLHandler& ref, std::string font, std::string label, glm::vec2 pos, int width, std::string default)
{
	auto ret = std::make_shared<LabeledTextbox>(ref, font, pos, width, default);
	ret->label_ = label;
	return ret;
}
using namespace jck::file::binary;
std::shared_ptr<LabeledTextbox> read_labeled_textbox(std::istream& in, OpenGLHandler& handler)
{
	std::string font;
	std::string label;
	glm::vec2 pos;
	int width;
	std::string filter;
	std::string default;
	read(in, font);
	read(in, label);
	read(in, pos);
	read(in, width);
	read(in, filter);
	read(in, default);
	auto ret = make_labeled_textbox(handler, font, label, pos, width, default);
	ret->filter() = filter;
	return ret;
}
std::ostream& write(std::ostream& out, const LabeledTextbox& ref)
{
	write(out, ref.font());
	write(out, ref.label());
	write(out, ref.pos());
	write(out, ref.width());
	write(out, ref.filter());
	write(out, ref.default());
	return out;
}
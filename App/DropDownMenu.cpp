#include "DropDownMenu.h"
#include "collision.h"
#include "OpenGLHandler.h"

DropDownMenu::DropDownMenu(OpenGLHandler& ref, glm::vec2 pos, std::string font, int width)
	: GUIElementID(ref)
	, state_(DropDownMenu::NONE)
	, choices_(1, "")
	, padding_(20.0f, 20.0f)
	, currentChoice_(0)
	, hoveredChoice_(0)
	, width_(width)
	, font_(font)
{
	glm::vec2 sizes;
	sizes.x = this->width_ * this->gl_.getFont(this->font_).width + this->padding_.x;
	sizes.y = this->gl_.getFont(this->font_).width + this->padding_.y;

	this->selected_ = make_box(ref, pos, sizes);
	this->selected_->color() = glm::vec4(0.12f, 0.12f, 0.12f, 1.0f);

	this->button_ = make_button(ref, pos, glm::vec2(sizes.y, sizes.y));
	this->button_->onPress() = [this] (bool pressed)
	{
		if(!pressed)
		{
			switch(this->state_)
			{
			case DropDownMenu::NONE:
				this->state_ = DropDownMenu::FOCUS;
				break;
			case DropDownMenu::FOCUS:
				this->state_ = DropDownMenu::NONE;
				break;
			}
		}
	};

	this->pos() = pos;

	this->choiceBox_ = make_box(ref, pos, sizes);
	this->choiceBox_->color() = glm::vec4(0.1f, 0.1f,0.1f,1.0f);
	this->hovered_ = make_box(ref, pos, sizes);
	this->hovered_->color() = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
}


DropDownMenu::~DropDownMenu(void)
{
}
glm::vec2& DropDownMenu::pos(void)
{
	return this->selected_->pos();
}
glm::vec2 DropDownMenu::pos(void) const
{
	return this->selected_->pos();
}
std::string DropDownMenu::currentChoice() const
{
	return this->choices_.at(this->currentChoice_);
}

std::vector<std::string>& DropDownMenu::choices(void)
{
	return this->choices_;
}
const std::vector<std::string>& DropDownMenu::choices(void) const
{
	return this->choices_;
}
int DropDownMenu::width() const
{
	return this->width_;
}
std::string DropDownMenu::font() const
{
	return this->font_;
}

void DropDownMenu::update(int elapsed) 
{
	this->button_->update(elapsed);

	this->button_->pos().x = this->pos().x + this->selected_->size().x;
	this->button_->pos().y = this->pos().y;
	this->choiceBox_->pos().x = this->pos().x;
	this->choiceBox_->pos().y = this->pos().y - this->choices_.size() * this->selected_->size().y;
	this->choiceBox_->size().y = this->choices_.size() * this->selected_->size().y;
	
	if(this->state_ == DropDownMenu::FOCUS)
	{
		bool mouseOver = isColliding(this->gl_.mouse(), this->choiceBox_->pos() + this->choiceBox_->size() / 2.0f, this->choiceBox_->size());
		if(mouseOver)
		{
			auto yDiff = this->gl_.mouse().y - this->choiceBox_->pos().y;
			yDiff /= this->selected_->size().y;
			this->hoveredChoice_ = (unsigned int)std::floor(yDiff);
			if(this->gl_.mouse().left)
			{
				this->currentChoice_ = this->hoveredChoice_;
			}
		} else
		{
			this->hoveredChoice_ = -1;
		}
		if(this->gl_.mouse().left)
		{
			this->state_ = DropDownMenu::NONE;
		}
	}
	
	this->hovered_->pos() = this->choiceBox_->pos();
	this->hovered_->pos().y += this->hoveredChoice_ * this->selected_->size().y;
}
void DropDownMenu::draw(void) const 
{
	// write selected text
	this->selected_->draw();
	this->button_->draw();
	glm::vec2 selectedTextPos = this->pos();
	selectedTextPos.x += this->selected_->size().x - this->padding_.x / 2.0f,
	selectedTextPos.y += this->padding_.y / 2.0f;
	this->gl_.useFont(this->font_);
	this->gl_.write(this->currentChoice(), selectedTextPos, false);
	// draw the button
	if(this->state_ == DropDownMenu::FOCUS)
	{
		int offset = 0;
		this->choiceBox_->draw();
		if(this->hoveredChoice_ >= 0 && this->hoveredChoice_ < this->choices_.size())
			this->hovered_->draw();
		for(auto choice : this->choices_)
		{
			selectedTextPos = this->choiceBox_->pos();
			selectedTextPos.x += this->selected_->size().x - this->padding_.x / 2.0f;
			selectedTextPos.y += offset * this->selected_->size().y + this->padding_.y / 2.0f;
			
			this->gl_.useFont(this->font_);
			this->gl_.write(choice, selectedTextPos, false);

			offset++;
		}
		// draw choices text
	}
}

std::shared_ptr<DropDownMenu> make_dropdown(OpenGLHandler& ref, glm::vec2 pos, std::string font, int width)
{
	return std::make_shared<DropDownMenu>(ref, pos, font, width);
}

using namespace jck::file::binary;

std::shared_ptr<DropDownMenu> read_dropdown(std::istream& in, OpenGLHandler& handler)
{
	glm::vec2 pos;
	std::string font;
	int width;
	int choiceCount;
	read(in, pos);
	read(in, font);
	read(in, width);
	auto ret =  make_dropdown(handler, pos, font, width);
	read(in, choiceCount);
	ret->choices().resize(choiceCount);
	for(int i = 0; i < choiceCount; i++)
	{
		ret->choices()[i] = read<std::string>(in);
	}
	
	return ret;
}
std::ostream& write(std::ostream& out, const DropDownMenu& ref)
{
	write(out, ref.pos());
	write(out, ref.font());
	write(out, ref.width());
	write(out, ref.choices().size());
	for(const auto& choice : ref.choices())
	{
		write(out, choice);
	}
	return out;
}
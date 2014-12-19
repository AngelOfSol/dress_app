#include "SetupScreen.h"
#include <fstream>
#include "LabeledTextbox.h"
#include "fileio.h"
#include "TextButton.h"
#include "TextBox.h"
#include "state_gl.h"
#include "Image.h"
#include "Button.h"
#include "DropDownMenu.h"
#include "WindowsDialog.h"
#include "RenderScene.h"
#include "file_loc.h"
SetupScreen::SetupScreen(OpenGLHandler& parent, cylinder_data& output):SceneID(parent), keyboard_(parent), done_(false), output_(output)
{
}


SetupScreen::~SetupScreen(void)
{
}

void SetupScreen::init(void)
{
	this->state_ = SetupScreen::NONE;
	this->curBox_ = 0;
	this->mouse_ = "";
	if(false)
	{
		this->load("ui.txt");
	} else
	{
		auto& config = *this->output_.config;
		std::string numberFilter = "-1234567890.";

		this->radius = make_labeled_textbox(this->gl_, "courier", "Radius:"
			, glm::vec2(config.as<int>("radius-textbox-x"), config.as<int>("radius-textbox-y"))
			, 6);
		this->radius->filter() = numberFilter;
		this->guiElements_.push_back(this->radius);

		this->length = make_labeled_textbox(this->gl_, "courier", "Length:"
			, glm::vec2(config.as<int>("length-textbox-x"), config.as<int>("length-textbox-y"))
			, 6);
		this->length->filter() = numberFilter;
		this->guiElements_.push_back(this->length);

		this->button = make_textbutton(this->gl_, "courier", "Finish"
			, glm::vec2(config.as<int>("finish-button-x"), config.as<int>("finish-button-y"))
			);
		this->button->onPress() = [this](bool pressed)
		{
			if(!pressed)
			{
				this->done_ = true;
			}
		};
		this->guiElements_.push_back(this->button);

		this->unitType = make_dropdown(this->gl_
			, glm::vec2(config.as<int>("unit-dropdown-x"), config.as<int>("unit-dropdown-y"))
			, "courier", 4);
		this->unitType->choices()[0] = "in";
		this->unitType->choices().push_back("mm");
		this->guiElements_.push_back(this->unitType);

		this->toolTip = make_dropdown(this->gl_
			, glm::vec2(config.as<int>("tooltip-dropdown-x"), config.as<int>("tooltip-dropdown-y"))
			, "courier", 6);
		this->toolTip->choices()[0] = "point";
		this->toolTip->choices().push_back("round");
		this->guiElements_.push_back(this->toolTip);

		this->side = make_dropdown(this->gl_
			, glm::vec2(config.as<int>("side-dropdown-x"), config.as<int>("side-dropdown-y"))
			, "courier", 9);
		this->side->choices()[0] = "OPERATOR";
		this->side->choices().push_back("EXIT");
		this->guiElements_.push_back(this->side);

		this->toolHomeX = make_labeled_textbox(this->gl_, "courier", "Tool Home Z:"
			, glm::vec2(config.as<int>("tool-home-z-textbox-x"), config.as<int>("tool-home-z-textbox-y"))
			, 6);
		this->toolHomeX->filter() = numberFilter;
		this->guiElements_.push_back(this->toolHomeX);

		this->toolHomeY = make_labeled_textbox(this->gl_, "courier", "Tool Home X:"
			, glm::vec2(config.as<int>("tool-home-x-textbox-x"), config.as<int>("tool-home-x-textbox-y"))
			, 6);
		this->toolHomeY->filter() = numberFilter;
		this->guiElements_.push_back(this->toolHomeY);

		/*this->offRadius = make_labeled_textbox(this->gl_, "courier", "Offset Radius:", glm::vec2(), 6, "0");
		this->offRadius->filter() = numberFilter;
		this->guiElements_.push_back(this->offRadius);

		auto offLength = make_labeled_textbox(this->gl_, "courier", "Offset Length:", glm::vec2(), 6, "0");
		offLength->filter() = numberFilter;
		this->guiElements_.push_back(offLength);*/
		//auto imageTest = make_image(this->gl_, glm::vec2(0, 0), glm::vec2(100, 100), "test.png");
		//this->guiElements_.push_back(imageTest);
		//this->save("ui.txt");
	}
}
scene_id SetupScreen::update(int elapsed)
{
	if (this->output_.pipe->check())
	{
		if (this->output_.pipe->get() == "close")
			this->gl_.close();
	}
	if (this->gl_.keys()['p'])
	{
		this->gl_.close();
	}


	this->keyboard_.update();
	for(auto& element : this->guiElements_)
	{
		element->update(elapsed);
	}
	if(this->state_ == SetupScreen::NONE)
	{
		if(this->keyboard_.ready())
		{
			char c = this->keyboard_.get();
			if(this->gl_.keys().control)
			{
				switch(c)
				{
				case 's':
					this->save("ui.txt");
					break;
				case 'o':
					this->load("ui.txt");
					break;
				}
			}
			if(this->gl_.keys().alt)
			{
				switch(c)
				{
				case 'p':
					switch(this->state_)
					{
					case SetupScreen::NONE:
						this->state_ = SetupScreen::POSITIONING;
						break;
					case SetupScreen::POSITIONING:
						this->state_ = SetupScreen::NONE;
						break;
					}
					break;
				}
			}
		}	
	}
	if(this->state_ == SetupScreen::POSITIONING)
	{
		auto& curbox = this->guiElements_[this->curBox_];
		if(this->keyboard_.ready())
		{
			char c = this->keyboard_.get();
			switch(c)
			{
			case 'R':
				this->guiElements_.erase(this->guiElements_.begin() + this->curBox_);
				break;
			case 'w':
				curbox->pos().y += 1;
				break;
			case 'W':
				curbox->pos().y += 10;
				break;
			case 's':
				curbox->pos().y -= 1;
				break;
			case 'S':
				curbox->pos().y -= 10;
				break;
			case 'a':
				curbox->pos().x -= 1;
				break;
			case 'A':
				curbox->pos().x -= 10;
				break;
			case 'd':
				curbox->pos().x += 1;
				break;
			case 'D':
				curbox->pos().x += 10;
				break;
			case ';':
				this->curBox_--;
				break;
			case '\'':
				this->curBox_++;
				break;
			}
			if(this->gl_.keys().alt)
			{
				switch(c)
				{
				case 'p':
					switch(this->state_)
					{
					case SetupScreen::NONE:
						this->state_ = SetupScreen::POSITIONING;
						break;
					case SetupScreen::POSITIONING:
						this->state_ = SetupScreen::NONE;
						break;
					}
					break;
				}
			}
			if(this->gl_.keys().control)
			{
				switch(c)
				{
				case 's':
					this->save("ui.txt");
					break;
				case 'o':
					this->load("ui.txt");
					break;
				}
			}
		}
		if(this->gl_.mouse().right)
		{
			curbox->pos() = this->gl_.mouse();
		}
		if(this->curBox_ < 0)
			this->curBox_ = this->guiElements_.size() - 1;
		else if((unsigned int)this->curBox_ >= this->guiElements_.size())
			this->curBox_ = 0;
		this->mouse_ = "Active: '" + std::to_string(this->curBox_) + "' @ (" + std::to_string((int)curbox->pos().x) + ", " + std::to_string((int)curbox->pos().y) + ")";
	}
	if(!this->done_)
	{
		return this->id;
	}
	else
	{
		try
		{
			for(const auto& element : this->guiElements_)
			{
				int id = element->getClassID();
				if(id == DropDownMenu::id)
				{
					auto dropDown = dynamic_cast<const DropDownMenu&>(*element);
					if(dropDown.currentChoice() == "mm")
					{
						this->output_.unitType = units::millimeters;
					} else if(dropDown.currentChoice() == "in")
					{
						this->output_.unitType = units::inches;
					}
					else if (dropDown.currentChoice() == "round")
					{
						this->output_.toolType = tools::rounded;
					}
					else if (dropDown.currentChoice() == "point")
					{
						this->output_.toolType = tools::triangle;
					}
					else if (dropDown.currentChoice() == "EXIT")
					{
						this->output_.side = origin_side::exit;
					}
					else if (dropDown.currentChoice() == "OPERATOR")
					{
						this->output_.side = origin_side::operator_;
					}
				} else if(id == LabeledTextbox::id)
				{
					auto textBox = dynamic_cast<const LabeledTextbox&>(*element);
					if(textBox.label() == "Tool Home X:")
					{
						this->output_.toolHome.x = stor<base_int>(textBox.data());
					} else if(textBox.label() == "Tool Home X:")
					{
						this->output_.toolHome.y = stor<base_int>(textBox.data());
					}
					else if (textBox.label() == "Radius:")
					{
						this->output_.size.y = stor<base_int>(textBox.data());
					}
					else if (textBox.label() == "Length:")
					{
						this->output_.size.x = stor<base_int>(textBox.data());
					}
					else if (textBox.label() == "Offset Radius:")
					{
						this->output_.offset.y = stor<base_int>(textBox.data());
					}
					else if (textBox.label() == "Offset Length:")
					{
						this->output_.offset.x = stor<base_int>(textBox.data());
					}
				}
			}
		} catch (std::invalid_argument e)
		{
			this->done_ = false;
			return this->id;
		}
		if (this->output_.fileName == "ERROR")
			this->output_.fileName = GetFileName("Select a Parts File");
		if (this->output_.fileName != "ERROR")
		{
			return RenderScene::id;
		}
		else
		{
			this->done_ = false;
			return this->id;
		}
	}
}
void SetupScreen::draw(void)
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	for(auto& element : this->guiElements_)
	{
		element->draw();
	}
	this->gl_.useFont("courier");
	this->gl_.write(
		this->mouse_
		, glm::vec2()
	);
}
using namespace jck::file::binary;
void SetupScreen::save(std::string filePath)
{
	std::ofstream outFile(get_path(filePath), std::ios::trunc);
	for(const auto& element : this->guiElements_)
	{
		int id = element->getClassID();
		write(outFile, id);
		if(id == Button::id)
		{
			write(outFile, dynamic_cast<const Button&>(*element));
		} else if(id == DropDownMenu::id)
		{
			write(outFile, dynamic_cast<const DropDownMenu&>(*element));
		} else if(id == LabeledTextbox::id)
		{
			write(outFile, dynamic_cast<const LabeledTextbox&>(*element));
		} else if(id == TextBox::id)
		{
			write(outFile, dynamic_cast<const TextBox&>(*element));
		} else if(id == TextButton::id)
		{
			write(outFile, dynamic_cast<const TextButton&>(*element));
		} else if(id == Image::id)
		{
			write(outFile, dynamic_cast<const Image&>(*element));
		}
	}

}
void SetupScreen::load(std::string filePath)
{
	std::ifstream inFile(get_path(filePath));
	while(!inFile.eof())
	{
		int id = read<int>(inFile);
		if(id == Button::id)
		{

			this->guiElements_.push_back(read_button(inFile, this->gl_));
		} else if(id == DropDownMenu::id)
		{
			this->guiElements_.push_back(read_dropdown(inFile, this->gl_));
		} else if(id == LabeledTextbox::id)
		{
			this->guiElements_.push_back(read_labeled_textbox(inFile, this->gl_));
		} else if(id == TextBox::id)
		{
			this->guiElements_.push_back(read_text_box(inFile, this->gl_));
		} else if(id == TextButton::id)
		{
			auto textButton = read_text_button(inFile, this->gl_);
			if(textButton->label() == "Finish")
			{
				textButton->onPress() = [this] (bool pressed)
				{
					if(!pressed)
					{
						this->done_ = true;
					}
				};
			}
			this->guiElements_.push_back(textButton);
		} else if(id == Image::id)
		{
			this->guiElements_.push_back(read_image(inFile, this->gl_));
		}
	}
}
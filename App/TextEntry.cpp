#include "TextEntry.h"


TextEntry::TextEntry(OpenGLHandler& source, std::string default):stream_(source), cursorOn_(false), cursorTime_(0), focus_(false), freeze_(0)
{
	this->data_ = default;
	this->onEnter_ = [] (std::string) { return true; };
}


TextEntry::~TextEntry(void)
{
}

void TextEntry::update(int time)
{
	if(this->focus_)
	{
		this->cursorTime_ += time;
		this->stream_.update();
		while(this->stream_.ready())
		{
			if(this->cursorOn_)
			{
				this->data_.pop_back();
			}
			this->cursorOn_ = false;
			this->cursorTime_ = 0;
			char get = this->stream_.get();
			if(get == KeyboardInput::ENTER)
			{
				if(this->onEnter_(this->data_))
					this->data_ = "";
			} else if(get == KeyboardInput::BACKSPACE)
			{
				if(this->data_.size() > 0)
					this->data_.pop_back();
			} else
			{
				if(this->freeze_ == 0 || this->data_.size() < this->freeze_)
				{
					if(this->filter_ == "" || std::find(std::begin(this->filter_), std::end(this->filter_), get) != std::end(this->filter_))
						this->data_.push_back(get);
				}
			}
		}
		if(this->cursorTime_ > 400)
		{
			if(!this->cursorOn_)
				this->data_.push_back('|');
			else
				this->data_.pop_back();
			this->cursorOn_ = !this->cursorOn_;
			this->cursorTime_ = 0;
		}
	} else
	{
		if(this->cursorOn_)
			this->data_.pop_back();
		this->cursorOn_ = false;
		this->cursorTime_ = 0;
	}
}

std::string TextEntry::data() const
{
	return this->data_;
}
unsigned int& TextEntry::freeze(void)
{
	return this->freeze_;
}

bool& TextEntry::focus()
{
	return this->focus_;
}

std::string& TextEntry::filter()
{
	return this->filter_;
}

std::string TextEntry::filter() const
{
	return this->filter_;
}
void TextEntry::setOnEnter(std::function<bool(std::string)> func)
{
	this->onEnter_ = func;
}
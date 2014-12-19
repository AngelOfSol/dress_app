#include "KeyboardInput.h"


KeyboardInput::KeyboardInput(OpenGLHandler& source):source_(source)
{
}


KeyboardInput::~KeyboardInput(void)
{
}

bool KeyboardInput::ready() const
{
	return this->output_.size() > 0;
}
char KeyboardInput::get(void)
{
	auto ret = this->output_.front();
	this->output_.pop();
	return ret;
}
void KeyboardInput::update(void)
{
	auto newKeys = this->source_.keys();
	if(!this->oldKeys_.backspace && newKeys.backspace)
	{
		this->output_.push(KeyboardInput::BACKSPACE);
	} 
	if(!this->oldKeys_.enter && newKeys.enter)
	{
		this->output_.push(KeyboardInput::ENTER);
	}
	if(!this->oldKeys_.space && newKeys.space)
	{
		this->output_.push(' ');
	} 
	if(newKeys.shift)
	{
		for(unsigned char c = 'a'; c <= 'z'; c++)
		{
			if(!this->oldKeys_[c] && newKeys[c])
			{
				// converts to capital
				this->output_.push(c - 'a' + 'A');
			}
		}
	} else
	{
		for(unsigned char c = 'a'; c <= 'z'; c++)
		{
			if(!this->oldKeys_[c] && newKeys[c])
			{
				this->output_.push(c);
			}
		}
	}
	if(newKeys.control)
	{
		for(unsigned char c = 'a'; c <= 'z'; c++)
		{
			if(!this->oldKeys_.otherKeys[c - 'a' + 1] && newKeys.otherKeys[c - 'a' + 1])
			{
				this->output_.push(c);
			}
		}
	} else
	{
		for(unsigned char c = 0; c < 128; c++)
		{
			if(!this->oldKeys_.otherKeys[c] && newKeys.otherKeys[c])
			{
				this->output_.push(c);
			}
		}
	}
	this->oldKeys_ = newKeys;
}
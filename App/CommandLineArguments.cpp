#include "CommandLineArguments.h"


namespace rmg_com
{

CommandLineArguments::CommandLineArguments(void)
{
}


CommandLineArguments::~CommandLineArguments(void)
{
}

CommandLineArguments make_args(int argc, char** argv, bool skipFirst)
{
	CommandLineArguments ret;
	// check to see if they want to keep the first argument or not
	int i = skipFirst ? 1 : 0;


	// combine all the command line arguments into one string

	enum
	{
		NONE,
		OPTION
	} state = NONE;

	std::string option = "";

	for (; i < argc; i++)
	{
		auto input = std::string(argv[i]);

		if (input.size() > 0)
		{
			if (input[0] == '-')
			{
				option = input.substr(1);
				ret.options_[option] = "";
				state = OPTION;
			}
			else
			{
				if (state == NONE)
				{
					ret.arguments_.push_back(input);
				}
				else if (state == OPTION)
				{
					ret.options_[option] = input;
					state = NONE;
				}
			}
		}
	}

	return ret;
}
}
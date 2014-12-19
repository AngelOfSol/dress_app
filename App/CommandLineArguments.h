#pragma once
#include <map>
#include <vector>
#include <string>
namespace rmg_com
{

class CommandLineArguments
{
public:
	~CommandLineArguments(void);
	
	friend CommandLineArguments make_args(int argumentCount, char** argumentValues, bool skipFirst = true);
	//friend CommandLineArguments make_args(const std::string& line);

	const std::map<std::string, std::string> options() const { return this->options_; };
	bool hasOption(std::string key) const { return this->options_.count(key) > 0; };
	const std::vector<std::string>& arguments() const { return this->arguments_; };

	const std::string& operator [](std::string t) const { return this->options_.at(t); };
	const std::string& operator [](const char* t) const { return this->options_.at(t); };


	const std::string& raw() const { return this->raw_; };
private:
	CommandLineArguments(void);
	std::map<std::string, std::string>  options_;
	std::vector<std::string> arguments_;
	std::string raw_;
};

}
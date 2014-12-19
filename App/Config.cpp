#include "Config.h"
#include <fstream>
#include "typedefs.h"
Config::Config()
{
}


Config::Config(std::string filename)
{
	std::ifstream data(filename);
	std::string line;
	while(std::getline(data, line))
	{
		auto keyEnd = std::find(line.begin(), line.end(), '=');
		this->data_[std::string(line.begin(), keyEnd)] = std::string(keyEnd + 1, line.end());
	}
}


Config::~Config(void)
{
}

template <>
std::string Config::as(std::string key) const
{
	return this->data_.at(key);
}
template <>
int Config::as(std::string key) const
{
	return std::stoi(this->data_.at(key));
}
template <>
double Config::as(std::string key) const
{
	return std::stod(this->data_.at(key));
}

template <>
void Config::set(std::string key, std::string value)
{
	this->data_[key] = value;
}
template <>
void Config::set(std::string key, int value)
{
	this->data_[key] = std::to_string(value);
}
template <>
void Config::set(std::string key, double value)
{
	this->data_[key] = std::to_string(value);
}

bool Config::save(std::string filename)
{
	std::ofstream out(filename);
	for(auto pairs : this->data_)
	{
		out << pairs.first << "=" << pairs.second << std::endl;
	}
	return true;
}
bool Config::load(std::string filename, bool overwrite)
{
	if(overwrite)
		this->data_.clear();
	
	std::ifstream data(filename);
	std::string line;
	while(std::getline(data, line))
	{
		auto keyEnd = std::find(line.begin(), line.end(), '=');
		this->data_[std::string(line.begin(), keyEnd)] = std::string(keyEnd + 1, line.end());
	}
	return true;
}

template <>
int_frac Config::as(std::string key) const
{
	return stor<base_int>(this->data_.at(key));
}
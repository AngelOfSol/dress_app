#pragma once
#include <string>
class Pipe
{
public:
	Pipe(std::string pipeLocation);
	~Pipe();
	void write(std::string);
	std::string get();
	bool check();
private:
	std::string const m_location;
	std::string m_data;
};


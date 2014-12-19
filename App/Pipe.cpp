#include "Pipe.h"
#include <fstream>

Pipe::Pipe(std::string location) :m_location(location), m_data("")
{
}


Pipe::~Pipe()
{
}

std::string Pipe::get()
{
	auto ret = this->m_data;
	this->m_data = "";
	return ret;
}
/*
need to edit to remove weird behavior with line breaks in strings.
pip only calls std getline once and then proceeds to DELETE the file afterwards,
making

*/

bool Pipe::check()
{
	// check to see if the pipe file exists

	std::ifstream pipe(this->m_location);
	if (pipe.is_open())
	{
		// make sure the file is good to read from
		// (specifically not currently being written to)
		if (pipe.good())
		{
			std::getline(pipe, this->m_data);

			pipe.close();
			// delete pipe to check again later
			remove(this->m_location.c_str());

			return true;
		}
	}
	return false;
}
void Pipe::write(std::string data)
{
	std::ofstream pipe(this->m_location, std::fstream::out);

	pipe << data << std::endl;

	pipe.flush();
	pipe.close();
}
#include "file_loc.h"


std::string get_path(std::string sub_path)
{
	static std::string directory = "C:\\simulator\\";
	return directory + sub_path;
}
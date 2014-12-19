#pragma once
#include <iostream>
#include <string>
#include "glm_includes.h"

namespace jck
{
namespace file
{
namespace binary
{
	template <typename Data, typename... Extra>
	Data read(std::istream& in, Extra... params)
	{
		Data ref;
		in.read((char*)&ref, sizeof(Data));
		return ref;
	};
	template <typename Data, typename... Extra>
	void read(std::istream& in, Data& ref, Extra... Params)
	{
		ref = read<Data, Extra...>(in, Params...);
	};
	template <typename Data>
	std::ostream& write(std::ostream& out, Data in)
	{
		out.write((char*)&in, sizeof(Data));
		return out;
	}
	
	template <> std::string read<std::string>(std::istream&);
	template <> std::ostream& write<std::string>(std::ostream&, std::string);
	
	template <> glm::vec2 read<glm::vec2>(std::istream&);
	template <> std::ostream& write<glm::vec2>(std::ostream&, glm::vec2);

	template <> glm::vec3 read<glm::vec3>(std::istream&);
	template <> std::ostream& write<glm::vec3>(std::ostream&, glm::vec3);

	template <> glm::vec4 read<glm::vec4>(std::istream&);
	template <> std::ostream& write<glm::vec4>(std::ostream&, glm::vec4);
};
};
};
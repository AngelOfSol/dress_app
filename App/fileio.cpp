#include "fileio.h"
namespace jck
{
namespace file
{
namespace binary
{
	
	
	template <> std::string read<std::string>(std::istream& in)
	{
		std::string ret = "";
		int size = read<int>(in);
		for(int i = 0; i < size; i++)
		{
			ret += read<char>(in);
		}
		return ret;
	}
	template <> std::ostream& write<std::string>(std::ostream& out, std::string in)
	{
		write(out, in.size());
		for(auto c : in)
		{
			write(out, c);
		}
		return out;
	}
	
	template <> glm::vec2 read<glm::vec2>(std::istream& in)
	{
		glm::vec2 ret;
		read(in, ret.x);
		read(in, ret.y);
		return ret;
	}
	template <> std::ostream& write<glm::vec2>(std::ostream& out, glm::vec2 in)
	{
		write(out, in.x);
		write(out, in.y);
		return out;
	}
	
	template <> glm::vec3 read<glm::vec3>(std::istream& in)
	{
		glm::vec3 ret;
		read(in, ret.x);
		read(in, ret.y);
		read(in, ret.z);
		return ret;
	}
	template <> std::ostream& write<glm::vec3>(std::ostream& out, glm::vec3 in)
	{
		write(out, in.x);
		write(out, in.y);
		write(out, in.z);
		return out;
	}

	template <> glm::vec4 read<glm::vec4>(std::istream& in)
	{
		glm::vec4 ret;
		read(in, ret.x);
		read(in, ret.y);
		read(in, ret.z);
		read(in, ret.w);
		return ret;
	}
	template <> std::ostream& write<glm::vec4>(std::ostream& out, glm::vec4 in)
	{
		write(out, in.x);
		write(out, in.y);
		write(out, in.z);
		write(out, in.w);
		return out;
	}

};
};
};
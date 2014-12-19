#pragma once
#include <exception>
#include <ft2build.h>
#include FT_FREETYPE_H
namespace jck
{

class FreeTypeFailure : public std::exception
{
	virtual const char* what() const throw() override
	{
		return "FreeType initialization failed.";
	}
};

/*
Provides an RAII based construct to handle FreeType libraries.
*/
class FreeTypeHandle
{
public:
	// Throws if something went wrong during library intializiation.
	FreeTypeHandle(void) throw(FreeTypeFailure);

	~FreeTypeHandle(void);
	operator const FT_Library&() const;
private:
	FreeTypeHandle(const FreeTypeHandle&);
	FT_Library library_;

};

}
#pragma once
#include "FreeTypeHandle.h"
#include <exception>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
namespace jck
{

class FreeTypeFontFailure : public std::exception
{
	virtual const char* what() const throw() override
	{
		return "FreeType font initialization failed.";
	}
};
/*
Provides an RAII based construct to handle FreeType fonts.
*/
class FreeTypeFontHandle
{
public:
	// Throws an exception when the font is not found or some other error happens.
	FreeTypeFontHandle(const FreeTypeHandle&, std::string fontName, int fontIndex) throw(FreeTypeFontFailure);
	~FreeTypeFontHandle(void);
	operator const FT_Face&() const;
	// accesses properties of FT_Face
	FT_Face operator ->();
private:
	FreeTypeFontHandle(const FreeTypeFontHandle&);
	FT_Face face_;
};

}
#include "FreeTypeFontHandle.h"
namespace jck
{

FreeTypeFontHandle::FreeTypeFontHandle(const FreeTypeHandle& handle, std::string fontName, int fontIndex)
{
	auto error = FT_New_Face(handle, fontName.c_str(), fontIndex, &this->face_);
	if(error)
		throw FreeTypeFontFailure();
}


FreeTypeFontHandle::~FreeTypeFontHandle(void)
{
	FT_Done_Face(this->face_);
}

FreeTypeFontHandle::operator const FT_Face &() const
{
	return this->face_;
}
FT_Face FreeTypeFontHandle::operator->()
{
	return this->face_;
}
}
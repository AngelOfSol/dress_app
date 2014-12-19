#include "FreeTypeHandle.h"

namespace jck
{
FreeTypeHandle::FreeTypeHandle(void)
{
	auto error = FT_Init_FreeType(&this->library_);

	if(error)
		throw FreeTypeFailure();

}


FreeTypeHandle::~FreeTypeHandle(void)
{
	FT_Done_FreeType(this->library_);
}

FreeTypeHandle::operator const FT_Library&() const
{
	return this->library_;
}
}
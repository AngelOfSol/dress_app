#include "Font.h"
#include "FreeTypeFontHandle.h"
#include "FreeTypeHandle.h"
#include <iostream>
#include <algorithm>

namespace jck
{

Font::Font(int width, int height, int length):data_(), width(width), height(height)
{
	// set data vector to have the right size
	// so we can immediately access data points without calling push_back
	this->data_.resize(width * length, std::vector<float>(height, 0.0f));
}


Font::~Font(void)
{
}
	

std::shared_ptr<Font> make_font(std::string fontName, int width, int height)
{
	try
	{
		
		// acquire objects defined by the FreeType library so we can use them
		FreeTypeHandle ftLibrary;
		{
			FreeTypeFontHandle fontFace(ftLibrary, fontName, 0);

			auto error = FT_Set_Pixel_Sizes(fontFace, width, height);

			auto& slot = fontFace->glyph;
		
			// retrieve glyph index from character code 
			auto glyph_index = FT_Get_Char_Index(fontFace, (char)(0));

			// load glyph image into the slot (erase previous one) 
			error = FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_DEFAULT);

			// convert to an anti-aliased bitmap 
			error = FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL );

			auto maxY = 0;
			auto maxX = 0;
			for(int i = 0; i < 128; i++)
			{
				// retrieve glyph index from character code 
				glyph_index = FT_Get_Char_Index(fontFace, (char)(i));

				// load glyph image into the slot (erase previous one) 
				error = FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_DEFAULT);

				// convert to an anti-aliased bitmap 
				error = FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL );
		
				// we only want to work in grayscale fonts
				if(slot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
				{
					int yOffset = slot->bitmap_top - slot->bitmap.rows;
					yOffset = std::max(yOffset, 0);
					int xOffset = slot->bitmap_left;
					maxY = std::max(maxY, slot->bitmap.rows + yOffset);
					maxX = std::max(maxX, slot->bitmap.width + xOffset);
				} // end pixel mode check
			} // end character iteration loop

			auto returnValue = std::make_shared<Font>(maxX, maxY, 128);

			for(int i = 0; i < 128; i++)
			{
				// retrieve glyph index from character code 
				glyph_index = FT_Get_Char_Index(fontFace, (char)(i));

				// load glyph image into the slot (erase previous one) 
				error = FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_DEFAULT);

				// convert to an anti-aliased bitmap 
				error = FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL );
		
				// we only want to work in grayscale fonts
				if(slot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
				{
					int yOffset = slot->bitmap_top - slot->bitmap.rows;
					yOffset = std::max(yOffset, 0);
					int xOffset = static_cast<int>(std::floor(maxX / 2.0f - slot->bitmap.width / 2.0f));

					for(int x = 0; x < slot->bitmap.width; x ++)
					{
						for( int y = 0; y < slot->bitmap.rows; y++)
						{
							// x + i * width is [0, 30] relative to the current character
							returnValue->data_[x + i * maxX + xOffset][y + yOffset] = 
							// x + (...) refers to the place in freetypes glyph structure where the current point lies
							// we normalize it because OpenGL works in [0, 1]
							slot->bitmap.buffer[x + (slot->bitmap.rows - y - 1) * slot->bitmap.width] / 255.0f;
 
						} // end y loop
					} // end x loop
					maxY = std::max(maxY, slot->bitmap.rows + yOffset);
				} // end pixel mode check
			} // end character iteration loop
			return returnValue;
		} // destroy the fontface
	} // since something went wrong, we give the user gets nothing
	catch(const FreeTypeFailure& e)
	{
		return std::shared_ptr<Font>();
	} catch(const FreeTypeFontFailure& e)
	{
		return std::shared_ptr<Font>();
	}
	return std::shared_ptr<Font>();
}
std::shared_ptr<Font> make_font(std::string fontPath, int size)
{
	return make_font(fontPath, size, size);
};

}
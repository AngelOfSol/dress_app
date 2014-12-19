#include "Texture.h"
#include <iostream>

Texture::Texture(unsigned int width, unsigned int height, GLuint id):width(width), height(height), id(id)
{
}


Texture::~Texture(void)
{
}

std::shared_ptr<Texture> make_texture(PixelData data, GLenum textureNumber)
{

	// set up the texture buffers so we can use them
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(textureNumber);
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// set up a raw data array for OpenGL to copy from
	std::vector<GLfloat> raw;
	raw.reserve(data.size() * data[0].size() * Pixel().size());
	
	for(unsigned int y = 0; y < data[0].size(); y++)
	{
		for(unsigned int x = 0; x < data.size(); x++)
		{
			auto& cell = data[x][y];
			raw.insert(raw.end(), cell.begin(), cell.end());
		}
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	// set up our texture parameters so opengl knows how to render them
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// give open gl our data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  data.size(), data[0].size(), 0, GL_RGBA, GL_FLOAT, raw.data());
	
	auto ret = std::make_shared<Texture>(data.size(), data[0].size(), id);
	return ret;
}
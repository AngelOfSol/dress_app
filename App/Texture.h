#pragma once
#include <memory>
#include <vector>
#include <glm\glm.hpp>
#include <functional>
#include <array>
#include "gl_includes.h"

;
typedef std::array<GLfloat, 4> Pixel;
typedef std::vector<std::vector<Pixel>> PixelData;

class Texture
{
public:
	Texture(unsigned int, unsigned int, GLuint);
	~Texture(void);
	const unsigned int width;
	const unsigned int height;
	const GLuint id;
private:
	friend std::shared_ptr<Texture> make_texture(PixelData data, GLenum textureNumber);
};

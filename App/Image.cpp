

#include "Image.h"
#include "fileio.h"
#include "OpenGLHandler.h"
#include "state_gl.h"
#include "file_loc.h"
#include <png.hpp>

Image::Image(OpenGLHandler& gl):GUIElementID(gl)
{
	glGenVertexArrays(1, &this->vao_);
	glGenBuffers(1, &this->vbo_);
}


Image::~Image(void)
{
	glDeleteBuffers(1, &this->vbo_);
	glDeleteVertexArrays(1, &this->vao_);
}
glm::vec2& Image::size(void)
{
	return this->size_;
}
glm::vec2 Image::size(void) const
{
	return this->size_;
}
std::string Image::imagePath(void) const
{
	return this->imagePath_;
}

void Image::update(int)
{
}
void Image::draw(void) const
{
	GLStateOn<GL_TEXTURE_2D, GL_BLEND> on;
	GLStateOff<GL_DEPTH_TEST> off;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture_->id);
	this->gl_.useProgram("image");
	glm::mat4 transform = glm::ortho(0.0f, this->gl_.windowSize().x, 0.0f, this->gl_.windowSize().y);
	transform = glm::translate(transform, glm::vec3(this->pos(), 0));
	transform = glm::scale(transform, glm::vec3(this->size(), 1));
	this->gl_.set(glUniformMatrix4fv, "transformMat", 1, false, &transform[0][0]);
	this->gl_.set(glUniform1i, "textureBuffer", 0);
	glBindVertexArray(this->vao_);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
std::shared_ptr<Image> make_image(OpenGLHandler& gl, glm::vec2 pos, glm::vec2 size, std::string imagePath)
{
	if(!Image::programInitialized)
	{
		gl.loadShader("image_vs", get_path("Shaders/imageVS.glsl"), GL_VERTEX_SHADER);
		gl.loadShader("image_gs", get_path("Shaders/imageGS.glsl"), GL_GEOMETRY_SHADER);
		gl.loadShader("image_fs", get_path("Shaders/imageFS.glsl"), GL_FRAGMENT_SHADER);

		gl.createProgram("image");

		gl.attachShader("image", "image_vs");
		gl.attachShader("image", "image_gs");
		gl.attachShader("image", "image_fs");

		gl.compileProgram("image");

		Image::programInitialized = true;
	}
	
	auto ret = std::make_shared<Image>(gl);

	ret->pos_ = pos;
	ret->size_ = size;
	ret->imagePath_ = imagePath;

	{
		PixelData data;
		{
			png::image<png::rgba_pixel> imageFile(imagePath.c_str());
		
			data.resize(imageFile.get_width());
			for(auto& row : data)
			{
				row.resize(imageFile.get_height());
			}
			for(int x = 0; x < imageFile.get_width(); x++)
			{
				for(int y = 0; y < imageFile.get_height(); y++)
				{
					auto imageY = imageFile.get_height() - y - 1;
					data[x][y][0] = imageFile[imageY][x].red;
					data[x][y][1] = imageFile[imageY][x].green;
					data[x][y][2] = imageFile[imageY][x].blue;
					data[x][y][3] = imageFile[imageY][x].alpha;
				}
			}
		}

		ret->texture_ = make_texture(data, GL_TEXTURE0);
	}

	glBindVertexArray(ret->vao_);
	glBindBuffer(GL_ARRAY_BUFFER, ret->vbo_);

	std::vector<GLfloat> raw;
	raw.push_back(0);
	raw.push_back(0);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * raw.size(), raw.data(), GL_STATIC_DRAW);

	// let opengl know the first attribute is the position at 2 floats long starting at 0
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// enable the attributes
	
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return ret;
}
using namespace jck::file::binary;
std::shared_ptr<Image> read_image(std::istream& in, OpenGLHandler& gl)
{
	glm::vec2 pos;
	glm::vec2 size;
	std::string imagePath;
	read(in, pos);
	read(in, size);
	read(in, imagePath);

	return make_image(gl, pos, size, imagePath);
}
std::ostream& write(std::ostream& out, const Image& ref)
{
	write(out, ref.pos());
	write(out, ref.size());
	write(out, ref.imagePath());

	return out;
}

bool Image::programInitialized = false;
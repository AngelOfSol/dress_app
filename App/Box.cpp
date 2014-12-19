#include "Box.h"
#include "glm_includes.h"
#include <vector>
#include "OpenGLHandler.h"
#include <string>
#include "state_gl.h"
#include "file_loc.h"
Box::Box(OpenGLHandler& gl):GUIElementID(gl)
{
	glGenVertexArrays(1, &this->vao_);
	glGenBuffers(1, &this->vbo_);
}


Box::~Box(void)
{
	glDeleteVertexArrays(1, &this->vao_);
	glDeleteBuffers(1, &this->vbo_);
}

void Box::update(int)
{
}

void Box::draw(void) const
{
	GLStateOff<GL_DEPTH_TEST, GL_STENCIL_TEST> off;
	this->gl_.useProgram("box");
	this->gl_.set(glUniform4fv, "pColor", 1, &this->color_[0]);
	glm::mat4 transform = glm::ortho(0.0f, this->gl_.windowSize().x, 0.0f, this->gl_.windowSize().y);
	transform = glm::translate(transform, glm::vec3(this->pos(), 0));
	transform = glm::scale(transform, glm::vec3(this->size(), 1));
	this->gl_.set(glUniformMatrix4fv, "transformMat", 1, false, &transform[0][0]);

	glBindVertexArray(this->vao_);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
glm::vec4& Box::color(void)
{
	return this->color_;
}

glm::vec2& Box::size(void)
{
	return this->size_;
}


glm::vec4 Box::color(void) const
{
	return this->color_;
}

glm::vec2 Box::size(void) const
{
	return this->size_;
}


std::shared_ptr<Box> make_box(OpenGLHandler& gl, glm::vec2 pos, glm::vec2 size)
{
	if(!Box::programInitialized)
	{

		gl.loadShader("boxVS", get_path("Shaders/boxVS.glsl"), GL_VERTEX_SHADER);
		gl.loadShader("boxGS", get_path("Shaders/boxGS.glsl"), GL_GEOMETRY_SHADER);
		gl.loadShader("boxFS", get_path("Shaders/boxFS.glsl"), GL_FRAGMENT_SHADER);
	
		gl.createProgram("box");
		gl.attachShader("box", "boxVS");
		gl.attachShader("box", "boxGS");
		gl.attachShader("box", "boxFS");
		gl.compileProgram("box");

		Box::programInitialized = true;
	}
	auto ret = std::make_shared<Box>(gl);
	ret->pos_ = pos;
	ret->size_ = size;

	glBindVertexArray(ret->vao_);
	glBindBuffer(GL_ARRAY_BUFFER, ret->vbo_);

	std::vector<GLfloat> raw;
	raw.resize(4);

	raw[0] = 0;
	raw[1] = 0;
	raw[2] = 1;
	raw[3] = 1;
	
	// tell opengl to copy the data

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * raw.size(), raw.data(), GL_STATIC_DRAW);

	// let opengl know the first attribute is the position at 2 floats long starting at 0

	// let opengl know the first attribute is the position at 3 floats long starting at 0
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// enable the attributes
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	return ret;
}
/*
template <> std::shared_ptr<Box> jck::file::binary::read<std::shared_ptr<Box>>(std::istream& in, OpenGLHandler& handler)
{
	glm::vec2 pos;
	glm::vec2 size;
	read(in, pos);
	read(in, size);
	return make_box(handler, pos, size);
}
template <> std::ostream& jck::file::binary::write(std::ostream& out, const Box& ref)
{
	write(out, ref.pos());
	write(out, ref.size());
	return out;
}
*/

bool Box::programInitialized = false;
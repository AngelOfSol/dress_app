#pragma once
#include "glm_includes.h"
#include "gl_includes.h"
#include <memory>
#include "fileio.h"
#include "GUIElementID.h"
class Box
	: public GUIElementID<Box>
{
public:
	~Box(void);
	Box(OpenGLHandler&);
	void update(int) override;
	void draw(void) const override;
	glm::vec4& color(void);
	glm::vec4 color(void) const;
	glm::vec2& size(void);
	glm::vec2 size(void) const;
private:
	Box(const Box&);
	glm::vec4 color_;
	GLuint vao_;
	GLuint vbo_;
	glm::vec2 size_;
	static bool programInitialized;

	friend std::shared_ptr<Box> make_box(OpenGLHandler& gl, glm::vec2 pos, glm::vec2 size);
};
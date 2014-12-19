#pragma once
#include "gl_includes.h"
#include <memory>
#include "Texture.h"
#include "guielementid.h"
class Image :
	public GUIElementID<Image>
{
public:
	Image(OpenGLHandler&);
	~Image(void);
	void update(int) override;
	void draw(void) const override;
	glm::vec2& size(void);
	glm::vec2 size(void) const;
	std::string imagePath(void) const;
private:
	GLuint vao_;
	GLuint vbo_;
	std::shared_ptr<Texture> texture_;
	glm::vec2 size_;
	std::string imagePath_;
	static bool programInitialized;
	friend std::shared_ptr<Image> make_image(OpenGLHandler& gl, glm::vec2 pos, glm::vec2 size, std::string imagePath);
};

std::shared_ptr<Image> read_image(std::istream&, OpenGLHandler&);
std::ostream& write(std::ostream&, const Image&);
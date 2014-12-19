#include "OpenGLHandler.h"
#include <fstream>
#include <glm\matrix.hpp>
#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\transform.hpp>
#include "Scene.h"
#include "state_gl.h"
#include "file_loc.h"

using okey = OpenGLHandler::Keys::state;
static void update(OpenGLHandler::Keys::state& s)
{
	s = okey(s & okey::ON);
}


OpenGLHandler::OpenGLHandler()
{
}

void OpenGLHandler::init(glm::vec2 winSize)
{
	this->windowSize_ = winSize;

	this->prev_ = 0;

	this->loadShader("textf", get_path("Shaders/textFS.glsl"), GL_FRAGMENT_SHADER);
	this->loadShader("textg", get_path("Shaders/textGS.glsl"), GL_GEOMETRY_SHADER);
	this->loadShader("textv", get_path("Shaders/textVS.glsl"), GL_VERTEX_SHADER);

	this->createProgram("text");
	this->attachShader("text", "textf");
	this->attachShader("text", "textg");
	this->attachShader("text", "textv");
	this->compileProgram("text");

	this->closed_ = false;
}

OpenGLHandler::~OpenGLHandler(void)
{
}

bool OpenGLHandler::loadShader(std::string shaderName, std::string filename, GLuint shaderType)
{
	std::string data = "";
	std::string line;
	std::ifstream inFile(filename);

	while (std::getline(inFile, line))
	{
		data += line + '\n';
	}

	data = "#extension GL_ARB_explicit_attrib_location : require\n" + data;

	data = "#version " + std::to_string(this->shaderVersion) + '\n' + data;
	auto shader = make_shader(data, shaderType);

	if (shader && this->shaderList_.count(shaderName) == 0)
	{
		this->shaderList_[shaderName] = shader;
		return true;
	}
	return false;
}
bool OpenGLHandler::createProgram(std::string programName)
{

	if (this->programList_.count(programName) == 0)
	{
		this->programList_[programName] = std::make_shared<ShaderProgram>();
		return true;
	}
	return false;
}

void OpenGLHandler::attachShader(std::string programName, std::string shaderName)
{
	this->programList_[programName]->addShader(this->shaderList_[shaderName]);
}

bool OpenGLHandler::compileProgram(std::string programName)
{
	this->programList_[programName]->compile();
	return GL_FALSE == this->programList_[programName]->getCompileStatus();
}

bool OpenGLHandler::compileAllPrograms()
{
	for (auto progPair : this->programList_)
	{
		progPair.second->compile();
		if (GL_FALSE == progPair.second->getCompileStatus())
			return false;
	}
	return true;
}
void OpenGLHandler::useProgram(std::string programName)
{
	glUseProgram(this->programList_[programName]->getIndex());
	this->currentProgram_ = programName;
}

bool OpenGLHandler::loadFont(std::string fontPath, std::string fontName, int size)
{
	auto newFont = jck::make_font(fontPath, size);

	auto& fontData = newFont->data();

	PixelData raw;
	raw.resize(fontData.size());
	for (auto& row : raw)
	{
		row.resize(fontData[0].size());
	}
	Pixel temp;
	for (unsigned int x = 0; x < fontData.size(); x++)
	{
		auto rowSize = fontData[0].size();
		for (unsigned int y = 0; y < rowSize; y++)
		{
			for (auto& val : temp)
			{
				val = fontData[x][y];
			}
			raw[x][y] = temp;
		}
	}
	auto fontlayout = make_texture(raw, GL_TEXTURE0);
	if (fontlayout)
	{
		this->fontList_[fontName].texture = fontlayout;
		this->fontList_[fontName].width = newFont->width;
		this->fontList_[fontName].height = newFont->height;
	}
	return (bool)fontlayout;

}
void OpenGLHandler::useFont(std::string fontName, float size)
{
	this->currentFont_ = fontName;

	FontData& data = this->fontList_[fontName];

	GLStateOn<GL_TEXTURE_2D> textures;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data.texture->id);
	glm::mat4 transform = glm::ortho(0.0f, this->windowSize().x, 0.0f, this->windowSize().y);

	this->useProgram("text");
	this->set(glUniformMatrix4fv, "transformMat", 1, false, &transform[0][0]);
	this->set(glUniform1i, "fontBuffer", 0);
	this->set(glUniform1f, "textWidth", this->getFont(fontName).width);
	this->set(glUniform1f, "textHeight", this->getFont(fontName).height);
	this->set(glUniform1f, "textureWidth", static_cast<GLfloat>(this->getFont(fontName).width) / static_cast<GLfloat>(this->getFont(fontName).texture->width));
	this->set(glUniform1f, "textureHeight", 1.0f);



}
auto OpenGLHandler::getFont(std::string name) const -> const FontData&
{
	return this->fontList_.at(name);
}
void OpenGLHandler::write(std::string data, glm::vec2 pos, bool right)
{
	GLStateOn<GL_TEXTURE_2D, GL_BLEND> on;
	GLStateOff<GL_DEPTH_TEST> off;

	GLuint vao = 0;
	GLuint vbo = 0;

	pos.x = std::floor(pos.x);
	pos.y = std::floor(pos.y);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	std::vector<GLfloat> raw;
	raw.reserve(data.size() * 5);
	int i = 0;
	int offset = (right) ? (0) : (data.size() * -this->fontList_[this->currentFont_].width);
	for (auto c : data)
	{
		raw.push_back(pos.x + i * this->fontList_[this->currentFont_].width + offset);

		raw.push_back(pos.y);
		raw.push_back(0.0f);
		raw.push_back((GLfloat)c / 128.0f);
		raw.push_back(0.0f);
		i++;
	}


	// tell opengl to copy the data

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * raw.size(), raw.data(), GL_STATIC_DRAW);

	// let opengl know the first attribute is the position at 3 floats long starting at 0

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	// let opengl know the second attribute is texel coordinates at 2 float long starting at the 3rd index

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// enable the attributes

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// draw

	glBindVertexArray(vao);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_POINTS, 0, data.size());
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

std::string OpenGLHandler::currentProgram() const
{
	return this->currentProgram_;
}

void OpenGLHandler::update(void)
{

	int newTime = glutGet(GLUT_ELAPSED_TIME);
	auto duration = newTime - this->prev_;
	if (this->prev_ == 0)
		duration = 0;
	this->prev_ = newTime;
	auto test1 = this->currentScene_ >= this->scenes_.size();
	auto test2 = !this->scenes_[this->currentScene_];
	if (test1 || test2)
	{
		this->close();
	}
	else
	{
		auto newScene = this->scenes_[this->currentScene_]->update(duration);
		if (newScene != this->currentScene_)
		{
			this->currentScene_ = newScene;
			auto test1 = this->currentScene_ >= this->scenes_.size();
			auto test2 = !this->scenes_[this->currentScene_];
			if (test1 || test2)
			{
				this->close();
			}
			this->scenes_[this->currentScene_]->init();
		}
		if (!this->closed_)
			glutPostRedisplay();
	}
	for (auto& key : this->keys_.letterKeys)
	{
		::update(key);
	}
	for (auto& key : this->keys_.otherKeys)
	{
		::update(key);
	}
	::update(this->keys_.alt);
	::update(this->keys_.shift);
	::update(this->keys_.control);
	::update(this->keys_.space);
	::update(this->keys_.backspace);
	::update(this->keys_.enter);
	::update(this->keys_.up);
	::update(this->keys_.down);
	::update(this->keys_.left);
	::update(this->keys_.right);
}
scene_id& OpenGLHandler::currentScene()
{
	return this->currentScene_;
}
const scene_id& OpenGLHandler::currentScene() const
{
	return this->currentScene_;
}
void OpenGLHandler::draw(void)
{
	this->scenes_[this->currentScene_]->draw();
	glutSwapBuffers();
}

void OpenGLHandler::reshape(int w, int h)
{
	this->windowSize_ = glm::vec2(w, h);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

glm::vec2 OpenGLHandler::windowSize() const
{
	return this->windowSize_;
}
void OpenGLHandler::mouseDown(int x, int y)
{
	this->mouse_.x = (float)x;
	this->mouse_.y = (float)y;
}
void OpenGLHandler::mouseUp(int x, int y)
{
	this->mouse_.x = (float)x;
	this->mouse_.y = (float)y;
}
void OpenGLHandler::mouseChange(int button, int state, int x, int y)
{
	this->mouse_.x = (float)x;
	this->mouse_.y = (float)y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		this->mouse_.left = state == GLUT_DOWN;
		break;
	case GLUT_RIGHT_BUTTON:
		this->mouse_.right = state == GLUT_DOWN;
		break;
	case GLUT_MIDDLE_BUTTON:
		this->mouse_.middle = state == GLUT_DOWN;
		break;
	};
}
auto OpenGLHandler::mouse() const -> Mouse
{
	auto ret = this->mouse_;
	ret.y -= this->windowSize().y;
	ret.y *= -1;
	return ret;
}

void OpenGLHandler::asciiKeyDown(unsigned char key, int x, int y)
{

	if (key >= 'a' && key <= 'z')
		this->keys_.letterKeys[key - 'a'] = Keys::JUST_PRESSED;
	else if (key >= 'A' && key <= 'Z')
		this->keys_.letterKeys[key - 'A'] = Keys::JUST_PRESSED;
	else if (key == (char)32)
		this->keys_.space = Keys::JUST_PRESSED;
	else if (key == (char)13)
		this->keys_.enter = Keys::JUST_PRESSED;
	else if (key == (char)8)
		this->keys_.backspace = Keys::JUST_PRESSED;

	this->keys_.otherKeys[key] = Keys::JUST_PRESSED;
}
void OpenGLHandler::asciiKeyUp(unsigned char key, int x, int y)
{
	if (key >= 'a' && key <= 'z')
		this->keys_.letterKeys[key - 'a'] = Keys::OFF;
	else if (key >= 'A' && key <= 'Z')
		this->keys_.letterKeys[key - 'A'] = Keys::OFF;
	else if (key == (char)32)
		this->keys_.space = Keys::OFF;
	else if (key == (char)13)
		this->keys_.enter = Keys::OFF;
	else if (key == (char)8)
		this->keys_.backspace = Keys::OFF;

	this->keys_.otherKeys[key] = Keys::OFF;
}
void OpenGLHandler::keyDown(int key, int x, int y)
{

	if (key == GLUT_KEY_UP)
	{
		this->keys_.up = Keys::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		this->keys_.down = Keys::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		this->keys_.left = Keys::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		this->keys_.right = Keys::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		this->keys_.shift = Keys::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_ALT_L || key == GLUT_KEY_ALT_R)
	{
		this->keys_.alt = Keys::JUST_PRESSED;
	}
	else if (key == GLUT_KEY_CTRL_L || key == GLUT_KEY_CTRL_R)
	{
		this->keys_.control = Keys::JUST_PRESSED;
	}
}

void OpenGLHandler::keyUp(int key, int x, int y)
{

	if (key == GLUT_KEY_UP)
	{
		this->keys_.up = Keys::OFF;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		this->keys_.down = Keys::OFF;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		this->keys_.left = Keys::OFF;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		this->keys_.right = Keys::OFF;
	}
	else if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
	{
		this->keys_.shift = Keys::OFF;
	}
	else if (key == GLUT_KEY_ALT_L || key == GLUT_KEY_ALT_R)
	{
		this->keys_.alt = Keys::OFF;
	}
	else if (key == GLUT_KEY_CTRL_L || key == GLUT_KEY_CTRL_R)
	{
		this->keys_.control = Keys::OFF;
	}
}

void OpenGLHandler::addScene(std::shared_ptr<Scene> scene)
{
	auto id = scene->getID();
	if (this->scenes_.size() <= id)
	{
		this->scenes_.resize(id + 1);
	}
	this->scenes_[id] = scene;
}

auto OpenGLHandler::keys() const -> const Keys&
{
	return this->keys_;
}
void OpenGLHandler::close(void)
{
	this->closed_ = true;
	glutLeaveMainLoop();
	glutExit();
	std::exit(0);
}
#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include "gl_includes.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Font.h"
#include <glm\glm.hpp>
#include "Scene.h"

class OpenGLHandler
{
public:
	struct FontData
	{
		std::shared_ptr<Texture> texture;
		int width;
		int height;
	};
	struct Mouse
		: public glm::vec2
	{
		bool left;
		bool middle;
		bool right;
		Mouse(): left(false), right(false), middle(false)
		{
		};
	};
	struct Keys
	{
		enum state
		{
			OFF = 0x0
			, ON = 0x1
			, JUST_PRESSED = 0x3
		} ;
		std::vector<state> letterKeys;
		std::vector<state> otherKeys;
		
		state operator [](char index) const
		{
			return this->letterKeys.at(index - 'a');
		}
		state space;
		state enter;
		state backspace;
		state shift;
		state control;
		state alt;

		state up;
		state down;
		state left;
		state right;

		Keys() :letterKeys(26), space(OFF), enter(OFF), shift(OFF), control(OFF), alt(OFF), backspace(OFF), otherKeys(256)
		{
		}
	};
	OpenGLHandler(void); // done
	~OpenGLHandler(void); // done

	void init(glm::vec2 winSize);

	bool loadShader(std::string shaderName, std::string filename, GLuint shaderType); // done
	void attachShader(std::string programName, std::string shaderName); // done

	bool createProgram(std::string programName); // done
	void useProgram(std::string programName); // done
	bool compileProgram(std::string programName); // done
	bool compileAllPrograms(); // done
	template <class Func, typename... Params>
	void set(Func call, const std::string& name, Params... params) // done
	{
		if(this->currentProgram_ != "")
		{
			this->programList_[this->currentProgram_]->setUniform(call, name, params...);
		}
	}

	bool loadFont(std::string filename, std::string fontName, int size); // done
	void useFont(std::string fontName, float size = 1.0f); // done
	const FontData& getFont(std::string name) const; // done
	void write(std::string data, glm::vec2 pos, bool rightOrient = true); // done
	
	glm::vec2 windowSize() const; // done
	Mouse mouse() const; // done
	const Keys& keys() const; // done
	scene_id& currentScene();
	const scene_id& currentScene() const;
	std::string currentProgram() const; // done


	void update(void); // done
	void draw(void); // done
	
	void asciiKeyUp(unsigned char, int, int); // done
	void asciiKeyDown(unsigned char, int, int); // done
	void keyDown(int, int, int); // done
	void keyUp(int, int, int); // done

	void reshape(int, int); // done

	void mouseDown(int, int); // done
	void mouseUp(int, int); // done
	void mouseChange(int, int, int, int); // done

	void addScene(std::shared_ptr<Scene>); // done

	void close(void);
	
	int shaderVersion;
private:
	
	std::vector<std::shared_ptr<Scene>> scenes_;
	std::map<std::string, std::shared_ptr<Shader>> shaderList_;
	std::map<std::string, std::shared_ptr<ShaderProgram>> programList_;
	std::map<std::string, FontData> fontList_;
	std::map<std::string, std::shared_ptr<Texture>> textureList_;
	std::string currentProgram_;
	std::string currentFont_;
	scene_id currentScene_;
	glm::vec2 windowSize_;
	Keys keys_;
	Mouse mouse_;
	int prev_;
	bool closed_;
};


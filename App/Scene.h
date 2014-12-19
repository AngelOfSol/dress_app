#pragma once
#include <string>

typedef unsigned int scene_id;

class OpenGLHandler;
class Scene
{
public:
	Scene(OpenGLHandler&);
	~Scene(void);
	virtual void init(void) = 0;
	virtual scene_id update(int) = 0;
	virtual void draw(void) = 0;
	virtual scene_id getID(void) const = 0;
protected:
	OpenGLHandler& gl_;
	static int getNewID();
	static int idCounter;
};


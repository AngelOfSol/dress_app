#include "Scene.h"


Scene::Scene(OpenGLHandler& arg0):gl_(arg0)
{
}


Scene::~Scene(void)
{
}

int Scene::getNewID()
{
	Scene::idCounter++;
	int ret = Scene::idCounter;
	return ret;
}
int Scene::idCounter = 0;
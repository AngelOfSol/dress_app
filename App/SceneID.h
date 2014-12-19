#pragma once
#include "scene.h"
template <class T>
class SceneID :
	public Scene
{
public:
	SceneID(OpenGLHandler& parent):Scene(parent)
	{
	}
	~SceneID(void)
	{
	}
	scene_id getID(void) const override
	{
		return SceneID::id;
	}
	static const scene_id id;
private:

};
template<class T>
const scene_id SceneID<T>::id = Scene::getNewID();

#pragma once
#include "sceneid.h"
#include "GUIElement.h"
#include "LabeledTextbox.h"
#include "TextButton.h"
#include "DropDownMenu.h"
#include <vector>
#include "cylinderdata.h"
#include "KeyboardInput.h"
class SetupScreen :
	public SceneID<SetupScreen>
{
public:
	SetupScreen(OpenGLHandler& parent, cylinder_data& output);
	~SetupScreen(void);
	void init(void) override;
	scene_id update(int) override;
	void draw(void) override;
	void save(std::string filePath);
	void load(std::string filePath);
private:
	enum
	{
		NONE,
		POSITIONING
	} state_;
	std::vector<std::shared_ptr<GUIElement>> guiElements_;

	LabeledTextbox::ptr radius;
	LabeledTextbox::ptr length;
	TextButton::ptr button;
	DropDownMenu::ptr unitType;
	DropDownMenu::ptr toolTip;
	DropDownMenu::ptr side;
	LabeledTextbox::ptr toolHomeX;
	LabeledTextbox::ptr toolHomeY;

	KeyboardInput keyboard_;
	int curBox_;
	bool done_;
	std::string mouse_;
	cylinder_data& output_;
};


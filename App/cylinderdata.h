#pragma once
#include <string>
#include "glm_includes.h"
#include "Vec2D.h"
#include "Pipe.h"
#include "Config.h"
#include <Windows.h>
enum class origin_side
{
	invalid,
	exit,
	operator_
};
enum class units
{
	invalid
	, inches
	, millimeters
};
enum class tools
{
	invalid
	, triangle
	, rounded
	, rectangle
};
struct cylinder_data
{
	vec2d size;
	vec2d toolHome;
	vec2d offset;
	units unitType;
	tools toolType;
	origin_side side;
	std::string fileName;
	Pipe * pipe;
	Config * config;
	HWND hwnd;
	bool acr;
	cylinder_data() :size(), toolHome(), unitType(units::invalid), toolType(tools::invalid), fileName("test2.htm"), pipe(nullptr)
	{
	}
};
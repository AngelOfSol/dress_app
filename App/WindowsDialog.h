#pragma once
#include <string>
#include <Windows.h>
std::string GetFileName(const std::string & prompt );

struct FileDialog
{
	static HWND handle;
};
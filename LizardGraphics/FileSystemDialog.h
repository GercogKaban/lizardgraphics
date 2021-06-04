#pragma once

#include <filesystem>
//#include "imgui.h"

class FileSystemDialog
{
public:

	static std::string startDialog(const char* currentPath);

private:
	static std::string prevPath;
};
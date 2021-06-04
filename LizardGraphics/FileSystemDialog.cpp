#include "FileSystemDialog.h"
#include "imgui.h"
#include "LLogger.h"

using namespace std::filesystem;

std::string FileSystemDialog::prevPath = current_path().generic_string();
std::string FileSystemDialog::startDialog(const char* currentPath)
{
	LOG_CALL
	try
	{
		if (ImGui::Button("<-"))
			return std::string(currentPath).substr(0, std::string(currentPath).find_last_of("/"));
		for (auto& p : std::filesystem::directory_iterator(currentPath))
		{
			auto path = p.path().generic_string();
			auto subStr = path.substr(path.find_last_of("/") + 1, path.size() - path.find_last_of("/") - 1);
			if (ImGui::Button(subStr.data()))
			{
				prevPath = currentPath;
				return path;
			}
		}
		if (ImGui::Button("<-"))
			return std::string(currentPath).substr(0, std::string(currentPath).find_last_of("/"));
	}
	catch (std::exception& ex)
	{
		PRINTLN(ex.what()); 
	}
	return currentPath;
}

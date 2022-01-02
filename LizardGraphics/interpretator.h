#pragma once
#include <unordered_map>
#include "LModel.h"
//#include <functional>

namespace LGraphics
{
	class LApp;
	class Interpretator
	{
	public:
		Interpretator(LApp* app);
		~Interpretator(){}

		auto getLastErrorCode() const { return lastErrorCode; }
		const std::string& getErrorText(int errorCode) const;
		bool exec(const std::string& command);

	private:

		LApp* app;

		enum class CommandTypes
		{
			WRONG_COMMAND,
			FUNCTION_CALL,
			METHOD_CALL,
			CONSTRUCTION,
		};

		enum class CommandErrors
		{
			_NO_ERROR,
			COMMAND_NAME_ERROR,
			FUNCTION_NAME_ERROR,
		};

		const std::string errors[2] =
		{
			"no error",
			"wrong command name"
			"wrong function name",
		};

		const std::unordered_map<std::string, CommandTypes> commandTypes =
		{
			{"call_func",CommandTypes::FUNCTION_CALL},
			{"call_method",CommandTypes::METHOD_CALL},
			{"construct",CommandTypes::CONSTRUCTION},
		};

		std::tuple<CommandTypes,std::string,std::vector<std::string>> parseCommand(const std::string& command) const;
		std::string parseFunctionName(const std::string& command) const;
		std::vector<std::string> parseFunctionArgs(const std::string& command) const;

		void callFunction(const std::string& func, const std::vector<std::string>& args);
		CommandErrors lastErrorCode = CommandErrors::_NO_ERROR;

		static LModel::ModelResource toModelResource(const std::string& val);
		static LImage::ImageResource toImageResource(const std::string& val);

		static glm::vec2 tovec2(const std::string& val);
		static glm::vec3 tovec3(const std::string& val);
		static glm::vec4 tovec4(const std::string& val);
		static glm::mat2 tomat2(const std::string& val);
		static glm::mat3 tomat3(const std::string& val);
		static glm::mat4 tomat4(const std::string& val);

		static int toint(const std::string& val);
		static bool tobool(const std::string& val);
		static size_t tosize_t(const std::string& val);
		static float tofloat(const std::string& val);

		// ___GENERATED CODE START___ DON'T MODIFY THIS LINE (OR YOU'L DIE IN AGONY!!!!)
static std::unordered_map<std::string, std::function<void(LGraphics::LApp* app,const std::vector<std::string>&)>> appFunctionPointers;
// ___GENERATED CODE END___   DON'T MODIFY THIS LINE (OR YOU'L DIE IN AGONY!!!!)
	};
}
#include "interpretator.h"
#include "LApp.h"
#include "LModel.h"
#include "LImagedShape.h"
//#include "functions_data.h"
#include "LPlane.h"
#include "LTorus.h"
#include "LCone.h"
#include "LCylinder.h"
#include "LCube.h"
#include "LSphere.h"
#include "LIcosphere.h"

// ___GENERATED CODE START___ DON'T MODIFY THIS LINE (OR YOU'L DIE IN AGONY!!!!)
std::unordered_map<std::string, std::function<void(LGraphics::LApp* app, const std::vector<std::string>&)>> LGraphics::Interpretator::appFunctionPointers = 
{{"setParallaxSelfShading(bool)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setParallaxSelfShading(tobool(args[0]));}},
{"setMatrices(mat4,mat4)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setMatrices(tomat4(args[0]),tomat4(args[1]));}},
{"setTesselation(bool)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setTesselation(tobool(args[0]));}},
{"setSleepTime(size_t)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setSleepTime(tosize_t(args[0]));}},
{"setClearColor(vec4)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setClearColor(tovec4(args[0]));}},
{"setCameraPos(vec3)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setCameraPos(tovec3(args[0]));}},
{"setCameraFront(vec3)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setCameraFront(tovec3(args[0]));}},
{"setCameraUp(vec3)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setCameraUp(tovec3(args[0]));}},
{"setViewRadius(float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setViewRadius(tofloat(args[0]));}},
{"setFov(float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setFov(tofloat(args[0]));}},
{"setYaw(float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setYaw(tofloat(args[0]));}},
{"setPitch(float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){app->setPitch(tofloat(args[0]));}},
{"LCone(LApp*,ImageResource)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){new LCone(app,toImageResource(args[1]));}},
{"LCube(LApp*,ImageResource)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){new LCube(app,toImageResource(args[1]));}},
{"LCylinder(LApp*,ImageResource)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){new LCylinder(app,toImageResource(args[1]));}},
{"LIcosphere(LApp*,ImageResource)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){new LIcosphere(app,toImageResource(args[1]));}},
{"LModel(LApp*,ModelResource,bool)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){new LModel(app,toModelResource(args[1]),tobool(args[2]));}},
{"setModel(mat4)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->setModel(tomat4(args[2]));else {if(toint(args[0]) == L_PLANE)((LPlane*)app->primitives[toint(args[0])][toint(args[1])])->setModel(tomat4(args[2]));else if(toint(args[0]) == L_CUBE)((LCube*)app->primitives[toint(args[0])][toint(args[1])])->setModel(tomat4(args[2]));else if(toint(args[0]) == L_SPHERE)((LSphere*)app->primitives[toint(args[0])][toint(args[1])])->setModel(tomat4(args[2]));else if(toint(args[0]) == L_ICOSPHERE)((LIcosphere*)app->primitives[toint(args[0])][toint(args[1])])->setModel(tomat4(args[2]));else if(toint(args[0]) == L_CONE)((LCone*)app->primitives[toint(args[0])][toint(args[1])])->setModel(tomat4(args[2]));else if(toint(args[0]) == L_TORUS)((LTorus*)app->primitives[toint(args[0])][toint(args[1])])->setModel(tomat4(args[2]));else if(toint(args[0]) == L_CYLINDER)((LCylinder*)app->primitives[toint(args[0])][toint(args[1])])->setModel(tomat4(args[2]));}}},
{"scale(float,float,float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->scale(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else {if(toint(args[0]) == L_PLANE)((LPlane*)app->primitives[toint(args[0])][toint(args[1])])->scale(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_CUBE)((LCube*)app->primitives[toint(args[0])][toint(args[1])])->scale(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_SPHERE)((LSphere*)app->primitives[toint(args[0])][toint(args[1])])->scale(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_ICOSPHERE)((LIcosphere*)app->primitives[toint(args[0])][toint(args[1])])->scale(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_CONE)((LCone*)app->primitives[toint(args[0])][toint(args[1])])->scale(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_TORUS)((LTorus*)app->primitives[toint(args[0])][toint(args[1])])->scale(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_CYLINDER)((LCylinder*)app->primitives[toint(args[0])][toint(args[1])])->scale(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));}}},
{"move(float,float,float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->move(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else {if(toint(args[0]) == L_PLANE)((LPlane*)app->primitives[toint(args[0])][toint(args[1])])->move(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_CUBE)((LCube*)app->primitives[toint(args[0])][toint(args[1])])->move(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_SPHERE)((LSphere*)app->primitives[toint(args[0])][toint(args[1])])->move(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_ICOSPHERE)((LIcosphere*)app->primitives[toint(args[0])][toint(args[1])])->move(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_CONE)((LCone*)app->primitives[toint(args[0])][toint(args[1])])->move(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_TORUS)((LTorus*)app->primitives[toint(args[0])][toint(args[1])])->move(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));else if(toint(args[0]) == L_CYLINDER)((LCylinder*)app->primitives[toint(args[0])][toint(args[1])])->move(tofloat(args[2]),tofloat(args[3]),tofloat(args[4]));}}},
{"rotateX(float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->rotateX(tofloat(args[2]));else {if(toint(args[0]) == L_PLANE)((LPlane*)app->primitives[toint(args[0])][toint(args[1])])->rotateX(tofloat(args[2]));else if(toint(args[0]) == L_CUBE)((LCube*)app->primitives[toint(args[0])][toint(args[1])])->rotateX(tofloat(args[2]));else if(toint(args[0]) == L_SPHERE)((LSphere*)app->primitives[toint(args[0])][toint(args[1])])->rotateX(tofloat(args[2]));else if(toint(args[0]) == L_ICOSPHERE)((LIcosphere*)app->primitives[toint(args[0])][toint(args[1])])->rotateX(tofloat(args[2]));else if(toint(args[0]) == L_CONE)((LCone*)app->primitives[toint(args[0])][toint(args[1])])->rotateX(tofloat(args[2]));else if(toint(args[0]) == L_TORUS)((LTorus*)app->primitives[toint(args[0])][toint(args[1])])->rotateX(tofloat(args[2]));else if(toint(args[0]) == L_CYLINDER)((LCylinder*)app->primitives[toint(args[0])][toint(args[1])])->rotateX(tofloat(args[2]));}}},
{"rotateY(float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->rotateY(tofloat(args[2]));else {if(toint(args[0]) == L_PLANE)((LPlane*)app->primitives[toint(args[0])][toint(args[1])])->rotateY(tofloat(args[2]));else if(toint(args[0]) == L_CUBE)((LCube*)app->primitives[toint(args[0])][toint(args[1])])->rotateY(tofloat(args[2]));else if(toint(args[0]) == L_SPHERE)((LSphere*)app->primitives[toint(args[0])][toint(args[1])])->rotateY(tofloat(args[2]));else if(toint(args[0]) == L_ICOSPHERE)((LIcosphere*)app->primitives[toint(args[0])][toint(args[1])])->rotateY(tofloat(args[2]));else if(toint(args[0]) == L_CONE)((LCone*)app->primitives[toint(args[0])][toint(args[1])])->rotateY(tofloat(args[2]));else if(toint(args[0]) == L_TORUS)((LTorus*)app->primitives[toint(args[0])][toint(args[1])])->rotateY(tofloat(args[2]));else if(toint(args[0]) == L_CYLINDER)((LCylinder*)app->primitives[toint(args[0])][toint(args[1])])->rotateY(tofloat(args[2]));}}},
{"rotateZ(float)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->rotateZ(tofloat(args[2]));else {if(toint(args[0]) == L_PLANE)((LPlane*)app->primitives[toint(args[0])][toint(args[1])])->rotateZ(tofloat(args[2]));else if(toint(args[0]) == L_CUBE)((LCube*)app->primitives[toint(args[0])][toint(args[1])])->rotateZ(tofloat(args[2]));else if(toint(args[0]) == L_SPHERE)((LSphere*)app->primitives[toint(args[0])][toint(args[1])])->rotateZ(tofloat(args[2]));else if(toint(args[0]) == L_ICOSPHERE)((LIcosphere*)app->primitives[toint(args[0])][toint(args[1])])->rotateZ(tofloat(args[2]));else if(toint(args[0]) == L_CONE)((LCone*)app->primitives[toint(args[0])][toint(args[1])])->rotateZ(tofloat(args[2]));else if(toint(args[0]) == L_TORUS)((LTorus*)app->primitives[toint(args[0])][toint(args[1])])->rotateZ(tofloat(args[2]));else if(toint(args[0]) == L_CYLINDER)((LCylinder*)app->primitives[toint(args[0])][toint(args[1])])->rotateZ(tofloat(args[2]));}}},
{"setNormalMapping(bool)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->setNormalMapping(tobool(args[2]));else {if(toint(args[0]) == L_PLANE)((LPlane*)app->primitives[toint(args[0])][toint(args[1])])->setNormalMapping(tobool(args[2]));else if(toint(args[0]) == L_CUBE)((LCube*)app->primitives[toint(args[0])][toint(args[1])])->setNormalMapping(tobool(args[2]));else if(toint(args[0]) == L_SPHERE)((LSphere*)app->primitives[toint(args[0])][toint(args[1])])->setNormalMapping(tobool(args[2]));else if(toint(args[0]) == L_ICOSPHERE)((LIcosphere*)app->primitives[toint(args[0])][toint(args[1])])->setNormalMapping(tobool(args[2]));else if(toint(args[0]) == L_CONE)((LCone*)app->primitives[toint(args[0])][toint(args[1])])->setNormalMapping(tobool(args[2]));else if(toint(args[0]) == L_TORUS)((LTorus*)app->primitives[toint(args[0])][toint(args[1])])->setNormalMapping(tobool(args[2]));else if(toint(args[0]) == L_CYLINDER)((LCylinder*)app->primitives[toint(args[0])][toint(args[1])])->setNormalMapping(tobool(args[2]));}}},
{"setParallaxMapping(bool)",[&](LGraphics::LApp* app, const std::vector<std::string>& args){if (toint(args[0]) == L_MODEL) app->models[toint(args[1])]->setParallaxMapping(tobool(args[2]));else {if(toint(args[0]) == L_PLANE)((LPlane*)app->primitives[toint(args[0])][toint(args[1])])->setParallaxMapping(tobool(args[2]));else if(toint(args[0]) == L_CUBE)((LCube*)app->primitives[toint(args[0])][toint(args[1])])->setParallaxMapping(tobool(args[2]));else if(toint(args[0]) == L_SPHERE)((LSphere*)app->primitives[toint(args[0])][toint(args[1])])->setParallaxMapping(tobool(args[2]));else if(toint(args[0]) == L_ICOSPHERE)((LIcosphere*)app->primitives[toint(args[0])][toint(args[1])])->setParallaxMapping(tobool(args[2]));else if(toint(args[0]) == L_CONE)((LCone*)app->primitives[toint(args[0])][toint(args[1])])->setParallaxMapping(tobool(args[2]));else if(toint(args[0]) == L_TORUS)((LTorus*)app->primitives[toint(args[0])][toint(args[1])])->setParallaxMapping(tobool(args[2]));else if(toint(args[0]) == L_CYLINDER)((LCylinder*)app->primitives[toint(args[0])][toint(args[1])])->setParallaxMapping(tobool(args[2]));}}},
};
// ___GENERATED CODE END___   DON'T MODIFY THIS LINE (OR YOU'L DIE IN AGONY!!!!)

LGraphics::Interpretator::Interpretator(LApp* app)
{
	this->app = app;
}

const std::string& LGraphics::Interpretator::getErrorText(int errorCode) const
{
	return errors[errorCode];
}

//"call_method method(vec3) %classId %objId {1.0,2.0,0.0}"
//"call_func func1(int,bool) 5,True"
//"construct LModel(...)"//

bool LGraphics::Interpretator::exec(const std::string& command)
{
	const auto res = parseCommand(command);
	callFunction(std::get<1>(res), std::get<2>(res));
	return true;
}

std::tuple<LGraphics::Interpretator::CommandTypes, std::string, std::vector<std::string>> 
LGraphics::Interpretator::parseCommand(const std::string& command) const
{
	std::string str = strip(command);
	auto commTypeEnd = std::find(str.begin(), str.end(), SPACE);
	const auto commandType = commandTypes.find(str.substr(0, getPosByIt(str.begin(), commTypeEnd)));
	if (commandType == commandTypes.end())
		return { LGraphics::Interpretator::CommandTypes::WRONG_COMMAND,
		errors[(int)LGraphics::Interpretator::CommandTypes::WRONG_COMMAND],std::vector<std::string>() };

	const std::string func = lstrip(str.substr(getPosByIt(str.begin(), commTypeEnd)));

	auto argsTypesStart = getPosByIt(func.begin(), std::find(func.begin(), func.end(), '(') + 1);
	auto argsTypesEnd = getPosByIt(func.begin(), std::find(func.begin(), func.end(), ')'));

	std::string args = lstrip(func.substr(argsTypesEnd + 1));
	const auto argTokens = splitToTokens(args, ',');

	auto funcPrototype = func.substr(0, argsTypesEnd + 1);
	const auto noSpaceFunc = std::string(funcPrototype.begin(), std::remove(funcPrototype.begin(), funcPrototype.end(), SPACE));
	return { commandType->second, noSpaceFunc, argTokens };
}

std::string LGraphics::Interpretator::parseFunctionName(const std::string& command) const
{
	return std::string();
}

std::vector<std::string> LGraphics::Interpretator::parseFunctionArgs(const std::string& command) const
{
	return std::vector<std::string>();
}

void LGraphics::Interpretator::callFunction(const std::string& func, const std::vector<std::string>& args)
{
	appFunctionPointers[func](app,args);
}

LGraphics::LModel::ModelResource LGraphics::Interpretator::toModelResource(const std::string& val)
{
	return { val };
}

LGraphics::LImage::ImageResource LGraphics::Interpretator::toImageResource(const std::string & val)
{
	return { val };
}

glm::vec2 LGraphics::Interpretator::tovec2(const std::string& val)
{
	const auto tokens = splitToTokens(val, ' ');
	return glm::vec2(std::stof(tokens[0]), std::stof(tokens[1]));
}

glm::vec3 LGraphics::Interpretator::tovec3(const std::string& val)
{
	const auto tokens = splitToTokens(val, ' ');
	return glm::vec3(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]));
}

glm::vec4 LGraphics::Interpretator::tovec4(const std::string& val)
{
	const auto tokens = splitToTokens(val, ' ');
	return glm::vec4(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

glm::mat2 LGraphics::Interpretator::tomat2(const std::string& val)
{
	const auto tokens = splitToTokens(val, ' ');
	return glm::mat2(std::stof(tokens[0]), std::stof(tokens[1]), 
		             std::stof(tokens[2]), std::stof(tokens[3]));
}

glm::mat3 LGraphics::Interpretator::tomat3(const std::string& val)
{
	const auto tokens = splitToTokens(val, ' ');
	return glm::mat3(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]),
		             std::stof(tokens[3]), std::stof(tokens[4]), std::stof(tokens[5]),
			         std::stof(tokens[6]), std::stof(tokens[7]), std::stof(tokens[8]));
}

glm::mat4 LGraphics::Interpretator::tomat4(const std::string& val)
{
	const auto tokens = splitToTokens(val, ' ');
	return glm::mat4(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]),
		             std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6]), std::stof(tokens[7]),
		             std::stof(tokens[8]), std::stof(tokens[9]), std::stof(tokens[10]), std::stof(tokens[11]),
		             std::stof(tokens[12]), std::stof(tokens[13]), std::stof(tokens[14]), std::stof(tokens[15]));
}

int LGraphics::Interpretator::toint(const std::string& val)
{
	return std::stoi(val);
}

bool LGraphics::Interpretator::tobool(const std::string& val)
{
	return stob(val);
}

size_t LGraphics::Interpretator::tosize_t(const std::string& val)
{
	return std::stoull(val);
}

float LGraphics::Interpretator::tofloat(const std::string& val)
{
	return std::stof(val);
}

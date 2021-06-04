#include <fstream>
#include <map>
#include <vector>

#include "LModel.h"
#include "CodeGen.h"

void CodeGen::generateCode(const char* outPath, LGraphics::LApp* app, const char* appName)
{
    std::vector<std::string> includeFilesVector;
    std::string includeFiles = "#include \"LizardGraphics/LApp.h\"\n";
    std::string code = "namespace LGraphics{\nvoid genWidgets(LGraphics::LApp* app)\n{\n";

    auto genData = [&](LGraphics::LWidget* obj, std::string name)
    {
        //if (std::find(notGenerateList.begin(), notGenerateList.end(), obj) != notGenerateList.end())
        //    continue;
        if (std::find(includeFilesVector.begin(), includeFilesVector.end(), obj->getObjectType()) == includeFilesVector.end())
        {
            includeFilesVector.push_back(obj->getObjectType());
            includeFiles += "#include \"" + std::string("LizardGraphics/") + obj->getObjectType() + ".h\"\n";
        }
        code += "auto " + name + " = new " + getConstructor(obj, appName) + '\n' +
            name + "->move(" + getMoveData(obj) + ");\n" +
            name + "->scale(" + getScaleData(obj) + ");\n" + name + "->color(glm::vec3(" + getColorData(obj) + "));\n" +
            name + "->setRotate(glm::mat4(" + getRotateData(obj) + "));\n" +
            name + "->setName(\"" + obj->getName() + "\");\n";

        includeFiles += "\n";
    };

    auto models = app->getModels();
    for (size_t i = 0; i < models.size(); ++i)
    {
        auto& obj = models[i];
        auto name = "model" + std::to_string(i + 1);
        genData(obj, name);
    }

    auto rectangles = app->getPrimitives();
    for (size_t i = 0; i < rectangles.size(); ++i)
    {
        auto& obj = rectangles[i];
        auto name = "rect" + std::to_string(i + 1);
        genData((LGraphics::LWidget*)obj, name);
    }

    code += "\n}\n}\n";
    writeToFile(outPath, std::string(includeFiles) + code);
}

void CodeGen::writeToFile(const char* outPath, const std::string & code)
{
    std::ofstream out;
    out.open(outPath);
    out << code;
    out.close();
}

//void CodeGen::notGenerateData(LGraphics::LWidget* w)
//{
//    notGenerateList.push_back(w);
//}

std::string CodeGen::getMoveData(LGraphics::LWidget* w)
{
    return std::string(std::to_string(w->getMove().x)+", " + std::to_string(w->getMove().y) + ", " + std::to_string(w->getMove().z));
}

std::string CodeGen::getScaleData(LGraphics::LWidget* w)
{
    return std::string(std::to_string(w->getScale().x) + ", " + std::to_string(w->getScale().y) + ", " + std::to_string(w->getScale().z));
}

std::string CodeGen::getColorData(LGraphics::LWidget* w)
{
    return std::string(std::to_string(w->getColor().x) + ", " + std::to_string(w->getColor().y) + ", " + std::to_string(w->getColor().z));
}

std::string CodeGen::getTextureData(LGraphics::LWidget* w)
{
    return w->getTexturePath();
}

std::string CodeGen::getHiddenData(LGraphics::LWidget* w)
{
    return std::to_string(w->isHidden());
}

std::string CodeGen::getTransparencyData(LGraphics::LWidget* w)
{
    return std::to_string(w->getTransparency());
}

std::string CodeGen::getRotateData(LGraphics::LWidget* w)
{
    std::string res;
    auto rotate = w->getRotate();
    for (size_t i = 0; i < rotate.length(); ++i)
        for (size_t j = 0; j < rotate[i].length(); ++j)
            res+= std::to_string(rotate[i][j]) + ",";
    res.pop_back();
    return res;
}

std::string CodeGen::getConstructor(LGraphics::LWidget* w, const char* appName)
{
    return w->getObjectType() == std::string("LWRectangle") ?
        std::string(w->getObjectType()) + "(" + appName + ",\"" + w->getTexturePath() + "\");"
        // need to add normalMap in ctor
        : std::string(w->getObjectType()) + "(" + appName + ",\"" + 
        ((LGraphics::LModel*)w)->getModelPath() + "\",\"" + w->getTexturePath() + "\");";
}

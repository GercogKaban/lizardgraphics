#include <fstream>
#include <map>
#include <vector>

#include "CodeGen.h"

void CodeGen::generateCode(const char* outPath, LGraphics::LApp* app, const char* appName)
{
    std::vector<std::string> includeFilesVector;
    std::string includeFiles = "#include \"LApp.h\"\n";
    std::string mainCode = "namespace LGraphics{\nvoid genWidgets(LGraphics::LApp* app)\n{\n";

    for (size_t i = 0; i < app->getObjects()->size(); ++i)
    {
        auto obj = (*app->getObjects())[i];
        if (std::find(notGenerateList.begin(), notGenerateList.end(), obj) != notGenerateList.end())
            continue;
        if (std::find(includeFilesVector.begin(), includeFilesVector.end(), obj->getObjectType()) == includeFilesVector.end())
        {
            includeFilesVector.push_back(obj->getObjectType());
            includeFiles += "#include \"" + std::string(obj->getObjectType()) + ".h\"\n";
        }
        auto name = "obj" + std::to_string(i + 1);
        mainCode += "auto " + name + " = new " + std::string(obj->getObjectType()) +
            "(" + appName + ",\"" + getTextureData(obj)+"\");\n" + name + "->move(" + getMoveData(obj) + ");\n" +
            name + "->scale(" + getScaleData(obj) + ");\n" + name + "->color(fvect3(" + getColorData(obj) + "));\n"+
            name + "->setHidden(" + getHiddenData(obj) + ");\n" + name + "->setWidgetMovability(" + getWidgetMovability(obj) + ");\n" +
            name + "->transparency("+ getTransparencyData(obj)+ ");\n"+ name + "->setRotate(glm::mat4(" + getRotateData(obj) + "));\n" + 
            name + "->setLabel(\""+ getLabelData(obj) + "\");\n" + name + "->setIsometricView("+ getIsometricData(obj) + ");\n\n";
    }
    includeFiles += "\n";
    mainCode += "}}\n";
    writeToFile(outPath, includeFiles + mainCode);
}

void CodeGen::notGenerateData(LGraphics::LWidget* w)
{
    notGenerateList.push_back(w);
}

void CodeGen::writeToFile(const char* outPath, std::string & code)
{
    std::ofstream out;
    out.open(outPath);
    out << code;
    out.close();
}

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

std::string CodeGen::getIsometricData(LGraphics::LWidget * w)
{
    return std::to_string(w->isIsometricView());
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

std::string CodeGen::getLabelData(LGraphics::LWidget * w)
{
    return w->getLabel();
}

std::string CodeGen::getWidgetMovability(LGraphics::LWidget * w)
{
    return std::to_string(w->getWidgetMovability());
}

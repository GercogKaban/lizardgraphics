#pragma once

#include <string>
#include "LApp.h"

class CodeGen
{
public:

    CodeGen(){}
    static void generateCode(const char* outPath, LGraphics::LApp* app, const char* appName);
    
private:

    static void writeToFile(const char* outPath, const std::string& code);

    //static void notGenerateData(LGraphics::LWidget* w);
    static std::string getMoveData(LGraphics::LWidget* w);
    static std::string getScaleData(LGraphics::LWidget* w);
    static std::string getColorData(LGraphics::LWidget* w);
    static std::string getTextureData(LGraphics::LWidget* w);
    static std::string getHiddenData(LGraphics::LWidget* w);
    static std::string getTransparencyData(LGraphics::LWidget* w);
    static std::string getRotateData(LGraphics::LWidget* w);
    static std::string getConstructor(LGraphics::LWidget* w, const char* appName);

    //static std::vector<LGraphics::LWidget*> notGenerateList;
};

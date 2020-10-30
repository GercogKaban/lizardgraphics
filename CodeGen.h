#pragma once

#include <string>
#include "LApp.h"

class CodeGen
{
public:

    CodeGen(){}
    void generateCode(const char* outPath, LGraphics::LApp* app, const char* appName);
    void notGenerateData(LGraphics::LWidget* w);
    
private:

    void writeToFile(const char* outPath, std::string& code);

    std::string getMoveData(LGraphics::LWidget* w);
    std::string getScaleData(LGraphics::LWidget* w);
    std::string getColorData(LGraphics::LWidget* w);
    std::string getTextureData(LGraphics::LWidget* w);
    std::string getHiddenData(LGraphics::LWidget* w);
    std::string getIsometricData(LGraphics::LWidget* w);
    std::string getTransparencyData(LGraphics::LWidget* w);
    std::string getRotateData(LGraphics::LWidget* w);
    std::string getLabelData(LGraphics::LWidget* w);
    std::string getWidgetMovability(LGraphics::LWidget* w);

    std::vector<LGraphics::LWidget*> notGenerateList;
    
};
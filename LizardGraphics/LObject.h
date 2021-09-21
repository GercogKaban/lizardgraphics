#pragma once
#define MEGATEXTURE_LG

namespace LGraphics
{
    class LObject
    {
    public:
        virtual const char* getObjectType() const { return "LObject"; }
    protected:
        LObject() {}
    };
}


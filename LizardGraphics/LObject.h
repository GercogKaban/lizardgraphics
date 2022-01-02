#pragma once
#define MEGATEXTURE_LG
#define METAGEN_CLASS
#define GEN_METADATA

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


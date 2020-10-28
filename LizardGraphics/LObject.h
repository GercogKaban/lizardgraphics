#pragma once
namespace LGraphics
{
    class LObject
    {
    public:
        virtual const char* getObjectType() { return "LObject"; }
    protected:
        LObject() {}
    };
}


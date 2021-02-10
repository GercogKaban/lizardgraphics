#pragma once
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


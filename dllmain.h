#pragma once

LApp* app;
extern "C"
{
#ifdef _MSC_VER 
    __declspec(dllexport) 
#endif
      void onCreate();
      void loop();
}
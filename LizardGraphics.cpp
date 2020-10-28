//#include "pch.h"
//#include "LizardGraphics.h"
//#include "textures.h"

//namespace LGraphics
//{
//    std::vector<std::string>LError::errors;
//    size_t LError::lastErrorNum;
//    LShaders::Shader* interfaceShader = nullptr;
//    LShaders::Shader* sceneObjectShader = nullptr;
//
//    namespace
//    {
//        LBuffer* baseRectangleBuffer = new LRectangleBuffer();
//        LTextBuffer* baseRectangleTextBuffer = new LTextBuffer();
//    }
//
//
//    //LWRectangle::LWRectangle(LRectangleShape * rectangle)
////    : LRectangleShapeI(rectangle)
////{
////    rectangle->setShader(sceneObjectShader);
////}
//
//    bool isPointInPolygon(int npol, float* xp, float* yp, fvect2 p)
//    {
//        bool c = false;
//        for (int i = 0, j = npol - 1; i < npol; j = i++)
//        {
//            if ((
//                (yp[i] < yp[j]) && (yp[i] <= p.y) && (p.y <= yp[j]) &&
//                ((yp[j] - yp[i]) * (p.x - xp[i]) > (xp[j] - xp[i]) * (p.y - yp[i]))
//                ) || (
//                (yp[i] > yp[j]) && (yp[j] <= p.y) && (p.y <= yp[i]) &&
//                    ((yp[j] - yp[i]) * (p.x - xp[i]) < (xp[j] - xp[i]) * (p.y - yp[i]))
//                    ))
//                c = !c;
//        }
//        return c;
//    }
//
//
//    fvect2 pointOnDisplayToGlCoords(fvect2 displaySize, fvect2 point)
//    {
//        point.y = displaySize.y - point.y;
//        return point / displaySize - (fvect2(1.0f, 1.0f) - point / displaySize);
//    }
//}
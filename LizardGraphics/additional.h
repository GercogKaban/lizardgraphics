#pragma once

#include "vectors.h"

namespace LGraphics
{
    bool isPointInPolygon(int npol, float* xp, float* yp, fvect2 p);
    fvect2 pointOnDisplayToGlCoords(fvect2 displaySize, fvect2 point);
}
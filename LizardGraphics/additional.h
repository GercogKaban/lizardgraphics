﻿#pragma once

#include <tuple>
#include "include/glm/glm.hpp"

#define FOR(i,n) for (size_t j = i; j < n; j++)

namespace LGraphics
{
    template <typename T, typename ... Args>
    auto make_array(const T t, const Args ... args)
    {
        return std::array<T, sizeof...(Args) + 1>({ t, args... });
    }

    bool isPointInPolygon(int npol, float* xp, float* yp, glm::vec2 p);
    glm::vec2 pointOnScreenToGlCoords(glm::vec2 screenSize, glm::vec2 point);
    glm::vec2 glCoordsToScreenCoords(glm::vec2 screenSize, glm::vec2 val);

    float xOnScreenToGlCoord(glm::vec2 screenSize, float x);
    float yOnScreenToGlCoord(glm::vec2 screenSize, float y);

    float xGlCoordToScreenCoord(glm::vec2 screenSize, float x);
    float yGlCoordToScreenCoord(glm::vec2 screenSize, float y);
    /*std::tuple<glm::vec2, glm::vec2> getMoveAndSizeByCoords(glm::vec2 topLeft, 
        glm::vec2 topRight, glm::vec2 bottomRight, glm::vec2 bottomLeft);*/

    glm::vec3 rgbToFloat(unsigned char r, unsigned char g, unsigned char b);

    size_t getPowerTwoAlign(size_t size);
}
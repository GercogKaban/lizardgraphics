#include "pch.h"
#include "additional.h"

namespace LGraphics
{
    bool isPointInPolygon(int npol, float* xp, float* yp, glm::vec2 p)
    {
        bool c = false;
        for (int i = 0, j = npol - 1; i < npol; j = i++)
            if ((
                (yp[i] < yp[j]) && (yp[i] <= p.y) && (p.y <= yp[j]) &&
                ((yp[j] - yp[i]) * (p.x - xp[i]) > (xp[j] - xp[i]) * (p.y - yp[i]))
                ) || (
                (yp[i] > yp[j]) && (yp[j] <= p.y) && (p.y <= yp[i]) &&
                    ((yp[j] - yp[i]) * (p.x - xp[i]) < (xp[j] - xp[i]) * (p.y - yp[i]))
                    ))
                c = !c;
        return c;
    }

    glm::vec2 pointOnScreenToGlCoords(glm::vec2 screenSize, glm::vec2 point)
    {
        point.y = screenSize.y - point.y;
        return point / screenSize - (glm::vec2(1.0f, 1.0f) - point / screenSize);
    }

    glm::vec2 glCoordsToScreenCoords(glm::vec2 screenSize, glm::vec2 val)
    {
        return ((val + glm::vec2(1.0f, 1.0f))*screenSize) / 2.0f;
    }

    float xOnScreenToGlCoord(glm::vec2 screenSize, float x)
    {
        return x / screenSize.x - (1.0f - x / screenSize.x);
    }

    float yOnScreenToGlCoord(glm::vec2 screenSize, float y)
    {
        y = screenSize.y - y;
        return y / screenSize.y - (1.0f - y / screenSize.y);
    }

    float xGlCoordToScreenCoord(glm::vec2 screenSize, float x)
    {
        return ((x + 1.0f)*screenSize.x) / 2.0f;
    }

    float yGlCoordToScreenCoord(glm::vec2 screenSize, float y)
    {
        return ((y + 1.0f)*screenSize.y) / 2.0f;
    }

    //std::tuple<glm::vec2, glm::vec2> getMoveAndSizeByCoords(glm::vec2 topLeft,
    //    glm::vec2 topRight, glm::vec2 bottomRight, glm::vec2 bottomLeft)
    //{
    //    return std::tuple<glm::vec2, glm::vec2>(
    //        { (topLeft.x - topRight.x) / 2.0f, (topLeft.y - bottomLeft.y) / 2.0f , 1.0f }, 
    //        glm::vec3(topLeft.x, topLeft.y, 0.0f) - glm::vec3(-0.5f, 0.5f, 0.0f));
    //}

    glm::vec3 rgbToFloat(unsigned char r, unsigned char g, unsigned char b)
    {
        return{ (float)r / (float)sizeof(unsigned char), (float)g / (float)sizeof(unsigned char), (float)b / (float)sizeof(unsigned char) };
    }

    bool isExists(const std::string& path)
    {
        return std::filesystem::exists(path);
    }

    size_t getPowerTwoAlign(size_t size)
    {
        size_t i = 2;
        while (size > i)
            i <<= 2;
        return i;
    }

    void findMeshTrianglesAdjacency(float* buff, size_t size, std::vector<float>& out)
    {

    }
}
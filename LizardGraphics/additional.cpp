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

    glm::mat4 assimpToGLM(aiMatrix4x4 matrix)
    {
        return glm::mat4(
            { matrix.a1, matrix.a2, matrix.a3,matrix.a4 },
            { matrix.b1, matrix.b2, matrix.b3, matrix.b4 },
            { matrix.c1, matrix.c2, matrix.c3, matrix.c4 },
            { matrix.d1, matrix.d2, matrix.d3, matrix.d4 });
    }

    glm::mat3 assimpToGLM(aiMatrix3x3 matrix)
    {
        return glm::mat3(
            { matrix.a1, matrix.a2, matrix.a3},
            { matrix.b1, matrix.b2, matrix.b3},
            { matrix.c1, matrix.c2, matrix.c3});
    }

    glm::vec2 assimpToGLM(aiVector2D vec)
    {
        return glm::vec2(vec.x,vec.y);
    }

    glm::vec3 assimpToGLM(aiVector3D vec)
    {
        return glm::vec3(vec.x,vec.y,vec.z);
    }

    glm::quat assimpToGLM(aiQuaternion quat)
    {
        return glm::quat(quat.x, quat.y, quat.z, quat.w);
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
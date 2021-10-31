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

    glm::mat4 assimpToGLM(const aiMatrix4x4& from)
    {
        return glm::transpose(glm::make_mat4(&from.a1));
        //glm::mat4 to;
        //to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        //to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        //to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        //to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        //return to;
    }

    glm::mat3 assimpToGLM(const aiMatrix3x3& from)
    {
        return glm::transpose(glm::make_mat3(&from.a1));
        //glm::mat3 to;
        //to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3;
        //to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3;
        //to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3;
        //return to;
    }

    glm::vec2 assimpToGLM(const aiVector2D& vec)
    {
        return glm::vec2(vec.x,vec.y);
    }

    glm::vec3 assimpToGLM(const aiVector3D& vec)
    {
        return glm::vec3(vec.x,vec.y,vec.z);
    }

    glm::quat assimpToGLM(const aiQuaternion& quat)
    {
        return glm::quat(quat.w, quat.x, quat.y, quat.z);
    }

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
}
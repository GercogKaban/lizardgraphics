#pragma once

#include <tuple>
#include <filesystem>
#include <assimp/scene.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/quaternion.hpp"

#define FOR(var,j,n) for (size_t var = j; i < n; i++)

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

    glm::mat4 assimpToGLM(const aiMatrix4x4& matrix);
    glm::mat3 assimpToGLM(const aiMatrix3x3& matrix);
    glm::vec2 assimpToGLM(const aiVector2D& vec);
    glm::vec3 assimpToGLM(const aiVector3D& vec);
    glm::quat assimpToGLM(const aiQuaternion& quat);
    /*std::tuple<glm::vec2, glm::vec2> getMoveAndSizeByCoords(glm::vec2 topLeft, 
        glm::vec2 topRight, glm::vec2 bottomRight, glm::vec2 bottomLeft);*/

    glm::vec3 rgbToFloat(unsigned char r, unsigned char g, unsigned char b);

    bool isExists(const std::string& path);

    size_t getPowerTwoAlign(size_t size);

    std::string extractFileNameFromPath(const std::filesystem::path& path);

    std::string lstrip(const std::string& str);
    std::string rstrip(const std::string& str);
    std::string strip(const std::string& str);

    std::vector<std::string> splitToTokens(const std::string& str, char delim);

    bool stob(const std::string& str);

    template <typename I>
    I findNumEntry(I begin, I end, std::decay_t<decltype(*begin)> elem, size_t num)
    {
        size_t counter = 0;
        while (counter < num && begin != end)
            if (*(begin++) == elem) counter++;
        return counter == num ? --begin : end;
    }


    template <typename I>
    size_t getPosByIt(I begin, I it)
    {
        return ((size_t)&*it - (size_t)&*begin) / sizeof(std::decay_t<decltype(*begin)>);
    }

    
}
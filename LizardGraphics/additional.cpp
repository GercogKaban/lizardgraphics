#include "pch.h"
#include "additional.h"
#include "enums.h"

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
    }

    glm::mat3 assimpToGLM(const aiMatrix3x3& from)
    {
        return glm::transpose(glm::make_mat3(&from.a1));
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

    std::string extractFileNameFromPath(const std::filesystem::path& path)
    {
        std::string pathCopy = path.generic_string();
        std::replace(pathCopy.begin(), pathCopy.end(), '\\', '/');
        auto fileNamePos = pathCopy.rfind("/");     
        return fileNamePos!= std::string::npos
            ? pathCopy.substr(fileNamePos + 1, pathCopy.size() - fileNamePos + 1)
            : pathCopy;
    }

    std::string lstrip(const std::string& str)
    {
        std::string res = str;
        size_t counter = 0;
        while (str[counter++] == SPACE);
        res.erase(0, counter - 1);
        return res;
    }

    std::string rstrip(const std::string& str)
    {
        std::string res = str;
        size_t counter = 0;
        while (str[str.size() - 1 - counter++] == SPACE);
        res.erase(str.size()-counter + 1);
        return res;
    }

    std::string strip(const std::string& str)
    {
        return lstrip(rstrip(str));
    }

    std::vector<std::string> splitToTokens(const std::string& str, char delim)
    {
        std::vector<std::string> tokens;
        std::stringstream stream(str);
        std::string token;
        while (getline(stream, token, delim))
            tokens.push_back(token);
        return tokens;
    }

    bool stob(const std::string& str)
    {
        return isdigit(str[0]) ? std::stoi(str) : str == "true" ? true : false;
    }
}
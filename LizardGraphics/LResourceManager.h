#pragma once

#include <map>

#include "include/stb/stb_image.h"

#ifdef OPENGL
#include "include/GLEW/glew.h"
#endif

namespace LGraphics
{
    class LApp;
    class LResourceManager
    {

#ifdef OPENGL
    public:

        static GLuint loadTexture(const char* path);
        static GLuint loadTexture(const unsigned char* bytes, size_t size);

    private:

        static GLuint initTexture(unsigned char* bytes, int width, int height);
        static std::map<std::string, GLuint> textures;
#endif // OPENGL

#ifdef VULKAN

    public:

        friend LApp;
        static VkImageView loadTexture(const char* path, int desiredChannel = 4);
        static VkImageView loadTexture(unsigned char * bytes, size_t size, const char* name, int desiredChannel = 4);

        static void setApp(LApp* app) { LResourceManager::app = app; }

    private:

        static VkImageView createImageView(unsigned char* pixels, int texWidth, int texHeight, int texChannels, const char* byteTexture);
        
        static LApp* app;
        static std::map<std::string, std::tuple<VkImageView,VkImage,VkDeviceMemory>> textures;
#endif

    };
}


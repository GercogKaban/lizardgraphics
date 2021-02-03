#pragma once

#include <unordered_map>

#include "include/stb/stb_image.h"
#include "include/tinyobjloader/tiny_obj_loader.h"

#ifdef OPENGL
#include "include/GLEW/glew.h"
#endif

namespace LGraphics
{
    class LApp;
    class LModel;
    class LModelBuffer;

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

        struct ModelData
        {
            LModelBuffer* buffer;
            VkImageView* textures;
        };

        friend LApp;
        friend LModel;

        static VkImageView loadTexture(const char* path, int desiredChannel = 4);
        static VkImageView loadTexture(unsigned char * bytes, size_t size, const char* name, int desiredChannel = 4);

        static void loadModel(LModel* model, const char* modelPath, bool debugInfo = false);

        static void setApp(LApp* app) { LResourceManager::app = app; }

    private:

        static VkImageView createImageView(unsigned char* pixels, int texWidth, int texHeight, int texChannels, const char* byteTexture);
        
        static LApp* app;
        static std::unordered_map<std::string, std::tuple<VkImageView,VkImage,VkDeviceMemory>*> textures;
        static std::unordered_map<std::string, ModelData*> models;
#endif

    };
}


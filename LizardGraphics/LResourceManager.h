#pragma once

#include <unordered_map>
#include <variant>
#include <memory>

#include "include/stb/stb_image.h"
#include "include/tinyobjloader/tiny_obj_loader.h"
//#include "VulkanMemoryAllocator/include/vk_mem_alloc.h"
#include "include/GLEW/glew.h"
#include "LApp.h"

//#define OGL_IMG LResourceManager::OpenGLImage
//#define VK_IMG LResourceManager::OpenGLImage
//

#define GET_DIFFUSE(name) ((LResourceManager::OpenGLImage*)LResourceManager::textures[name].textures.get())->diffuse.texture
#define GET_NORMALS(name)  ((LResourceManager::OpenGLImage*)LResourceManager::textures[name].textures.get())->normals.texture
//#define GET_DIFFUSE_PTR(name) &GET_DIFFUSE(name)
//#define GET_NORMALS_PTR(name) &GET_NORMALS(name)

//#define GET_DIFFUSE_VK(name) ((LResourceManager::VulkanImage*)LResourceManager::textures[name].textures)->diffuse
//#define GET_NORMALS_VK(name)  ((LResourceManager::VulkanImage*)LResourceManager::textures[name].textures)->normals
//#define GET_DIFFUSE_PTR_VK(name) &GET_DIFFUSE_VK(name)
//#define GET_NORMALS_PTR_VK(name) &GET_NORMALS_VK(name)

namespace LGraphics
{
    //class LApp;
    class LModel;
    class LModelBuffer;
    class LImage;
    class LRectangleMirror;

    class LResourceManager
    {

        struct TexturesData
        {
            struct VulkanImageData
            {
                struct VulkanTextureData
                {
                    VkImageView texture = nullptr;
                    VkImage image;
                    VmaAllocation allocData;
                };
                VulkanTextureData diffuse, normals;
            };

            struct OGLImageData
            {
                //struct OGLTextureData
                //{
                //    std::shared_ptr<GLuint> texture = nullptr;
                //};
                GLuint diffuse, normals;
            };

            void* textures = nullptr;

            TexturesData(void* t)
            {
                textures = t;
            }

            TexturesData(){}

            TexturesData(TexturesData&& d) noexcept
                : textures(std::move(d.textures))
            {
                d.textures = nullptr;
            }

            ~TexturesData()
            {
                if (textures)
                {
                    if (app->info.api == L_OPENGL)
                        delete (OGLImageData*)textures;
                    else if (app->info.api == L_VULKAN)
                        delete (VulkanImageData*)textures;
                }
            }
        };

        struct ModelData
        {
            LModelBuffer* buffer;
            void** textures;
            size_t meshesToDraw = 0;
        };

        //using OpenGLTexture = TexturesData::OGLImageData::OGLTextureData;
        using OpenGLImage = TexturesData::OGLImageData;
        using VulkanTexture = TexturesData::VulkanImageData::VulkanTextureData;
        using VulkanImage = TexturesData::VulkanImageData;

        friend LApp;
        friend LModel;
        friend LImage;
        friend LRectangleMirror;

    public:
        // uses ResourceManager
        //static TexturesData loadTextureImageObject(const char* diffusePath, const char* normalsPath, const char* name);

        // doesn't use ResourceManager
        static void* loadTexture(const char* path, size_t& mipLevels);
        static void* loadCubeTexture(const std::vector<std::string>& paths, const char* name);

        //static void* loadTexture(unsigned char* bytes, size_t size, const char* name, int desiredChannel = 4);
        //static void loadModel(LModel* model, const char* modelPath, bool debugInfo = false);

        static void setApp(LApp* app) { LResourceManager::app = app; }

    private:

        //static void* loadNormals(const char* path, int desiredChannel = 4);
        //static void* loadNormals(unsigned char* bytes, size_t size, const char* name, size_t& mipLevels, int desiredChannel = 4);
        //static void* loadNormals(unsigned char* bytes, int width, int height, const char* path);

        //static void* loadTexture(const char* path, size_t& mipLevels, int desiredChannel = 4);
        //static void* loadTexture(unsigned char* bytes, size_t size, const char* name, size_t& mipLevels, int desiredChannel = 4);
        static void genTexture(uint8_t* bytes, int width, int height, GLuint* texture);

        static void createImageView(uint8_t* pixels, int texWidth,
            int texHeight, int texChannels, const char* path, size_t& miplevels,
            VkImageView& texture, VkImage& image, VmaAllocation& textureImageMemory);


        static LApp* app;
        static std::unordered_map<std::string, TexturesData> textures;
        static std::unordered_map<std::string, ModelData> models;
    };
}


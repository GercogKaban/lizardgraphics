#include "pch.h"
#include "LApp.h"
#include "LResourceManager.h"
#include "LError.h"
#include "textures.h"
#include "include/SOIL2/stb_image.h"

namespace LGraphics
{
#ifdef OPENGL
    std::map<std::string, GLuint> LResourceManager::textures;

    GLuint LResourceManager::loadTexture(const char* path)
    {
        int width, height;
        unsigned char* image;

        if (!path)
           image = SOIL_load_image_from_memory(LGraphics::notexture, notextureSize, &width, &height, 0, SOIL_LOAD_RGBA);
        else
        {
            if (textures.find(path) != textures.end())
                return textures[path];

            image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
            if (!image)
            {
                image = SOIL_load_image_from_memory(LGraphics::notexture, notextureSize, &width, &height, 0, SOIL_LOAD_RGBA);
                LWRITE("item at address: ", path, " is missing");
            }
        }
        GLuint texture = initTexture(image, width, height);
        if (path && image) textures.insert(std::make_pair(path, texture));
        return texture;
        //if (texturePath.size()) texturePath.clear();
        //if (texturesBytes) delete texturesBytes;
    }

    GLuint LResourceManager::loadTexture(const unsigned char* bytes, size_t size)
    {
        int width, height;
        auto image = SOIL_load_image_from_memory(bytes, size, &width, &height, 0, SOIL_LOAD_RGBA);
        return initTexture(image, width, height);
    }

    GLuint LResourceManager::initTexture(unsigned char* bytes, int width, int height)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(bytes);
        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;

        return 0;
    }
#endif OPENGL

#ifdef VULKAN

    std::map<std::string, std::tuple<VkImageView, VkImage, VkDeviceMemory>> LResourceManager::textures;
    LApp* LResourceManager::app;


    VkImageView LResourceManager::loadTexture(const char* path)
    {
        if (textures.find(path) != textures.end())
            return std::get<0>(textures[path]);

        VkImage textureImage;
        VkImageView view;
        VkDeviceMemory textureImageMemory;

        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        app->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(app->g_Device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(app->g_Device, stagingBufferMemory);

        stbi_image_free(pixels);

        app->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage, textureImageMemory);

        app->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        app->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        app->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(app->g_Device, stagingBuffer, nullptr);
        vkFreeMemory(app->g_Device, stagingBufferMemory, nullptr);

        app->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, view);
        textures.insert(std::make_pair(path, std::make_tuple(view, textureImage, textureImageMemory)));
        return view;
    }

#endif
}

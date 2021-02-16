#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "pch.h"

#include "LApp.h"
#include "LResourceManager.h"
#include "LModel.h"
#include "LModelBuffer.h"
#include "LLogger.h"
#include "textures.h"

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

    std::unordered_map<std::string, std::tuple<VkImageView, VkImage, VmaAllocation, size_t>*> LResourceManager::textures;
    std::unordered_map<std::string, LResourceManager::ModelData*> LResourceManager::models;
    LApp* LResourceManager::app;

    VkImageView LResourceManager::loadTexture(const char* path, int desiredChannel)
    {
        size_t dummy;
        return loadTexture(path, dummy, desiredChannel);
    }

    VkImageView LResourceManager::loadTexture(unsigned char* bytes, size_t size, const char* name, int desiredChannel)
    {
        size_t dummy;
        return loadTexture(bytes, size, name, dummy, desiredChannel);
    }

    VkImageView LResourceManager::loadTexture(const char* path, size_t& mipLevels, int desiredChannel)
    {
        if (!path || !strlen(path))
            return std::get<0>(*textures["dummy"]);
        if (textures.find(path) != textures.end())
            return std::get<0>(*textures[path]);

        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        if (texChannels == STBI_rgb)
        {
            PRINTLN(LOG_HEADER,"!!!!!!!!!WARNING!!!!!!!!! ", path, " in RGB format, this may cause drawing errors!");
            PRINTLN(LOG_HEADER, "Please, use textures in RGBA format!");
        }
        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        return createImageView(pixels, texWidth, texHeight, STBI_rgb_alpha, path, mipLevels);
    }

    VkImageView LResourceManager::loadTexture(unsigned char* bytes, size_t size, const char* name, size_t& mipLevels, int desiredChannel)
    {
        int height, width, imageChannels;
        if (textures.find(name) != textures.end())
            return std::get<0>(*textures[name]);
        stbi_uc* pixels = stbi_load_from_memory(bytes, size, &height, &width, &imageChannels, desiredChannel);

        return createImageView(pixels, width, height, imageChannels, name, mipLevels);
    }

    void LResourceManager::loadModel(LModel* model, const char* modelPath, bool debugInfo)
    {
        if (!models[modelPath])
        {
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;

            std::vector<std::vector<uint32_t>> vertIndices;
            std::vector<float> vertices;

            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath))
                throw std::runtime_error(warn + err);

            if (debugInfo)
            {
                PRINTLN("\nmodel path = ", modelPath, '\n');
                PRINTLN("model vertices count = ", attrib.vertices.size(), '\n');
            }

            model->meshesCount = shapes.size();
            auto meshesCount = model->meshesCount;
            vertIndices.resize(meshesCount);

            for (size_t i = 0; i < meshesCount; ++i)
            {
                if (debugInfo)
                {
                    PRINTLN("mesh #", i, " - ", shapes[i].name.data());
                    PRINTLN("    indices count = ", shapes[i].mesh.indices.size(), '\n');
                }

                for (const auto& index : shapes[i].mesh.indices)
                {
                    // vertex data
                    vertices.push_back(attrib.vertices[3 * index.vertex_index]);
                    vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                    vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

                    // textures data
                    if (index.texcoord_index != -1)
                    {
                        vertices.push_back(attrib.texcoords[2* index.texcoord_index]);
                        vertices.push_back(1.0f - attrib.texcoords[2* index.texcoord_index + 1]);
                    }

                    else
                    {
                        vertices.push_back(-1);
                        vertices.push_back(-1);
                    }

                    // normals data
                    //...


                    //indices data
                    vertIndices[i].push_back(vertIndices[i].size());
                }
            }

            auto buffer = new LModelBuffer(app, vertices, vertIndices);
            auto modelData = new ModelData
            {   buffer,
                new VkImageView[2],
                meshesCount,
            };

            model->meshesToDraw = new bool[meshesCount];
            model->buffer = buffer;

            models.insert(std::make_pair(std::string(modelPath), modelData));
            models[modelPath] = modelData;
            std::fill(model->meshesToDraw, model->meshesToDraw + meshesCount, true);
        }

        else
        {
            auto model_ = models[modelPath];
            model->buffer = model_->buffer;
            model->textures = model_->textures;
            model->meshesToDraw = new bool[model_->meshesToDraw];
        }
    }

    VkImageView LResourceManager::createImageView(unsigned char* pixels, int texWidth,
        int texHeight, int texChannels, const char* path, size_t& miplevels)
    {
        VkImage textureImage;
        VkImageView view;
        VmaAllocation textureImageMemory;

        VkDeviceSize imageSize = texWidth * texHeight* texChannels;

        miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
        app->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage, textureImageMemory, miplevels);

        {
            VkBuffer stagingBuffer;
            VmaAllocation stagingBufferMemory;

            app->createBuffer(pixels, stagingBuffer, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, imageSize);
            app->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, 
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, miplevels);
            app->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
            //app->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, 
                //VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);
            vmaDestroyBuffer(app->allocator, stagingBuffer, stagingBufferMemory);

            app->generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, miplevels);
        }

        stbi_image_free(pixels);

        app->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, view, miplevels);

        textures.insert(std::make_pair(path, 
            new std::tuple<VkImageView, VkImage, VmaAllocation,size_t>(view, textureImage, textureImageMemory, miplevels)));
        return view;
    }

#endif
}

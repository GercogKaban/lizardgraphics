#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "pch.h"

//#include "LApp.h"

#include "LResourceManager.h"
#include "LModel.h"
#include "LModelBuffer.h"
#include "LLogger.h"
#include "textures.h"

namespace LGraphics
{
    std::unordered_map<std::string, LResourceManager::TexturesData> LResourceManager::textures;
    std::unordered_map<std::string, LResourceManager::ModelData> LResourceManager::models;
    std::vector<AtlasData> LResourceManager::atlasData;
    LApp* LResourceManager::app;

    //LResourceManager::TexturesData LResourceManager::loadTextureImageObject(const char* diffusePath, const char* normalsPath, const char* name)
    //{
    //    LOG_CALL
    //    size_t dummy;
    //    auto text = textures.find(std::string(name));
    //    if (text != textures.end())
    //        return text->second;
    //    else
    //    {
    //        if (!diffusePath)
    //            throw std::runtime_error("please type correct path!");
    //        if (!normalsPath)
    //            PRINTLN("WARNING, TEXTURE NAMED BY ", name, "WILL BE LOADED WOTHOUT NORMAL MAP!");
    //        TexturesData ret;
    //        if (app->info.api == L_OPENGL)
    //        {
    //            using TYPE = LResourceManager::OpenGLImage;
    //            ret.textures = new TYPE();
    //            ((TYPE*)ret.textures)->diffuse.texture.reset((GLuint*)(loadTexture(diffusePath, dummy).get()));
    //            ((TYPE*)ret.textures)->normals.texture.reset((GLuint*)(loadTexture(normalsPath, dummy).get()));
    //        }
    //        //else
    //        //{
    //        //    using TYPE = LResourceManager::VulkanImage;
    //        //    ret.textures = std::make_shared<TYPE>(new TYPE());
    //        //    ((TYPE*)ret.textures.get())->diffuse. = loadTexture(diffusePath, dummy);
    //        //}
    //        return ret;
    //        //loadTexture(diffusePath, dummy);
    //        //normals = loadTexture(normalsPath, dummy);
    //    }
    //}

    void LResourceManager::loadCacheAtlases()
    {
        Atlas atl("dummy");
        auto atls = atl.getCacheAtlases();
        for (const auto& a : atls)
        {
            atl.loadAtlas(a);
            auto data = atl.getAtlasData();
            atlasData.push_back(data);
            size_t dummy;
            loadTexture(data.atlasPath.data(), dummy);
            for (const auto& d : data.texturePaths)
                textures.insert(std::make_pair(d, &atlasData[atlasData.size() - 1]));
        }
    }

    void* LResourceManager::loadTexture(const char* path, size_t& mipLevels)
    {
        //auto it = textures.find(path);
        //if (it != textures.end())
        //    return &((OpenGLImage*)it->second.textures)->diffuse;
        if (auto it = textures.find(path); it != textures.end())
            return &((OpenGLImage*)it->second.textures)->diffuse;

        int texWidth, texHeight;
        int dummy;
        uint8_t* pixels = (uint8_t*)stbi_load(path, &texWidth, &texHeight, &dummy, STBI_rgb_alpha);
        GLuint texture;
        if (app->info.api == L_OPENGL)
            genTexture(pixels, texWidth, texHeight, &texture);
        //else
        //{
        //    auto vlk = (VulkanTexture*)texture.get();
        //    createImageView(pixels, texWidth, texHeight, STBI_rgb_alpha, path, mipLevels, vlk->texture, vlk->image, vlk->allocData);
        //}
        //
        stbi_image_free(pixels);
        textures.emplace(std::make_pair(std::string(path), std::move(TexturesData{ new OpenGLImage{ texture, 0 } })));
        return &((OpenGLImage*)textures[path].textures)->diffuse;
    }

    void* LResourceManager::loadCubeTexture(const std::vector<std::string>& paths, const char* name)
    {
        LOG_CALL
        //if (auto it(textures.find(name)); it != textures.end())
        //   return it->second.textures;
        //std::shared_ptr<void> texture(new GLuint());
            
        void* texture = new GLuint();
        glGenTextures(1, (GLuint*)texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *(GLuint*)texture);
       
        for (size_t i = 0; i < paths.size(); i++)
        {
            int texWidth, texHeight;
            int dummy;
            uint8_t* pixels = (uint8_t*)stbi_load(paths[i].data(), &texWidth, &texHeight, &dummy, STBI_rgb_alpha);
            if (!pixels)
                throw std::runtime_error("failed to load texture image!");
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels
            );
            stbi_image_free(pixels);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        //std::shared_ptr<void> textData;
        //if (app->info.api == L_OPENGL)
        //    textData.reset(new LResourceManager::OpenGLImage{
        //    LResourceManager::OpenGLTexture{std::make_shared<GLuint>(texture)},
        //        LResourceManager::OpenGLTexture{std::make_shared<GLuint>(nullptr)}});
        ////else if (app->info.api == L_VULKAN)
        ////    textData = new LResourceManager::VulkanImage{ LResourceManager::VulkanTexture{nullptr}, LResourceManager::VulkanTexture{nullptr} };
        //textures.insert(std::make_pair(std::string(name), LResourceManager::TexturesData{ textData }));
        return texture;
    }

    LResourceManager::~LResourceManager()
    {
        std::vector<std::string> text;
        text.reserve(textures.size());
        for (const auto& t : textures)
        {
            if (dynamic_cast<AtlasData*>((AtlasData*)t.second.textures))
            {
                auto data = (AtlasData*)t.second.textures;
                for (const auto& path : data->texturePaths)
                    text.push_back(path);
            }
            else
                text.push_back(t.first);
        }

        
        Atlas atl("out.jpg");
        atl.setSizeThreshold(INT_MAX);
        atl.makeAtlas(text);
        atl.saveAtlas();
        LResourceManager::textures.clear();
    }

    void LResourceManager::genTexture(uint8_t* bytes, int width, int height, GLuint* texture)
    {
        LOG_CALL
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)app->info.anisotropy);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    //void LResourceManager::loadModel(LModel* model, const char* modelPath, bool debugInfo)
    //{
    //    LOG_CALL
    //    if (!models[modelPath])
    //    {
    //        tinyobj::attrib_t attrib;
    //        std::vector<tinyobj::shape_t> shapes;
    //        std::vector<tinyobj::material_t> materials;
    //        std::string warn, err;

    //        std::vector<std::vector<uint32_t>> vertIndices;
    //        std::vector<float> vertices;

    //        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath))
    //            throw std::runtime_error(warn + err);

    //        if (debugInfo)
    //        {
    //            PRINTLN("\nmodel path = ", modelPath, '\n');
    //            PRINTLN("model vertices count = ", attrib.vertices.size(), '\n');
    //        }

    //        model->meshesCount = shapes.size();
    //        auto meshesCount = model->meshesCount;
    //        vertIndices.resize(meshesCount);

    //        for (size_t i = 0; i < meshesCount; ++i)
    //        {
    //            if (debugInfo)
    //            {
    //                PRINTLN("mesh #", i, " - ", shapes[i].name.data());
    //                PRINTLN("    indices count = ", shapes[i].mesh.indices.size(), '\n');
    //            }

    //            for (const auto& index : shapes[i].mesh.indices)
    //            {
    //                // vertex data
    //                vertices.push_back(attrib.vertices[3 * index.vertex_index]);
    //                vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
    //                vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

    //                // textures data
    //                if (index.texcoord_index != -1)
    //                {
    //                    vertices.push_back(attrib.texcoords[2* index.texcoord_index]);
    //                    vertices.push_back(1.0f - attrib.texcoords[2* index.texcoord_index + 1]);
    //                }

    //                else
    //                {
    //                    vertices.push_back(-1);
    //                    vertices.push_back(-1);
    //                }

    //                // normals data
    //                //...


    //                //indices data
    //                vertIndices[i].push_back(vertIndices[i].size());
    //            }
    //        }

    //        auto buffer = new LModelBuffer(app, vertices, vertIndices);
    //        auto modelData = new ModelData
    //        {   buffer,
    //            (void**)new VkImageView[2],
    //            meshesCount,
    //        };

    //        model->meshesToDraw = new bool[meshesCount];
    //        model->buffer = buffer;

    //        models.insert(std::make_pair(std::string(modelPath), modelData));
    //        models[modelPath] = modelData;
    //        std::fill(model->meshesToDraw, model->meshesToDraw + meshesCount, true);
    //    }

    //    else
    //    {
    //        auto model_ = models[modelPath];
    //        model->buffer = model_->buffer;
    //        model->textures = model_->textures;
    //        model->meshesToDraw = new bool[model_->meshesToDraw];
    //    }
    //}

    void LResourceManager::createImageView(uint8_t* pixels, int texWidth,
        int texHeight, int texChannels, const char* path, size_t& miplevels,
        VkImageView& view, VkImage& textureImage, VmaAllocation& textureImageMemory)
    {
        LOG_CALL
        const auto imageSize = texWidth * texHeight * texChannels;
        
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
    }
}
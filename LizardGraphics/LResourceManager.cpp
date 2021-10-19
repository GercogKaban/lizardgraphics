#define STB_IMAGE_IMPLEMENTATION
//#define TINYOBJLOADER_IMPLEMENTATION

#include "pch.h"

//#include "LApp.h"

#include "LResourceManager.h"
#include "LLogger.h"
#include "textures.h"
#include "LBuffer.h"

namespace LGraphics
{
    std::unordered_map<std::string, TexturesData> LResourceManager::textures;
    std::unordered_map<std::string, LModel*> LResourceManager::models;
    std::vector<AtlasData> LResourceManager::atlasData;
    LApp* LResourceManager::app;

    void* LResourceManager::loadTexture(const char* path, size_t& mipLevels)
    {
        int texWidth, texHeight;
        int dummy;
        uint8_t* pixels = (uint8_t*)stbi_load(path, &texWidth, &texHeight, &dummy, STBI_rgb_alpha);
        if (!pixels) return nullptr;
        GLuint id;
        if (app->info.api == L_OPENGL)
            genTexture(pixels, texWidth, texHeight, &id);
        //else
        //{
        //    auto vlk = (VulkanTexture*)texture.get();
        //    createImageView(pixels, texWidth, texHeight, STBI_rgb_alpha, path, mipLevels, vlk->texture, vlk->image, vlk->allocData);
        //}
        //
        stbi_image_free(pixels);
        return new GLuint(id);
    }

    void* LResourceManager::loadCubeTexture(const std::vector<std::string>& paths)
    {
        LOG_CALL

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
        return texture;
    }

    void LResourceManager::setTexture(const std::string& texturePath, TexturesData& data, void* texture)     
    {
        if (texture)
        {
            auto& td = TO_GL(data);
            td.id = *(GLuint*)texture;
            delete texture;

#ifdef MEGATEXTURE_LG
            td.offset = app->megatexture.subtextures[texturePath].first;
            td.size = app->megatexture.subtextures[texturePath].second;
#endif
        }
    }

    std::vector<TexturesData> LResourceManager::loadImageResource(LImage::ImageResource res)
    {
        const auto diffusePath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string()  +
            "/textures/").generic_string() + '/' +  app->qualityDirectories[app->info.texturesQuality] +  "/diffuse/" + res.name;
        const auto normalPath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() +
            "/textures/").generic_string() + '/' + app->qualityDirectories[app->info.texturesQuality] + "/normal/" + res.name;
        const auto displacementPath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() +
            "/textures/").generic_string() + '/' + app->qualityDirectories[app->info.texturesQuality] + "/displacement/" + res.name;

        if (res.diffuse && textures.find(diffusePath) == textures.end())
            textures.emplace(std::make_pair(diffusePath, std::move(TexturesData())));
        if (res.normals && textures.find(normalPath) == textures.end())
            textures.emplace(std::make_pair(normalPath, std::move(TexturesData())));
        if (res.parallax && textures.find(displacementPath) == textures.end())
            textures.emplace(std::make_pair(displacementPath, std::move(TexturesData())));

        auto itd = textures.find(diffusePath);
        const auto& td = TO_GL(itd->second);

        auto itn = textures.find(normalPath);
        const auto& tn = TO_GL(itn->second);

        auto itp = textures.find(displacementPath);
        const auto& tp = TO_GL(itp->second);

        if (res.diffuse && td.id == NO_TEXTURE)
        {
            size_t dummy;
            auto texture = loadTexture(diffusePath.data(), dummy);
            setTexture(diffusePath, itd->second, texture);
        }

        if (res.normals && tn.id == NO_TEXTURE)
        {
            size_t dummy;
            auto texture = loadTexture(normalPath.data(), dummy);
            setTexture(normalPath, itn->second, texture);
        }

        if (res.parallax && tp.id == NO_TEXTURE)
        {
            size_t dummy;
            auto texture = loadTexture(displacementPath.data(), dummy);
            setTexture(displacementPath, itp->second, texture);
        }

        return { itd->second,itn->second, itp->second };
    }

    TexturesData LResourceManager::loadImageSkyboxResource(LImage::ImageSkyboxResource res)
    {
        if (textures.find("skybox/" + res.name) == textures.end())
        {
            auto it = textures.emplace(std::make_pair("skybox/" + res.name, std::move(TexturesData(new TexturesData::OGLImageData()))));

            //auto it = textures.find("skybox/" + res.name);
            auto& td = TO_GL(it.first->second);
            const auto skyboxPath = std::filesystem::current_path().generic_string() + '/' +
                app->qualityDirectories[app->info.texturesQuality] + "/skyboxes/";
            std::vector<std::string> paths = {
                skyboxPath + "right" + res.extension,skyboxPath + "left" + res.extension,
                skyboxPath + "top" + res.extension,skyboxPath + "bottom" + res.extension,
                skyboxPath + "front" + res.extension,skyboxPath + "back" + res.extension };
            if (td.id == NO_TEXTURE)
            {
                auto texture = loadCubeTexture(paths);
                td.id = *(GLuint*)texture;
                delete texture;
            }
        }
        return textures.find("skybox/" + res.name)->second;
    }

    //LResourceManager::TexturesData::OGLImageData& LResourceManager::toGl(TexturesData& data)
    //{
    //    return *((LResourceManager::OpenGLImage*)data.textures);
    //}

    //LResourceManager::TexturesData::VulkanImageData& LResourceManager::toVk(TexturesData& data)
    //{
    //    return *((LResourceManager::VulkanImage*)data.textures);
    //}

    // утечка!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    void LResourceManager::clear()
    {
        LResourceManager::textures.clear();
        //for (auto m : models)
        //    delete m.second;
        LResourceManager::models.clear();
    }

    LResourceManager::~LResourceManager()
    {
       
    }

    void LResourceManager::genTexture(uint8_t* bytes, int width, int height, GLuint* texture)
    {
        LOG_CALL
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)app->info.anisotropy);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LResourceManager::loadModel(LModel* model, LModel::ModelResource res)
    {
        LOG_CALL
        if (auto it = models.find(res.name); it != models.end())
        {
            model->meshes = it->second->meshes;
            return;
        }
        const auto modelPath = std::filesystem::current_path().generic_string() + '/' + "models/" +
            app->qualityDirectories[app->info.texturesQuality] + '/' + res.name;
        PRINTLN(modelPath);
        loadModel(model, modelPath);
        models.insert(std::make_pair(res.name, model));
    }

    void LResourceManager::loadModel(LModel* model, const std::string& modelPath)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(modelPath, app->modelLoadingFlags);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            throw std::runtime_error("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
        //auto m_GlobalInverseTransform = scene->mRootNode->mTransformation;
        //m_GlobalInverseTransform.Inverse();
        processNode(app, model->meshes, scene->mRootNode, scene);
    }

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

    void LResourceManager::processNode(LApp* app, std::vector<LModel::Mesh>& meshes, aiNode* node, const aiScene* scene)
    {
        LOG_CALL
        // process all the node's meshes (if any)
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(app,mesh, scene));
        }

        // then do the same for each of its children
        for (uint32_t i = 0; i < node->mNumChildren; i++)
            processNode(app,meshes, node->mChildren[i], scene);
    }

    LModel::Mesh LResourceManager::processMesh(LApp* app, aiMesh* mesh, const aiScene* scene)
    {
        LOG_CALL
        LModel::Mesh out;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 pos, normals, tangent, bitangent;
            glm::vec2 textureCoords;

            pos.x = mesh->mVertices[i].x, pos.y = mesh->mVertices[i].y, pos.z = mesh->mVertices[i].z;

            normals.x = mesh->mNormals[i].x;
            normals.y = mesh->mNormals[i].y;
            normals.z = mesh->mNormals[i].z;
            if (mesh->mTextureCoords[0])
            {
                const auto crop = 0.009f;
                textureCoords.x = mesh->mTextureCoords[0][i].x > 0? mesh->mTextureCoords[0][i].x - crop : mesh->mTextureCoords[0][i].x + crop;
                textureCoords.y = mesh->mTextureCoords[0][i].y > 0 ? mesh->mTextureCoords[0][i].y - crop : mesh->mTextureCoords[0][i].y + crop;
            }
            else
                textureCoords = glm::vec2(0.0f, 0.0f);

            if (!mesh->HasTangentsAndBitangents())
                throw std::runtime_error("can't load tangents and bitangents!\n");
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;

            bitangent.x = mesh->mBitangents[i].x;
            bitangent.y = mesh->mBitangents[i].y;
            bitangent.z = mesh->mBitangents[i].z;

            vertex.Position = pos;
            vertex.Normal = normals;
            vertex.TexCoords = textureCoords;
            vertex.Tangent = tangent;
            vertex.Bitangent = bitangent;
            vertices.push_back(vertex);
        }

        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            assert(face.mNumIndices == 3);
            for (size_t j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        LBuffer* b = new LBuffer(app, vertices, indices);
        TexturesData d {new TexturesData::OGLImageData };
        TexturesData n{ new TexturesData::OGLImageData };
        TexturesData p{ new TexturesData::OGLImageData };
        if (mesh->mMaterialIndex >= 0)
        {
            auto& gld = TO_GL(d);
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            auto diff = loadMaterialTextures(material, aiTextureType_DIFFUSE);
            const auto& gldiff = TO_GL(diff);
            gld.id = gldiff.id;
            gld.offset = gldiff.offset;
            gld.size = gldiff.size;

            auto& gln = TO_GL(n);
            auto norm = loadMaterialTextures(material, aiTextureType_NORMALS);
            const auto& gldnorm = TO_GL(norm);
            gln.id = gldnorm.id;
            gln.offset = gldnorm.offset;
            gln.size = gldnorm.size;

            auto& glp = TO_GL(p);
            auto parallax = loadMaterialTextures(material, aiTextureType_HEIGHT);
            const auto& gldPar = TO_GL(parallax);
            glp.id = gldPar.id;
            glp.offset = gldPar.offset;
            glp.size = gldPar.size;
        }

        out.buffer = b;
        out.image = new LImage(d,n,p);
        out.image->diffusePath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() + "/textures/").generic_string()
            + '/' + app->qualityDirectories[app->info.texturesQuality] + "/diffuse/";
        out.image->normalsPath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() + "/textures/").generic_string()
            + '/' + app->qualityDirectories[app->info.texturesQuality] + "/normal/";
        out.image->normalsPath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() + "/textures/").generic_string()
            + '/' + app->qualityDirectories[app->info.texturesQuality] + "/displacement/";
        return out;
    }

    const TexturesData& LResourceManager::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
    {
        for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string strCpp = std::string(str.C_Str());
            auto texturesPath = std::filesystem::read_symlink(std::filesystem::current_path().generic_string() + "/textures/").generic_string() +
                '/' + app->qualityDirectories[app->info.texturesQuality];
            if (type == aiTextureType_DIFFUSE)
                texturesPath += "/diffuse/";
            else if (type == aiTextureType_NORMALS)
                texturesPath += "/normal/";
            else if (type == aiTextureType_HEIGHT)
                texturesPath += "/displacement/";
            std::replace(strCpp.begin(), strCpp.end(), '\\', '/');
            strCpp = strCpp.rfind('/') ? texturesPath+ strCpp.substr(strCpp.rfind('/') + 1) : texturesPath + strCpp;
            if (auto it = textures.find(strCpp); it != textures.end())
                return it->second;
            // костыль!!!!
            else
            {
                size_t dummy;
                GLuint* texture = (GLuint*)loadTexture(texturesPath.data(), dummy);
                if (!texture)
                    return textures.find("dummy")->second;
                TexturesData d;
                auto& td = TO_GL(d);
                    td.id = *texture;
                delete texture;
                return textures.insert(std::make_pair(strCpp, d)).first->second;
                //return d;
            }
        }
        return textures.find("dummy")->second;
    }
}
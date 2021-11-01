#define STB_IMAGE_IMPLEMENTATION
//#define TINYOBJLOADER_IMPLEMENTATION

#include "pch.h"

//#include "LApp.h"

#include "LResourceManager.h"
#include "LLogger.h"
#include "textures.h"
#include "LBuffer.h"
#include "LAnimation.h"

namespace LGraphics
{
    std::unordered_map<std::string, TexturesData> LResourceManager::textures;
    std::unordered_map<std::string, LModel*> LResourceManager::models;
    std::vector<AtlasData> LResourceManager::atlasData;
    LModel* LResourceManager::currentModel;
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
        const auto diffusePath = app->getRealDiffusePath() + res.name;
        const auto normalPath = app->getRealNormalPath() + res.name;
        const auto displacementPath = app->getRealDisplacementPath() + res.name;
        const auto reflexPath = app->getRealReflexPath() + res.name;

        if (res.diffuse && textures.find(diffusePath) == textures.end())
            textures.insert(std::make_pair(diffusePath, TexturesData()));
        if (res.normals && textures.find(normalPath) == textures.end())
            textures.insert(std::make_pair(normalPath, TexturesData()));
        if (res.parallax && textures.find(displacementPath) == textures.end())
            textures.insert(std::make_pair(displacementPath, TexturesData()));
        if (res.reflex && textures.find(reflexPath) == textures.end())
            textures.insert(std::make_pair(reflexPath, TexturesData()));

        auto itd = textures.find(diffusePath);
        auto itn = textures.find(normalPath);
        auto itp = textures.find(displacementPath);
        auto itr = textures.find(reflexPath);

        size_t dummy;
        if (res.diffuse && !itd->second.textures)
        {
            itd->second.textures = new TexturesData::OGLImageData();
            auto texture = loadTexture(diffusePath.data(), dummy);
            setTexture(diffusePath, itd->second, texture);
        }
        
        if (res.normals && !itn->second.textures)
        {
            itn->second.textures = new TexturesData::OGLImageData();
            auto texture = loadTexture(normalPath.data(), dummy);
            setTexture(normalPath, itn->second, texture);
        }

        if (res.parallax && !itp->second.textures)
        {
            itp->second.textures = new TexturesData::OGLImageData();
            auto texture = loadTexture(displacementPath.data(), dummy);
            setTexture(displacementPath, itp->second, texture);
        }

        if (res.reflex && !itr->second.textures)
        {
            itr->second.textures = new TexturesData::OGLImageData();
            auto texture = loadTexture(reflexPath.data(), dummy);
            setTexture(reflexPath, itr->second, texture);
        }

        return { itd->second,itn->second, itp->second, itr->second };
    }

    TexturesData LResourceManager::loadImageSkyboxResource(LImage::ImageSkyboxResource res)
    {
        if (textures.find("skybox/" + res.name) == textures.end())
        {
            auto it = textures.emplace(std::make_pair("skybox/" + res.name, std::move(TexturesData(new TexturesData::OGLImageData()))));

            //auto it = textures.find("skybox/" + res.name);
            auto& td = TO_GL(it.first->second);
            const auto skyboxPath = app->getRealTexturesPath() + "/skyboxes/";
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

    void LResourceManager::loadModel(LModel* model, LModel::ModelResource res, bool cropTextureCoords)
    {
        LOG_CALL
        if (auto it = models.find(res.name); it != models.end())
        {
            model->meshes = it->second->meshes;
            return;
        }
        const auto modelPath = app->getRealModelsPath() + res.name;
        PRINTLN(modelPath);
        loadModel(model, modelPath, cropTextureCoords);
        models.insert(std::make_pair(res.name, model));
    }

    void LResourceManager::loadModel(LModel* model, const std::string& modelPath, bool cropTextureCoords)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(modelPath, app->modelLoadingFlags);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            PRINTLN("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
            throw std::runtime_error("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
        }

        currentModel = model;   
        auto& modelAnimations = model->animations;
        processNode(app, model->meshes, scene->mRootNode, scene, cropTextureCoords, scene->mRootNode->mTransformation);
        for (size_t i = 0; i < scene->mNumAnimations; ++i)
        {
            Animation animation;
            const auto assimpAnimation = scene->mAnimations[i];
            animation.m_Duration = assimpAnimation->mDuration;
            animation.m_TicksPerSecond = assimpAnimation->mTicksPerSecond;
            ReadHeirarchyData(animation.m_RootNode, scene->mRootNode);
            ReadMissingBones(assimpAnimation, model, animation);
            modelAnimations.insert(std::make_pair(assimpAnimation->mName.C_Str(),animation));
        }
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

    void LResourceManager::processNode(LApp* app, std::vector<LModel::Mesh>& meshes, aiNode* node, const aiScene* scene, bool cropTextureCoords,
        aiMatrix4x4 transform)
    {
        LOG_CALL
        // process all the node's meshes (if any)
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(app,mesh, scene, cropTextureCoords, transform));
        }

        // then do the same for each of its children
        for (uint32_t i = 0; i < node->mNumChildren; i++)
            processNode(app,meshes, node->mChildren[i], scene, cropTextureCoords, node->mChildren[i]->mTransformation);
    }
    
    LModel::Mesh LResourceManager::processMesh(LApp* app, aiMesh* mesh, const aiScene* scene, bool cropTextureCoords,
        aiMatrix4x4 transform)
    {
        LOG_CALL
        LModel::Mesh out;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        const auto crop = cropTextureCoords ? 0.01f : 0.0f;

        std::pair<float, float> rangeX, rangeY, rangeZ;
        rangeX.first = std::numeric_limits<float>::max(); rangeX.second = std::numeric_limits<float>::min();
        rangeY.first = std::numeric_limits<float>::max(); rangeY.second = std::numeric_limits<float>::min();
        rangeZ.first = std::numeric_limits<float>::max(); rangeZ.second = std::numeric_limits<float>::min();
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            glm::vec2 textureCoords;

            if (!mesh->HasTangentsAndBitangents())
                throw std::runtime_error("can't load tangents and bitangents!\n");

            if (mesh->mTextureCoords[0])
            {
                textureCoords.x = mesh->mTextureCoords[0][i].x > 0 ? mesh->mTextureCoords[0][i].x - crop : mesh->mTextureCoords[0][i].x + crop;
                textureCoords.y = mesh->mTextureCoords[0][i].y > 0 ? mesh->mTextureCoords[0][i].y - crop : mesh->mTextureCoords[0][i].y + crop;
            }
            else
                textureCoords = glm::vec2(0.0f, 0.0f);

            aiVector3D pos_ = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            aiMatrix3x3 transform_(transform);
            pos_ *= transform_;

            vertex.Position = assimpToGLM(pos_);
            vertex.Normal = assimpToGLM(mesh->mNormals[i]);
            vertex.Tangent = assimpToGLM(mesh->mTangents[i]);
            vertex.Bitangent = assimpToGLM(mesh->mBitangents[i]);
            vertex.TexCoords = textureCoords;
            vertices.push_back(vertex);
            if (pos_.x < rangeX.first) rangeX.first = pos_.x;
            if (pos_.x > rangeX.second) rangeX.second = pos_.x;

            if (pos_.y < rangeY.first) rangeY.first = pos_.y;
            if (pos_.y > rangeY.second) rangeY.second = pos_.y;

            if (pos_.z < rangeZ.first) rangeZ.first = pos_.z;
            if (pos_.z > rangeZ.second) rangeZ.second = pos_.z;
        }
        app->lastLoadedRanges = { rangeX ,rangeY ,rangeZ };

        ExtractBoneWeightForVertices(currentModel,vertices, mesh, scene);

        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            assert(face.mNumIndices == 3);
            for (size_t j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        out.buffer = new LBuffer(app, vertices, indices);
        out.image = new LImage({ new TexturesData::OGLImageData }, { new TexturesData::OGLImageData }, { new TexturesData::OGLImageData });
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            out.image->setDiffuse(loadMaterialTextures(material, aiTextureType_DIFFUSE));
            out.image->setNormal(loadMaterialTextures(material, aiTextureType_NORMALS));
            out.image->setDisplacement(loadMaterialTextures(material, aiTextureType_HEIGHT));
            out.image->setReflex(loadMaterialTextures(material, aiTextureType_REFLECTION));
        }

        const auto& normText = out.image->getNormal();
        const auto& castedNorm = TO_GL(normText);
        out.image->setNormalMapping(castedNorm.id != NO_TEXTURE);

        const auto& displText = out.image->getParallax();
        const auto& castedDispl = TO_GL(displText);
        out.image->setParallaxMapping(castedDispl.id != NO_TEXTURE);

        const auto& refText = out.image->getReflex();
        const auto& castedRef = TO_GL(refText);
        out.image->setReflexMapping(castedRef.id != NO_TEXTURE);

        const auto qualityDir = app->getRealTexturesPath();
        out.image->diffusePath = qualityDir + "diffuse/";
        out.image->normalsPath = qualityDir + "normal/";
        out.image->displacementPath = qualityDir + "displacement/";
        out.image->reflexPath = qualityDir + "reflex/";
        return out;
    }

    const TexturesData& LResourceManager::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
    {
        for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string strCpp = std::string(str.C_Str());
            auto texturesPath = app->getRealTexturesPath();
            if (type == aiTextureType_DIFFUSE)
                texturesPath += "diffuse/";
            else if (type == aiTextureType_NORMALS)
                texturesPath += "normal/";
            else if (type == aiTextureType_HEIGHT)
                texturesPath += "displacement/";
            else if (type == aiTextureType_REFLECTION)
                texturesPath += "reflex/";
            std::replace(strCpp.begin(), strCpp.end(), '\\', '/');
            strCpp = strCpp.rfind('/') ? texturesPath + strCpp.substr(strCpp.rfind('/') + 1) : texturesPath + strCpp;
            return loadMaterialTextures(strCpp);
        }
        return loadMaterialTextures("");
    }

    const TexturesData& LResourceManager::loadMaterialTextures(const std::string& path)
    {
        if (auto it = textures.find(path); it != textures.end())
            return it->second;
        // костыль!!!!
        else
        {
            size_t dummy;
            GLuint* texture = (GLuint*)loadTexture(path.data(), dummy);
            if (!texture)
                return textures.find("dummy")->second;
            TexturesData d;
            d.textures = new TexturesData::OGLImageData();
            auto& td = TO_GL(d);
            td.id = *texture;
            delete texture;
            return textures.insert(std::make_pair(path, d)).first->second;
        }
        return textures.find("dummy")->second;
    }

    void LResourceManager::SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.BoneIDs[i] = -1;
            vertex.BoneWeights[i] = 0.0f;
        }
    }

    void LResourceManager::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.BoneIDs[i] < 0)
            {
                vertex.BoneWeights[i] = weight;
                vertex.BoneIDs[i] = boneID;
                break;
            }
        }
    }

    void LResourceManager::ExtractBoneWeightForVertices(LModel* model, std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = model->m_BoneInfoMap;
        int& boneCount = model->m_BoneCounter;

        for (size_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            const std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = assimpToGLM(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
                boneID = boneInfoMap[boneName].id;
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;

            for (size_t weightIndex = 0; weightIndex < mesh->mBones[boneIndex]->mNumWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

    void LResourceManager::ReadMissingBones(const aiAnimation* assimpAnimation, LModel* model, Animation& animation)
    {
        auto& boneInfoMap = model->GetBoneInfoMap();//getting m_BoneInfoMap from Model class
        int& boneCount = model->GetBoneCount();     //getting the m_BoneCounter from Model class

        //reading channels(bones engaged in an animation and their keyframes)
        for (size_t i = 0; i < assimpAnimation->mNumChannels; i++)
        {
            const auto channel = assimpAnimation->mChannels[i];
            const std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            animation.m_Bones.push_back(Bone(channel->mNodeName.data,
                boneInfoMap[channel->mNodeName.data].id, channel)); 
        }

        animation.m_BoneInfoMap = boneInfoMap;
    }

    void LResourceManager::ReadHeirarchyData(NodeData& dest, const aiNode* src)
    {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = assimpToGLM(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++)
        {
            NodeData newData;
            ReadHeirarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

}
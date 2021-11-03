#pragma once

#include <unordered_map>
#include <variant>
#include <memory>
#include <vector>

//#include "include/stb/stb_image.h"
//#include "include/tinyobjloader/tiny_obj_loader.h"
//#include "VulkanMemoryAllocator/include/vk_mem_alloc.h"
#include "include/GLEW/glew.h"
#include "LApp.h"
#include "AtlasGenerator.h"
#include "LImage.h"
#include "LModel.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace LGraphics
{
    class LModel;
    class LModelBuffer;
    class LImage;
    class LRectangleMirror;
    class LRectangleShape;
    class LSkyBox;
    class Animation;
    class Animator;

    class LResourceManager
    {
        friend LRectangleShape;
        friend LSkyBox;
        friend Animation;
        friend Animator;
 
        struct ModelData
        {
            LModelBuffer* buffer;
            TexturesData** textures;
            //size_t meshesToDraw = 0;
        };

        friend LApp;
        friend LModel;
        friend LImage;
        friend LCube;
        friend LRectangleMirror;

    public:

        static std::vector<TexturesData> loadImageResource(LImage::ImageResource res);
        static TexturesData loadImageSkyboxResource(LImage::ImageSkyboxResource res);

        //static TexturesData::OGLImageData& toGl(TexturesData& data);
        //static TexturesData::VulkanImageData& toVk(TexturesData& data);

        static void setApp(LApp* app) { LResourceManager::app = app; }
        static void clear();
        ~LResourceManager();

    private:

        static void  loadModel(LModel* model, LModel::ModelResource res, bool cropTextureCoords);
        static void  loadModel(LModel* model, const std::string& path, bool cropTextureCoords);
        static void* loadTexture(const char* path, size_t& mipLevels);
        static void* loadCubeTexture(const std::vector<std::string>& paths);
        static void setTexture(const std::string& texturePath, TexturesData& data, void* texture);

        static void genTexture(uint8_t* bytes, int width, int height, GLuint* texture);

        static void createImageView(uint8_t* pixels, int texWidth,
            int texHeight, int texChannels, const char* path, size_t& miplevels,
            VkImageView& texture, VkImage& image, VmaAllocation& textureImageMemory);

        static void processNode(LApp* app, std::vector<LModel::Mesh>&, aiNode* node, const aiScene* scene, bool cropTextureCoords, 
            aiMatrix4x4 transform);
        static LModel::Mesh processMesh(LApp* app, aiMesh* mesh, const aiScene* scene, bool cropTextureCoords, 
            aiMatrix4x4 transform);
        static const TexturesData& loadMaterialTextures(aiMaterial* mat, aiTextureType type);
        static const TexturesData& LResourceManager::loadMaterialTextures(const std::string& path);

        static void SetVertexBoneDataToDefault(Vertex& vertex);
        static void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
        static void ExtractBoneWeightForVertices(LModel* model, std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

        static void ReadMissingBones(const aiAnimation* assimpAnimation, LModel* model, Animation& animation);
        static void ReadHeirarchyData(NodeData& dest, const aiNode* src);

        static std::string tryToFindFile(const std::string& path);
        static const aiScene* tryToLoadScene(const std::string& path, Assimp::Importer& importer, size_t it = 0);

        static LApp* app;

        static std::vector<AtlasData> atlasData;
        static std::unordered_map<std::string, TexturesData> textures;
        static std::unordered_map<std::string, LModel*> models;
        static LModel* currentModel;
    };
}


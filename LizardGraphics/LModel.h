#pragma once
#include "LShape.h"
#include "LImage.h"
#include "LBuffer.h"
#include "LAnimation.h"

namespace LGraphics
{
    class LResourceManager;

    class LModel : public LShape
    {
        friend LApp;
        friend Animation;
    public:

        struct Mesh
        {
            LBuffer* buffer = nullptr;
            LImage* image = nullptr;
        };

        struct ModelResource
        {
            std::string name;
        };

        friend LApp;
        friend LResourceManager;

        const char* getObjectType() const { return "LModel"; }

        void setDiffuse(const TexturesData& data, size_t meshNum = 0);
        void setNormal(const TexturesData& data, size_t meshNum = 0);
        void setDisplacement(const TexturesData& data, size_t meshNum = 0);

        void setNormalMapping(bool normalMapping, size_t meshNum = 0);
        void setParallaxMapping(bool parallaxMapping, size_t meshNum = 0);

        void setNormalMappingAllMeshes(bool normalMapping);
        void setParallaxMappingAllMeshes(bool parallaxMapping);

        bool getNormalMapping(size_t meshNum = 0)   const { return meshes[meshNum].image->getNormalMapping(); };
        bool getParallaxMapping(size_t meshNum = 0) const { return meshes[meshNum].image->getParallaxMapping(); };

        LModel(LApp* app, ModelResource res, bool cropTextureCoords = false);
        LModel(LApp* app, const std::string& modelPath, const std::string& diffusePath,
            const std::string& normalPath, const std::string& displacementPath, bool cropTextureCoords = false);
        LModel(LApp* app, const std::string& modelPath, const std::vector<std::string>& diffuseNames,
            const std::vector<std::string>& normalNames, const std::vector<std::string>& displacementNames, bool cropTextureCoords = false);

        LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices, bool cropTextureCoords = false);
        LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices, 
            const std::vector<uint32_t>& indices);
        ~LModel();
        const auto& getMehes() const { return meshes; };
        void draw() override;

        const auto& getAnime() const { return animations;}

    protected:

        LModel(LApp* app, const std::string& path, bool cropTextureCoords, size_t dummy);
        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex) override {}
        std::vector<Mesh> meshes;

        std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;
        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }

        std::vector<Animation> animations;
        std::vector<Animator> animators;

        void init();
    };
}
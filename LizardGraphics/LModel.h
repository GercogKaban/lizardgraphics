#pragma once
#include "LShape.h"
#include "LImage.h"
#include "LBuffer.h"

namespace LGraphics
{
    class LResourceManager;
    class LModel : public LShape
    {
        friend LApp;
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

        LModel(LApp* app, ModelResource res);
        LModel(LApp* app, const std::string& modelPath, const std::string& diffusePath,
            const std::string& normalPath, const std::string& displacementPath);
        LModel(LApp* app, const std::string& modelPath, const std::vector<std::string>& diffuseNames,
            const std::vector<std::string>& normalNames, const std::vector<std::string>& displacementNames);

        LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices);
        LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~LModel();
        const auto& getMehes() const { return meshes; };
        void draw() override;

    protected:

        LModel(LApp* app, const std::string& path, size_t dummy);
        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex) override {}
        std::vector<Mesh> meshes;
    };
}
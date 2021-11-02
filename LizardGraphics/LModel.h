#pragma once
#include "LShape.h"
#include "LImage.h"
#include "LBuffer.h"
#include "LAnimation.h"

namespace LGraphics
{
    class LResourceManager;

#define CTOR_PATH_VARS const std::string& modelName, const std::string& diffuseName, \
     const std::string& normalName, const std::string& displacementName, const std::string& reflexName

#define CTOR_PATH_VARS_VEC const std::string& modelName, const std::vector<std::string>& diffuseNames, \
     const std::vector<std::string>& normalNames, const std::vector<std::string>& displacementNames, const std::vector<std::string>& reflexNames

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
        void setReflex(const TexturesData& data, size_t meshNum = 0);

        void setNormalMapping(bool normalMapping, size_t meshNum = 0);
        void setParallaxMapping(bool parallaxMapping, size_t meshNum = 0);
        void setReflexMapping(bool reflexMapping, size_t meshNum = 0);

        void setNormalMappingAllMeshes(bool normalMapping);
        void setParallaxMappingAllMeshes(bool parallaxMapping);
        void setReflexMappingAllMeshes(bool reflexMapping);

        bool getNormalMapping(size_t meshNum = 0)   const { return meshes[meshNum].image->getNormalMapping(); };
        bool getParallaxMapping(size_t meshNum = 0) const { return meshes[meshNum].image->getParallaxMapping(); };
        bool getReflexMapping(size_t meshNum = 0) const { return meshes[meshNum].image->getReflexMapping(); };

        LModel(LApp* app, ModelResource res, bool cropTextureCoords = false);
        LModel(LApp* app, CTOR_PATH_VARS, bool cropTextureCoords = false);
        LModel(LApp* app, CTOR_PATH_VARS_VEC, bool cropTextureCoords = false);

        LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices, bool cropTextureCoords = false);
        LModel(LApp* app, LImage::ImageResource res, const std::vector<Vertex>& vertices, 
            const std::vector<uint32_t>& indices);
        ~LModel();
        const auto& getMehes() const { return meshes; };
        void draw() override;

        const auto& getAnime() const { return animations;}

        bool getSpeedModifier() const;
        void setSpeedModifier(float speed);

        void playAnimation();
        void playAnimation(const std::string& name);
        void stopAnimation();
        void restartAnimation();

        //void setReflexSize(size_t reflexSize);
        size_t getReflexSize() const;

    protected:

        LModel(LApp* app, const std::string& path, bool cropTextureCoords, size_t dummy);
        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex) override {}
        std::vector<Mesh> meshes;

        std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;
        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }

        std::unordered_map<std::string,Animation> animations;
        Animator animator;
        glm::mat4 mTransformation;
 
        bool playAnimation_ = false;
        void init();

        size_t reflexSize = 1024;
        GLuint reflexCubeMap = UNINITIALIZED, depthMap = UNINITIALIZED, reflexFBO = UNINITIALIZED;
    };
}
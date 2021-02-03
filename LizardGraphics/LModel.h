#pragma once
#include "LShape.h"

namespace LGraphics
{
    class LModel : public LShape
    {
    public:

        enum TextureType
        {
            BASE_TEXTURE,
            NORMALS,
        };
        friend LResourceManager;

        const char* getObjectType() override { return "LModel"; }

        LModel(LApp* app, const char* modelPath, const char* texturePath = nullptr, 
            const char* normalsPath = nullptr, bool debugInfo = false);
        ~LModel();
        
        void loadTexture(const char* path, TextureType type);

        void setMeshDrawing(size_t num, bool draw);
        bool getMeshDrawing(size_t num) const { return meshesToDraw[num]; }
        size_t getMeshesCount() const { return meshesCount; }

#ifdef VULKAN
        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex, size_t objectNum) override;
#endif
    protected:

        void loadModel(const char* modelPath, bool debugInfo);

        bool mouseOnIt() override { return false; }
        glm::vec3 getCenter() const override { return glm::vec3(); }


        const char* modelPath;
        VkImageView* textures;
        bool* meshesToDraw = nullptr;
        size_t meshesCount = 0;
    };
}
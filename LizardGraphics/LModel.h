#pragma once
#include "LShape.h"
#include "LImage.h"

namespace LGraphics
{
    class LResourceManager;
    class LModel : public LShape
    {
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

        LModel(LApp* app, ModelResource res);
        ~LModel();
        const auto& getMehes() const { return meshes; };
        void draw() override;

    protected:

        void draw(VkCommandBuffer commandBuffers, uint32_t frameIndex) override {}
        std::vector<Mesh> meshes;
    };
}
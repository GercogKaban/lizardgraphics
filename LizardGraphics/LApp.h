#pragma once

#include <mutex>
#include <optional>
#include <map>
#include "ObjectPool.h"

#ifdef OPENGL
#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"
#endif
#include "include/glm/glm.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "LObject.h"

namespace LShaders
{
    class Shader;
}

namespace LGraphics
{
    namespace
    {
    }

    class LNonWidget;
    //class LShaders::Shader;
    class LResourceManager;
    class LWRectangle;
    class LShape;
    class LRectangleShape;
    class LBuffer;
    class LWidget;

    /*!
    @brief Класс приложения Lizard Graphics

    Объект этого класса должен объявляться перед любым из элементом 
    LGraphics, т.к. LApp инициализурент OpenGL, GLFW окно и некоторые 
    другие вспомогательные классы
    */
    class LApp : public LObject
    {
        friend LShape;
        friend LRectangleShape;
        friend LWRectangle;
        friend LShaders::Shader;
        friend LBuffer;
        friend LResourceManager;
        friend LWidget;

    public:
        
#ifdef OPENGL
        LApp();
#endif
#ifdef VULKAN
        LApp(size_t objectPoolSize);
#endif
        ~LApp();

        /*!
        @brief Бесконечный цикл

        Открывает окно, при этом запуская бесконечный цикл, в котором
        рисуются все объекты сцены. Выход из цикла осуществляется при закрытии окна.
        */
        void loop();

        /*!
        @brief Возвращает размеры окна (в пикселях).

        */
        szvect2 getWindowSize() const { return szvect2(width, height); }

        /*!
        @brief Возвращает дескриптор GLFW окна.

        */
        GLFWwindow* getWindowHandler() { return window_; }

        void lockFps(size_t fps_) { fpsLock = fps_; }

        void setResolution(size_t resolutionX, size_t resolutionY) { glfwSetWindowSize(window_, resolutionX, resolutionY); }
        void setMatrices(glm::mat4 view, glm::mat4 projection);

        void deleteWidget(LWidget* w);
        void removeWidget(LWidget* w);

        void refreshObjectMatrices();

        void setKeyCallback(std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> callback);
        void setMouseCallback(std::function<void(GLFWwindow* w, int button, int action, int mods)> callback);
        void setScrollCallback(std::function<void(GLFWwindow* window, double xoffset, double yoffset)>callback);

        std::vector<LWidget*>& getNonInterfaceObjects() { return nonInterfaceObjects; }
        LShaders::Shader* getStandartWorldObjShader() const;

        bool isPressed(int key);

        glm::mat4 getViewMatrix() const { return view; }
        glm::mat4 getProjectionMatrix() const { return projection; }

        std::mutex& getDrawingMutex() { return drawingMutex; }

        void setSleepTime(size_t milliseconds) { sleepTime = milliseconds;}
        size_t getSleepTime() const { return sleepTime; }

        void switchScreenMode();

        void setBeforeDrawingFunc(std::function<void()> func) { beforeDrawingFunc = func; }
        void setAfterDrawingFunc(std::function<void()> func) { afterDrawingFunc = func; }

        void setViewPoint(glm::vec3 view) { viewPoint = view; /*refreshCamera();*/ };
        void setViewRadius(float radius) { viewRadius = radius; /*refreshCamera(); refreshProjection();*/ }

        glm::vec3 getViewPoint() const { return viewPoint; }
        float getViewRadius() const { return viewRadius; }

        LShaders::Shader* getLightningShader() { return experimentalLightShader; }

        unsigned int getDepthMap() const { return depthMap; }
         
        glm::vec3 getLightPos() const { return lightPos; }
        glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }

        bool lightIsInited() const { return lightIsInited_; }
        void initLight() { lightIsInited_ = true; }

        fvect2 getMouseCoords() const { return mouseCoords; }

        ObjectPool<LWRectangle*> lwRectPool;

        glm::vec3 viewAxonometricVector = glm::vec3(1/sqrt(3));
        bool perspectiveProjection = false;

        std::vector<LNonWidget*> customObjects;
        glm::vec3 lightPos;

        size_t getObjectsLimit() const { return objectCountLim; }

        void setImgui(std::function<void()> func) { imgui = func; }

    protected:

        size_t objectCountLim;

        std::function<void()> imgui = []() {};
        fvect2 mouseCoords = fvect2(0.0f);

        bool lightIsInited_ = false;
        glm::mat4 lightSpaceMatrix;

        void setLightPos(glm::vec3 lightPos);
        void setLightSpaceMatrix();

        void initTextures(std::vector<LWidget*>& objects);
        void setMatrices();
        void addObject(LWidget* w);

        void refreshCamera();
        void refreshProjection();

        void init();
        void initLEngine();
        void initRenderer();
        void initImGui();

        void initTextures();

        void checkEvents();

        void releaseResources();

        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void character_callback(GLFWwindow* window, unsigned int codepoint);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


#ifdef VULKAN

    public:
        VkDevice getDevice() const { return g_Device; }
    protected:

        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        LShaders::Shader* baseShader;

        VkDescriptorPool descriptorPool;

        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        VkCommandPool commandPool;
        VkImageView dummyTexture;

        std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_LUNARG_monitor"
        };

        const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        bool checkValidationLayerSupport();

        void createInstance();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSurface();
        void createGraphicsPipeline();

        void createDescriptorSets();
        void createDescriptorSetLayout();

        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();
        void createUniformBuffers();
        void createDescriptorPool();
        //void createTextureImage(const char* path, VkImage& image, VkDeviceMemory& mem);

        void createImage(uint32_t width, uint32_t height, VkFormat format,
            VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
            VkImage& image, VkDeviceMemory& imageMemory);

        //void createTextureImageView();
        void createTextureSampler();
        void createImageView(VkImage image, VkFormat format, VkImageView& view);

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        template<typename T>
        void createBuffer(T* buffer, VkBuffer& dst, VkDeviceMemory& dstMem, VkBufferUsageFlags usage, size_t bufSize)
        {
            VkDeviceSize bufferSize = bufSize;

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(g_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, buffer, (size_t)bufferSize);
            vkUnmapMemory(g_Device, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, dst, dstMem);

            copyBuffer(stagingBuffer, dst, bufferSize);

            vkDestroyBuffer(g_Device, stagingBuffer, nullptr);
            vkFreeMemory(g_Device, stagingBufferMemory, nullptr);
        }

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        void mapUniformData();
        void updateUniformBuffer(uint32_t currentImage, uint32_t objectNum, LWidget* w);
        void updateTexture(VkImageView& view, uint32_t currentImage, uint32_t objectNum);

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();
        std::vector<const char*> getRequiredExtensions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) 
        {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }

        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        struct QueueFamilyIndices 
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool isComplete() 
            {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        //VkImage textureImage, testIm;
        //VkDeviceMemory textureImageMemory, testImMemory;
        //VkImageView textureImageView, testImView;
        VkSampler textureSampler;

        size_t currentFrame = 0;
        bool framebufferResized = false;

        const int MAX_FRAMES_IN_FLIGHT = 2;


        struct Vertex 
        {
            glm::vec3 pos;
            glm::vec2 texCoord;

            static VkVertexInputBindingDescription getBindingDescription()
            {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
                std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex, pos);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex, texCoord);
                return attributeDescriptions;
            }
        };

        struct BaseShaderConstants
        {
            alignas(16) glm::mat4 proj;
            alignas(16) glm::mat4 view;
        };

        struct BaseVertexUBuffer
        {
            alignas(16) glm::mat4 model = glm::mat4(1.0f);
            alignas(16) glm::vec4 color = glm::vec4(1.0f);
        };

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


        struct UboDataDynamicV 
        {
            glm::mat4 *model = nullptr;
        } testStructV;

        size_t dynamicAlignment;

        void* alignedAlloc(size_t size, size_t alignment)
        {
            void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
            data = _aligned_malloc(size, alignment);
#else
            int res = posix_memalign(&data, alignment, size);
            if (res != 0)
                data = nullptr;
#endif
            return data;
        }

        void alignedFree(void* data)
        {
#if	defined(_MSC_VER) || defined(__MINGW32__)
            _aligned_free(data);
#else
            free(data);
#endif
        }
#endif

        GLFWwindow* window_;

        glm::vec3 viewPoint = glm::vec3(14.0f, 14.0f, 0.0f);
        float viewRadius = 10.0f;
        std::vector<LWidget*> nonInterfaceObjects;

        int width, height;

        size_t fps = 0, prevFps = 0, fpsLock = SIZE_MAX, sleepTime = 0;

        LBuffer* standartRectBuffer;

        LShaders::Shader* standartInterfaceshader, *standartWorldObjShader, *checkMarkShader, *colorBarShader, *experimentalLightShader,
            *shadowMap,*defaultShader, *multi_shadowMap, *multi_defaultShader;

        glm::mat4 view, projection;

        std::mutex drawingMutex;
        std::map<int, bool> pressedKeys;

        bool fullscreen = false;

        void* uniformsMem[2] = { nullptr,nullptr };

        std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {};
        std::function<void(GLFWwindow* w, int button, int action, int mods)> mouseCallback = [](GLFWwindow* w, int button, int action, int mods) {};
        std::function<void(GLFWwindow* window, double xoffset, double yoffset)> scrollCallback = [](GLFWwindow* window, double xoffset, double yoffset) {};

        std::function<void()> beforeDrawingFunc = []() {};
        std::function<void()> afterDrawingFunc = []() {};

        unsigned int shadowWidth = 1024, shadowHeight = 1024;

        unsigned int depthMapFBO, depthMap;
        fvect4 borderColor = fvect4(1.0, 1.0, 1.0, 1.0);

        VkAllocationCallbacks*   g_Allocator = NULL;
        VkInstance               g_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
//        Vk
        VkDevice                 g_Device = VK_NULL_HANDLE;
        uint32_t                 g_QueueFamily = (uint32_t)-1;
        VkQueue                  g_Queue = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
        VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

        size_t minUniformBufferOffsetAlignment = 0;

        ImGui_ImplVulkanH_Window* wd;

        ImGui_ImplVulkanH_Window g_MainWindowData;
        int                      g_MinImageCount = 2;
        bool                     g_SwapChainRebuild = false;

        const VkFormat requestSurfaceImageFormat[4] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
        const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

        static void check_vk_result(VkResult err)
        {
            if (err == 0)
                return;
            fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
            if (err < 0)
                abort();
        }

#ifdef IMGUI_VULKAN_DEBUG_REPORT
        VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
        {
            (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
            fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
            return VK_FALSE;
        }
#endif // IMGUI_VULKAN_DEBUG_REPORT

        void setupVulkan();

        // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
        // Your real engine/app may not use them.
        void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);

        void CleanupVulkanWindow()
        {
            ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
        }

        void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
        void FramePresent(ImGui_ImplVulkanH_Window* wd);

        static void glfw_error_callback(int error, const char* description)
        {
            fprintf(stderr, "Glfw Error %d: %s\n", error, description);
        }
    };
}


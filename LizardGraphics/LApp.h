﻿#pragma once

#include <mutex>
#include <optional>
#include <unordered_map>
#include <stack>
#include <deque>
#include "LLogger.h"
#include "enums.h"
#include "LWidget.h"
//#include "LCube.h"

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include <android/log.h>
#include <android_native_app_glue.h>

static const char* tag_ = "Lizard_Graphics";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, tag_, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, tag_, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, tag_, __VA_ARGS__))

extern bool initialized_;
bool initialize(android_app* app);

// Functions interacting with Android native activity
void android_main(struct android_app* state);
void terminate(void);
void handle_cmd(android_app* app, int32_t cmd);

#endif

#include "ObjectPool.h"

#include "include/glm/glm.hpp"

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include "vulkan_wrapper.h"
#endif

#include "include/GLEW/glew.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "VulkanMemoryAllocator/src/vk_mem_alloc.h"

#include "LObject.h"

namespace LShaders
{
    class Shader;
    class VulkanShader;
    class OpenGLShader;
}

namespace LGraphics
{
    namespace
    {
    }

    class LNonWidget;
    class LResourceManager;
    class LWRectangle;
    class LShape;
    class LRectangleShape;
    class LBuffer;
    class LModelBuffer;
    class LWidget;
    class LModel;
    class ImGuiInterface;
    class LLogger;
    class LCircle;
    class LImage;
    class LCubeBuffer;
    class LCube;
    class LSkyBox;
    class LRectangleMirror;
    class InstantPoolCubes;

    struct LAppCreateInfo
    {
        RenderingAPI api = L_VULKAN;
        size_t wndWidth = 0, wndHeight = 0;
        size_t poolSize = 100;
        size_t sleepThread = 0;
        LStates vsync = L_FALSE;
        LStates saveObjects = L_FALSE;
        LStates loadObjects = L_FALSE;
        LStates lighting = L_FALSE;
        uint32_t anisotropy = 16;
        uint32_t MSAA = 0;
        LProjections projection = L_ORTHOGRAPHIC;
        uint8_t logFlags = 0;
        LStates redactorMode = L_FALSE;
    };

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
        friend LShaders::VulkanShader;
        friend LShaders::OpenGLShader;
        friend LBuffer;
        friend LModelBuffer;
        friend LResourceManager;
        friend LWidget;
        friend LModel;
        friend ImGuiInterface;
        friend LLogger;
        friend LCircle;
        friend LImage;
        friend LCubeBuffer;
        friend LCube;
        friend LSkyBox;
        friend LRectangleMirror;
        friend InstantPoolCubes;

    public:

        LApp(const LAppCreateInfo& info);
        ~LApp();

        LWidget* findByLID(int id);
        /*!
        @brief Бесконечный цикл

        Открывает окно, при этом запуская бесконечный цикл, в котором
        рисуются все объекты сцены. Выход из цикла осуществляется при закрытии окна.
        */
        virtual void loop();

        void emergencyStop(unsigned long code);
        void emergencyStop(std::exception& exception);

        void switchRendererTo(RenderingAPI api);
        void setAfterSwitchingFunc(std::function<void()> func) { afterSwitchingFunc = func; }
        const LAppCreateInfo& getAppInfo() const { return info; }

        const int* getObjectsOnScreen() const { return objectsOnScreen; }

    protected:

        void* buff;

        template <typename T>
        class LID
        {
        public:
            LID() {}
            T getNewID()
            {
                T ret;
                if (gaps.size())
                {
                    ret = gaps.front();
                    gaps.pop();
                }
                else
                    ret = lastId++;
                return ret;
            }
            void releaseID(T id)
            {
                gaps.push(id);
            }
        private:
            std::queue<T> gaps;
            T lastId = 0;
        };

        class InstantPoolCubes
        {
            friend LCube;
            friend LSkyBox;

            LApp* app;
            GLuint vbo;
            LWidget::WidgetUniforms* uniformBuffers;
            LCube** objs;
            size_t uniformBuffersSize;
            //std::vector<LWidget::WidgetUniforms> uniformBuffers;
            //std::vector<LCube*> objs;

        public:

            InstantPoolCubes(){}
            ~InstantPoolCubes();

            void setApp(LApp* app);
            void draw();
            void makeInstantObj(LCube* cube);
            void updateBuffer();
            void initPool();

        }cubeInstantPool;

        int* objectsOnScreen;
        LID<int> idManager;

        void loop_();
        void initApp_(const LAppCreateInfo& info);
        void initErrorRecovering();
        void updateShaders();
        void updateBuffers();
        void updateTextures();

        void drawScene();

    public:

        unsigned int cubeTexture;
        bool renderingToCube = false;

        /*!
        @brief Возвращает размеры окна (в пикселях).

        */
        glm::vec<2, size_t> getWindowSize() const;

        /*!
        @brief Возвращает дескриптор GLFW окна.

        */
        GLFWwindow* getWindowHandler() { return window_; }

        //void setResolution(size_t resolutionX, size_t resolutionY) { glfwSetWindowSize(window_, resolutionX, resolutionY); }
        void setMatrices(glm::mat4 view, glm::mat4 projection);

        template <typename C>
        LWidget* removeWidget(LWidget* w, C& collection)
        {
            for (size_t i = 0; i < collection.size(); ++i)
                if (collection[i] == w)
                {
                    collection.erase(collection.begin() + i);
                    break;
                }
            return w;
        }

        template <typename C>
        void deleteWidget(LWidget* w, C& collection)
        {
            delete removeWidget(w, collection);
        }

        template <typename C>
        void addObject(LWidget* w, std::vector<C*>& collection)
        {
            assert(collection.size() + 1 <= getPoolSize() && "error, pool overflowed!\n");
            collection.push_back(((C*)w));
        }

        void refreshObjectMatrices();

        std::vector<LWidget*>& getPrimitives() { return primitives; }
        std::vector<LModel*>& getModels() { return models; }

        LShaders::Shader* getStandartShader() const;

        bool isPressed(int key);

        glm::mat4 getViewMatrix() const { return view; }
        glm::mat4 getProjectionMatrix() const { return projection; }

        std::mutex& getDrawingMutex() { return drawingMutex; }

        void setSleepTime(size_t milliseconds) { sleepTime = milliseconds; }
        size_t getSleepTime() const { return sleepTime; }


        void lockFrontViewCamera() { cameraFrontViewLock = true; }
        void unlockFrontViewCamera() { cameraFrontViewLock = false; }
        bool isCameraFrontLocked() const { return cameraFrontViewLock; }

        glm::vec3 viewAxonometricVector = glm::vec3(1 / sqrt(3));

        void setClearColor(glm::vec4 clearColor);

        void setBeforeDrawingFunc(std::function<void()> func) { beforeDrawingFunc = func; }
        void setAfterDrawingFunc(std::function<void()> func) { afterDrawingFunc = func; }

        void setCameraPos(glm::vec3 pos) { cameraPos = pos; }
        void setCameraFront(glm::vec3 cameraFront) { this->cameraFront = cameraFront; }
        void setCameraUp(glm::vec3 cameraUp) { this->cameraUp = cameraUp; }
        void setViewRadius(float radius) { viewRadius = radius; /*refreshCamera(); refreshProjection();*/ }

        void drawUI(bool draw) { drawUI_ = draw; }

        void setCursorEnabling(bool cursorEnable);
        bool isCursorEnabled() const { return cursorEnabled; }

        glm::vec3 getCameraFront() const { return cameraFront; }
        glm::vec3 getCameraPos() const { return cameraPos; }
        glm::vec3 getCameraUp() const { return cameraUp; }
        float getViewRadius() const { return viewRadius; }

        const std::unique_ptr<LShaders::Shader>& getLightningShader() { return info.api == L_OPENGL ? openGLLightShader : lightShader; }
        //LShaders::Shader* getLightningShader() { return experimentalLightShader; }

        unsigned int getDepthMap() const { return depthMap; }

        void setLighting(LStates state) { info.lighting = state; }
        void setLightPos(glm::vec3 lightPos);
        glm::vec3 getLightPos() const { return lightPos; }
        glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }

        bool lightIsInited() const { return lightIsInited_; }
        void initLight() { lightIsInited_ = true; }

        glm::vec2 getMouseCoords() const { return mouseCoords; }

        ObjectPool<LWRectangle*> lwRectPool;

        std::vector<LNonWidget*> customObjects;
        glm::vec3 lightPos; //lightDir;

        size_t getPoolSize() const { return info.poolSize; }

        void setImgui(std::function<void()> func) { imgui = func; }
        void addObjectToDelete(LWidget* w, LTypes type) { toDelete.push(std::make_pair(w, type)); }
        void addObjectToCreate(LWidget* w, LTypes type) { toCreate.push(std::make_pair(w, type)); }

        void setUserMouseButtonCallback(std::function<void(GLFWwindow* window, int button, int action, int mods)> func);
        void setUserCursorCallback(std::function<void(GLFWwindow* window, double xpos, double ypos)> func);
        void setUserKeyCallback(std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> func);
        void setUserCharacterCallback(std::function<void(GLFWwindow* window, unsigned int codepoint)> func);
        void setUserScrollCallback(std::function<void(GLFWwindow* window, double xoffset, double yoffset)>func);

        uint8_t getLogFlags() const { return info.logFlags; }
        bool cursorModeSwitched = false;

        void setYaw(float yaw) { this->yaw = yaw; };
        float getYaw() const { return yaw; }

        void setPitch(float pitch) { this->pitch = pitch; };
        float getPitch() const { return pitch; }

    protected:
        static LAppCreateInfo info;

        LSkyBox* skybox = nullptr;

        bool drawUI_ = true;

        std::stack<std::pair<LWidget*, LTypes>> toDelete;
        std::stack<std::pair<LWidget*, LTypes>> toCreate;

        std::function<void()> imgui = []() {};
        glm::vec2 mouseCoords = glm::vec2(0.0f);
        glm::vec2 prevCoords = glm::vec2(0.0f);
        float yaw = -20.0f;
        float pitch = 10.0f;

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        bool cursorEnabled = true;
        bool cameraFrontViewLock = false;
        bool lightIsInited_ = false;

        bool prevCameraFrontDefined = false;
        glm::vec3 prevCameraFront = glm::vec3(0.0f);
        glm::mat4 lightSpaceMatrix;

        void setLightSpaceMatrix();

        void initTextures(std::vector<LWidget*>& objects);
        void setMatrices();

        void refreshCamera();
        void refreshProjection();

        void setupLG();
        void initLEngine();
        void initRenderer();

        void initOpenGL();
        void initVulkan();

        void setWindowCallbacks();

        void initTextures();

        void checkEvents();

        void releaseVulkanResources();
        void releaseOpenGLResources();
        void releaseGlfwResources();
        void releaseLGResources();
        void releaseResources();

        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void character_callback(GLFWwindow* window, unsigned int codepoint);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

        std::function<void(GLFWwindow* window, int button, int action, int mods)> userMouseButtonCallback =
            [](GLFWwindow* window, int button, int action, int mods) {};
        std::function<void(GLFWwindow* window, double xpos, double ypos)> userCursorCallback =
            [](GLFWwindow* window, double xpos, double ypos) {};
        std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> userKeyCallback =
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {};
        std::function<void(GLFWwindow* window, unsigned int codepoint)> userCharacterCallback =
            [](GLFWwindow* window, unsigned int codepoint) {};
        std::function<void(GLFWwindow* window, double xoffset, double yoffset)>
            userScrollCallback = [](GLFWwindow* window, double xoffset, double yoffset) {};

    public:
        VkDevice getDevice() const { return g_Device; }
    protected:

        struct DirLight
        {
            glm::vec3 direction;
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
        };

        struct UboDataDynamicV
        {
            glm::mat4* model = nullptr;
        };

        /*struct VulkanStruct
        {*/
        std::deque<size_t> indexGaps;

        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        VkDescriptorPool descriptorPool;

        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VmaAllocation> uniformBuffersMemory;
        size_t uniformElementSize;

        VkCommandPool commandPool;
        VkImageView dummyTexture;

        VkImage depthImage;
        VmaAllocation depthImageMemory;
        VkImageView depthImageView;
        VkFormat depthFormat;

        VmaAllocator allocator;

        VkRenderPass renderPass;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
        };

        const std::vector<const char*> deviceExtensions =
        { VK_KHR_SWAPCHAIN_EXTENSION_NAME,
          VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
          VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
        };

#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
        static const size_t possibleMipLevels = 15;
        VkSampler textureSamplers[possibleMipLevels];

        std::vector <VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        UboDataDynamicV testStructV;
        size_t dynamicAlignment;
        //} vulkanWrap;


        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        bool checkValidationLayerSupport();

        void createInstance();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSurface();
        void createGraphicsPipeline();
        void createAllocator();

        void createDescriptorSets();
        void createDescriptorSetLayout();

        void createFramebuffers();
        void createCommandPool();
        void createUniformBuffers();
        void createDescriptorPool();
        void createImageViews();

        void createRenderPass();

        void createDepthResources();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();
        bool hasStencilComponent(VkFormat format);

        void createImage(uint32_t width, uint32_t height, VkFormat format,
            VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
            VkImage& image, VmaAllocation& imageMemory, uint32_t mipLevels);

        void createTextureSampler(VkSampler& sampler, size_t mipLevels);
        void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& view, uint32_t miplevels);

        void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize offset);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t miplevels);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        template<typename T>
        void createBuffer(T* buffer, VkBuffer& dst, VmaAllocation& allocation, VkBufferUsageFlags usage, size_t bufSize)
        {
            VkDeviceSize bufferSize;
            VmaAllocation stagingAllocation;
            VkBuffer stagingBuffer;

            VmaStats stats;
            vmaCalculateStats(allocator, &stats);

            createBuffer(bufSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, dst, allocation);

            auto unusedBytes = stats.memoryType[VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT].unusedBytes;

            if (!unusedBytes)
                // 256 mb
                unusedBytes = 268435456;

            const auto batches = bufSize / unusedBytes;

            auto stageBufTransfer = [&](VkDeviceSize offset)
            {
                createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    stagingBuffer, stagingAllocation);

                void* data;
                vmaMapMemory(allocator, stagingAllocation, &data);
                memcpy(data, buffer + (size_t)offset, bufferSize);
                vmaUnmapMemory(allocator, stagingAllocation);

                copyBuffer(stagingBuffer, dst, bufferSize, offset);

                vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
            };

            for (size_t i = 0; i < batches; ++i)
            {
                bufferSize = unusedBytes;
                bufSize -= bufferSize;
                stageBufTransfer(i * unusedBytes);
            }
            if (bufSize)
            {
                bufferSize = bufSize;
                stageBufTransfer(batches * unusedBytes);
            }
        }

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
            VkBuffer& buffer, VmaAllocation& allocation);
        //void createBufferWithData(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
           // VmaAllocation& allocation, void* data);

        void mapUniformData();
        void updateUniformBuffer(uint32_t currentImage, uint32_t objectNum, LWidget* w);
        void updateTexture(VkImageView& view, uint32_t currentImage, uint32_t objectNum, size_t mipLevels);

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();
        std::vector<const char*> getRequiredExtensions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

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

        struct Vertex
        {
            glm::vec3 pos;
            glm::vec2 texCoord;
            glm::vec3 normals;

            static VkVertexInputBindingDescription getBindingDescription()
            {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
        };

        struct BaseShaderConstants
        {
            alignas(16) glm::mat4 projView;
        };

        struct ShaderConstants
        {
            alignas(16) glm::mat4 projView;
            alignas(16) glm::vec3 viewPos;

            alignas(16) glm::vec3 lightPos;
            //alignas(16) glm::vec3 lightColor;
            alignas(16) glm::vec3 ambient;
            alignas(16) glm::vec3 diffuse;
            //alignas(4) int lighting;
            //alignas(16) glm::vec3 specular;

            //alignas(4) float ambient;
            //alignas(4) float specular;
        };

        struct BaseVertexUBuffer
        {
            alignas(16) glm::mat4 model = glm::mat4(1.0f);
            alignas(16) glm::vec4 color = glm::vec4(1.0f);
            alignas(4) int materialNum = 0;
        };

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void* alignedAlloc(size_t size, size_t alignment)
        {
            void* data = nullptr;
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

        void handleSEH(const size_t& code);
        void handleCppException(std::exception& err);

        GLFWwindow* window_;

        glm::mat4 view, projection;

        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float viewRadius = 10.0f;

        std::vector<LWidget*> primitives;
        std::vector<LModel*> models;

        //int width, height;

        size_t sleepTime = 0;

        LBuffer* standartSkyBoxBuffer;
        LBuffer* standartRectBuffer;
        LBuffer* standartCubeBuffer;

        std::unique_ptr<LShaders::Shader> openGLLightShader, lightShader, skyBoxShader, skyBoxMirrorShader, shadowMapShader;

        std::mutex drawingMutex;
        std::unordered_map<uint32_t, bool> pressedKeys;

        bool fullscreen = false;

        void* uniformsMem[2] = { nullptr,nullptr };

        std::function<void()> beforeDrawingFunc = []() {};
        std::function<void()> afterDrawingFunc = []() {};
        std::function<void()> afterSwitchingFunc = []() {};

        uint32_t shadowWidth = 1024, shadowHeight = 1024;

        GLuint depthMapFBO, depthMap, ssbo;

        bool drawingInShadow;
        glm::vec4 borderColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

        VkAllocationCallbacks* g_Allocator = NULL;
        VkInstance               g_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
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
            PRINTLN("[vulkan] Debug report from ObjectType: ", std::to_string(objectType), "%i\nMessage: ", std::string(pMessage), "\n\n");
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
            PRINTLN("Glfw Error ", std::to_string(error), ": ", description);
        }

        static void glfw_error_callback_Vk(int error, const char* description)
        {
        }
    };
}


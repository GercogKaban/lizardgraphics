#pragma once
#define PARALLEL_UPDATE
#define MAX_LIGHTS 128

#include <mutex>
#include <optional>
#include <unordered_map>
#include <stack>
#include <deque>
#include <array>
#include "LLogger.h"
#include "enums.h"
#include "LWidget.h"

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

#include "vulkan_wrapper.h"
#endif

#include "ObjectPool.h"

#include "include/glm/glm.hpp"

#include "include/GLEW/glew.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "VulkanMemory/Allocator/include/vk_mem_alloc.h"

#include "include/GLFW/glfw3.h"

//#include "vulkan/vulkan.h"
//#include "include/GLEW/glew.h"

#include "AtlasGenerator.h"
#include "LObject.h"
#include "LLights.h"

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
    class LPlane;
    class LCone;
    class LTorus;
    class LSphere;
    class LIcosphere;
    class LCylinder;
    class LShape;
    class LImagedShape;
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
    class LPicking;

    struct LAppInitialCreateInfo
    {
        RenderingAPI api = L_OPENGL;
        RenderingTypes renderingType = FORWARD;
        size_t wndWidth = 0, wndHeight = 0;
        size_t sleepThread = 0;
        LStates vsync = L_FALSE;
        LStates saveObjects = L_FALSE;
        LStates loadObjects = L_FALSE;
        LStates picking = L_FALSE;
        LStates lighting = L_FALSE;
        uint32_t anisotropy = 16;
        uint32_t MSAA = 0;
        LProjections projection = L_ORTHOGRAPHIC;
        uint8_t logFlags = 0;
        LStates redactorMode = L_FALSE;
        size_t freeThreads = std::max((int)std::thread::hardware_concurrency() - 2, 1);
        ModelLoading loading = LGraphics::ModelLoading::QUALITY;
        QualityLevels texturesQuality = LGraphics::QualityLevels::AUTO;
        size_t shadowsWidth = 1024, shadowsHeight = 1024;
        std::filesystem::path resourceDir = "";
        LStates ssao = L_FALSE;
        LStates ssr = L_FALSE;
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
        friend LImagedShape;
        friend LPlane;
        friend LCone;
        friend LTorus;
        friend LSphere;
        friend LIcosphere;
        friend LCylinder;
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
        friend LLight;
        friend LSpotLight;
        friend LPointLight;
        friend LPicking;

    public:

        LApp(const LAppInitialCreateInfo& info);
        ~LApp();

        /*!
        @brief Бесконечный цикл

        Открывает окно, при этом запуская бесконечный цикл, в котором
        рисуются все объекты сцены. Выход из цикла осуществляется при закрытии окна.
        */
        virtual void loop();

        void emergencyStop(unsigned long code);
        void emergencyStop(std::exception& exception);

        const LAppInitialCreateInfo& getAppInfo() const { return info; }

        // нужен фикс
        float tesselationLevel = 2.0f;
        bool flag__ = false;

    protected:

        void setParallaxSelfShading(bool shading) { this->parallaxSelfShading = shading; }
        bool getParallaxSelfShading() const { return parallaxSelfShading; }

        void switchRendererTo(RenderingAPI api);
        void setAfterSwitchingFunc(std::function<void()> func) { afterSwitchingFunc = func; }

        static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) 
        {
            PRINTLN("[OpenGL Error](", std::to_string(type), ") " , message ,'\n');
        }

        GLuint currentDepthMap;

        bool parallaxSelfShading = true;
        uint32_t modelLoadingFlags = 0;
        std::unordered_map<QualityLevels, std::string> qualityDirectories;

        LPicking* picking = nullptr;
        void loop_();
        void initApp_(const LAppInitialCreateInfo& info);
        void initErrorRecovering();
        void updateShaders();
        void updateBuffers();
        void updateTextures();

        void renderSceneObjects();

        void mainPass();
        void shadowPass(LLight* l);
        void reflexPass(GLuint reflexFBO, size_t reflexSize, glm::vec3 position);
        void pickingPass(GLuint pickingFBO, int colorBuffer);
        void deferredRenderingPass();
        void postProcessingPass();

        void initBaseShaders();
        void initForwardRenderingShaders();
        void initDefferedRenderingShaders();

        void clearColor();
        void clearDepth();
        void clearColorDepth();

        static void errorCallback(int error, const char* description);

        bool isDirectoryChanged(const std::string& path, const std::string& cacheFile) const;
        void saveDirectoryChangedTime(const std::string& path, const std::string& filePath) const;

        std::string getRealDiffusePath() const;
        std::string getRealNormalPath() const;
        std::string getRealDisplacementPath() const;
        std::string getRealReflexPath() const;

        std::string getRealTexturesPath() const;
        std::string getRealModelsPath() const;

        std::time_t GetFileWriteTime(const std::filesystem::path& filename) const
        {
#if defined ( _WIN32 )
            {
                struct stat fileInfo;
                if (stat(filename.generic_string().c_str(), &fileInfo) != 0)
                    throw std::runtime_error("Failed to get last write time.");
                return fileInfo.st_mtime;
            }
#else
            {
                auto fsTime = std::filesystem::last_write_time(filename);
                return decltype (fsTime)::clock::to_time_t(fsTime);
            }
#endif
        }
        
        struct Megatexture
        {
            Atlas textureAtl = Atlas("textures/out.jpg");
            Atlas normalAtl = Atlas("textures/out.jpg");
            Atlas parallaxAtl = Atlas("textures/out.jpg");
            Atlas reflexAtl = Atlas("textures/out.jpg");
            std::unordered_map<std::string, std::pair<glm::vec2, glm::vec2>> subtextures, subtexturesNormal, subtexturesParallax,
                subtexturesReflex;
            GLuint id, idNormal, idParallax, idReflex; 
        } megatexture;

        void generateMegatexture(const std::string& texturesPath, Atlas& atl, const std::string& atlPath);
        void initMegatextureData(const Atlas& atl, std::unordered_map<std::string, std::pair<glm::vec2, glm::vec2>>& subtextures,
            GLuint megatextureId);

        LModel* cube, *plane, *sphere, *icosphere, *cone, *cylinder, *torus;

    public:

        // нужен фикс
        float heightScale = 0.03f;
        struct LFog
        {
            glm::vec3 color;
            float density;
            bool isEnabled = false;
        }fog;

        void initReflex(LModel* m);

        /*!
        @brief Возвращает размеры окна (в пикселях).

        */
        glm::vec<2, size_t> getWindowSize() const;

        /*!
        @brief Возвращает дескриптор GLFW окна.
        */
        static GLFWwindow* getWindowHandler() { return window_; }

        void setMatrices(glm::mat4 view, glm::mat4 projection);
        
        template<typename T>
        void fastErase(std::vector<T>& collection, size_t id)
        {
            const auto lastObjId = collection.size() - 1;
            if (lastObjId != id)
            {
                collection[id] = collection[lastObjId];
                collection[lastObjId]->id = id;
            }
            collection.pop_back();
        }

        void addLight(LLight* l);
        void removeLight(LLight* l);
        void deleteLight(LLight* l);

        void addObject(LImagedShape* w);
        void removeObject(LImagedShape* w);
        void deleteObject(LImagedShape* w);

        void addObject(LModel* w);
        void removeObject(LModel* w);
        void deleteObject(LModel* w);

        const auto& getPrimitives() const { return primitives; }
        const auto& getModels() const { return models; }

        void setTesselation(bool tesselation) { this->tesselation = tesselation; }
        bool getTesselation() const { return tesselation; }

        const auto& getSpotLights() const { return lights[L_SPOT_LIGHT]; }
        const auto& getPointLights() const { return lights[L_POINT_LIGHT]; }
        const auto& getDirectedLights() const { return lights[L_DIRECTIONAL_LIGHT]; }
        const auto& getLights() const {return lights;}

        LShaders::Shader* getStandartShader() const;
        LShaders::Shader* getStandartShaderTes() const;

        bool isPressed(int key);

        const glm::mat4& getViewMatrix() const { return view; }
        const glm::mat4& getProjectionMatrix() const { return projection; }

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

        const std::unique_ptr<LShaders::Shader>& getLightningShaderTes() { return info.api == L_OPENGL ? openGLLightShaderTes : lightShader; }
        const std::unique_ptr<LShaders::Shader>& getLightningShader() { return info.api == L_OPENGL ? openGLLightShader : lightShader; }

        glm::vec2 getMouseCoords() const { return mouseCoords; }

        //ObjectPool<LPlane*> lwRectPool;
        //std::vector<LNonWidget*> customObjects;

        void setImgui(std::function<void()> func) { imgui = func; }
        void safeDelete(LImagedShape* w) { toDelete.push(w); }

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

        float getCurrentFrame() const { return lastFrame; }
        float getDeltaTime() const { return deltaTime; }

        struct PixelInfo
        {
            uint32_t objectID;
            uint32_t primitiveNum;
            uint32_t triangleNum;
        };

        std::pair<LWidget*,PixelInfo> getObjectByMousePos(size_t x, size_t y);

    protected:

        struct FBOAttach
        {
            int textureType;
            int componentType;
            glm::ivec2 attachmentSize;
            int valuesType;
            GLuint drawBuffer;
            GLuint attachmentId;
        };

        struct RBOAttach
        {
            int componentType;
            glm::ivec2 attachmentSize;
            GLuint drawBuffer;
            GLuint fbo;
        };

        class PostProcessingFBO
        {
            friend LApp;
        public:
            PostProcessingFBO(glm::ivec2 bufferSize);
            ~PostProcessingFBO();
        protected:

            GLuint buffer, color, normal, depth;
            std::vector<GLuint> drawingBuffers;
        };

        PostProcessingFBO* postProcessingFBO = nullptr;

        class GBuffer
        {
            friend LApp;
        public:
            GBuffer(glm::ivec2 bufferSize);
            ~GBuffer();
        protected:

            GLuint gBuffer, gPosition, gNormal, gAlbedoSpec, rboDepth;
            std::vector<GLuint> drawingBuffers;
        };

        GBuffer* gBuffer = nullptr;

        class FullscreenQuad
        {
            friend LApp;
        public:
            void draw();

            FullscreenQuad();
            ~FullscreenQuad();
        protected:
            const float quadVertices[20] = 
            {
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };

            GLuint vao, vbo;
        } *fullscreenQuad = nullptr;

        PixelInfo readPixel(size_t x, size_t y, uint32_t fbo, int colorBuffer) const;
        LWidget* getObjectByPixel(const PixelInfo& pixelinfo) const;

        std::string loadingText;
        static LAppInitialCreateInfo info;

        LSkyBox* skybox = nullptr;

        bool drawUI_ = true;
        bool tesselation = false;

        std::stack<LImagedShape*> toDelete;
        std::stack<LImagedShape*> toCreate;

        std::stack<LModel*> toDeleteM;
        std::stack<LModel*> toCreateM;

        std::function<void()> imgui = []() {};
        glm::vec2 mouseCoords = glm::vec2(0.0f);
        glm::vec2 prevCoords = glm::vec2(0.0f);
        float yaw = -20.0f;
        float pitch = 10.0f;

        std::array<std::pair<float, float>, 3> lastLoadedRanges;

        glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);

        bool cursorEnabled = true;
        bool cameraFrontViewLock = false;

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        bool prevCameraFrontDefined = false;
        glm::vec3 prevCameraFront = glm::vec3(0.0f);

        std::deque<LLight*> lightsToInit;
        std::deque<LModel*> modelReflexesToInit;

        void setMatrices();

        void refreshCamera();
        void refreshProjection();

        void setupLG();
        void initLEngine();
        void initRenderer();

        void initOpenGL();
        void initVulkan();

        static FBOAttach createAttachment(const FBOAttach& attach);
        static GLuint createFramebuffer(const std::vector<FBOAttach>& attachments);
        static GLuint createRenderbuffer(const RBOAttach& attach);

        void setWindowCallbacks();

        void checkEvents();
        static void checkError();
        static void checkFramebufferError();

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

        struct UboDataDynamicV
        {
            glm::mat4* model = nullptr;
        };

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

        static GLFWwindow* window_;

        glm::mat4 view, projection;

        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float viewRadius = 10.0f;

        std::vector<LImagedShape*> primitives[7];

        std::vector<LLight*> lights[3];
        LLight* currentLight = nullptr;
        std::vector<LModel*> models;

        size_t sleepTime = 0;
        LBuffer* standartRectBuffer;

        std::unique_ptr<LShaders::Shader> openGLLightShader, openGLLightShaderTes, lightShader, skyBoxShader, 
        shadowMapShader, shadowMapModelShader, shadowCubeMapShader, shadowCubeMapModelShader, modelShader, reflexPrimitiveShader,
        reflexModelShader, pickingPrimitiveShader, pickingModelShader, deferredQuadShader, postProcessingShader;

        std::mutex drawingMutex;
        std::unordered_map<uint32_t, bool> pressedKeys;

        bool fullscreen = false;

        void* uniformsMem[2] = { nullptr,nullptr };

        std::function<void()> beforeDrawingFunc = []() {};
        std::function<void()> afterDrawingFunc = []() {};
        std::function<void()> afterSwitchingFunc = []() {};
        std::function<void()> loadingScreenFunc = customLoadingScreen;

        static void customLoadingScreen();
        std::thread* loadingScreen;

        bool drawingInShadow = false, drawingReflex = false, drawingPicking = false, deferredRendering = false;
        glm::vec3 reflexPos;
        glm::vec4 borderColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

        VkAllocationCallbacks*   g_Allocator = NULL;
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


#pragma once

#include <mutex>
#include <optional>

#include "include/GLEW/glew.h"
#include "include/GLFW/glfw3.h"
#include "include/glm/glm.hpp"
#include "Lshaders.h"
#include "LIRectangle.h"
#include "ObjectPool.h"

#include "LWRectangle.h"
//#include "LMultiWRectangle.h"

namespace LGraphics
{
    class LTextRender;
    class LColorBar;
    namespace
    {
    }

    class LNonWidget;
    class Text;
    class LShaders::Shader;

    /*!
    @brief Класс приложения Lizard Graphics

    Объект этого класса должен объявляться перед любым из элементом 
    LGraphics, т.к. LApp инициализурент OpenGL, GLFW окно и некоторые 
    другие вспомогательные классы
    */
    class LApp : public LObject
    {
        friend LRectangleShape;
        friend LColorBar;
        friend LTextRender;
        friend LShaders::Shader;

    public:
        
        LApp();
        ~LApp(){releaseResources();}

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
        GLFWwindow* getWindowHandler() { return window; }

        void addText(Text* text);
        //void addText(std::string text, fvect2 pos, float scale, fvect3 color, int refreshingMode);
        Text* popText(int refreshingMode);
        LWidget* getActiveWidget();

        void lockFps(size_t fps_) { fpsLock = fps_; }

        void setResolution(size_t resolutionX, size_t resolutionY) { glfwSetWindowSize(window, resolutionX, resolutionY); }
        void setActiveWidget(LWidget* w) { activeWidget = w; }
        void setMatrices(glm::mat4 view, glm::mat4 projection);
        void setWidgetsMovability(bool movability) { widgetsMovability = movability; }

        void deleteWidget(LWidget* w);
        void removeWidget(LWidget* w);

        void refreshObjectMatrices();

        void setKeyCallback(std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> callback);
        void setMouseCallback(std::function<void(GLFWwindow* w, int button, int action, int mods)> callback);
        void setScrollCallback(std::function<void(GLFWwindow* window, double xoffset, double yoffset)>callback);

        std::vector<LWidget*>& getInterfaceObjects(){ return interfaceObjects; }
        std::vector<LWidget*>& getNonInterfaceObjects() { return nonInterfaceObjects; }
        //std::vector<LWidget*>& getObjects() { return &objects; }
        //std::vector<Text>* getTextObjects() { return textObjects; }

        LShaders::Shader* getStandartWorldObjShader() const { return standartWorldObjShader; }
        LShaders::Shader* getStandartInterfaceShader() const { return standartInterfaceshader; }
        LShaders::Shader* getStandartCheckMarkShader() const { return checkMarkShader; }

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
        ObjectPool<LColorBar*> lColorBarPool;

        glm::vec3 viewAxonometricVector = glm::vec3(1/sqrt(3));
        bool perspectiveProjection = false;

        std::vector<LNonWidget*> customObjects;
        glm::vec3 lightPos;

    protected:

        fvect2 mouseCoords = fvect2(0.0f);

        bool lightIsInited_ = false;
        glm::mat4 lightSpaceMatrix;

        void setLightPos(glm::vec3 lightPos);
        void setLightSpaceMatrix();

        void initTextures(std::vector<LWidget*>& objects);
        void setMatrices();
        void addObject(LWidget* w, bool isInterfaceObj);
        void moveWidgetToMouse(LWidget* w);

        void refreshCamera();
        void refreshProjection();


        void init();
        void initLEngine();
        void initRenderer();

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
        VkDevice getDevice() const { return device; }
    protected:

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        VkDevice device;

        VkQueue graphicsQueue;
        VkSurfaceKHR surface;
        VkQueue presentQueue;

        LShaders::Shader* baseShader;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;

        VkCommandPool commandPool;

        VkBuffer vertexBuffer, indexBuffer;

        VkDeviceMemory vertexBufferMemory, indexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
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
        void createSwapChain();
        void createRenderPass();
        void createGraphicsPipeline();
        void createDescriptorSetLayout();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();
        void createSemaphores();
        void createSyncObjects();
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createTextureImage();
        void createImage(uint32_t width, uint32_t height, VkFormat format,
            VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
            VkImage& image, VkDeviceMemory& imageMemory);

        void createTextureImageView();
        void createTextureSampler();
        VkImageView createImageView(VkImage image, VkFormat format);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        template<typename T>
        void createBuffer(T& buffer, VkBuffer& dst, VkDeviceMemory& dstMem, VkBufferUsageFlags usage)
        {
            VkDeviceSize bufferSize = sizeof(buffer[0]) * buffer.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, buffer.data(), (size_t)bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, dst, dstMem);

            copyBuffer(stagingBuffer, dst, bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        void recreateSwapChain();
        void cleanupSwapChain();

        void drawFrame();
        void updateUniformBuffer(uint32_t currentImage);

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

        void createImageViews();
        std::vector<VkImageView> swapChainImageViews;

        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;


        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;

        size_t currentFrame = 0;
        bool framebufferResized = false;

        const int MAX_FRAMES_IN_FLIGHT = 2;

        struct Vertex 
        {
            glm::vec2 pos;
            glm::vec3 color;
            glm::vec2 texCoord;

            static VkVertexInputBindingDescription getBindingDescription() {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
                std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex, pos);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex, color);

                attributeDescriptions[2].binding = 0;
                attributeDescriptions[2].location = 2;
                attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

                return attributeDescriptions;
            }
        };

        struct UniformBufferObject {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 proj;
        };


        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        };

        const std::vector<uint16_t> indices = {
           0, 1, 2, 2, 3, 0
        };


        //static VkVertexInputBindingDescription getBindingDescription();

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

#endif

        GLFWwindow* window;
        LTextRender* textRenderer;

        glm::vec3 viewPoint = glm::vec3(14.0f, 14.0f, 0.0f);
        float viewRadius = 10.0f;
        std::vector<LWidget*> interfaceObjects;
        std::vector<LWidget*> nonInterfaceObjects;

        //std::vector<LWidget*> objects;
        std::vector<Text*> textObjects;

        int width, height;

        size_t fps = 0, prevFps = 0, fpsLock = SIZE_MAX, sleepTime = 0;

        LWidget* activeWidget = nullptr;
        //LWidget* prevActiveWidget = nullptr;
        LWidget* widgetToMove = nullptr;
        bool widgetsMovability = false;

        LBuffer* standartRectBuffer;

        LShaders::Shader* standartInterfaceshader, *standartWorldObjShader, *checkMarkShader, *colorBarShader, *experimentalLightShader,
            *shadowMap,*defaultShader, *multi_shadowMap, *multi_defaultShader;

        glm::mat4 view, projection;

        std::mutex drawingMutex;
        std::map<int, bool> pressedKeys;

        bool fullscreen = false;

        std::function<void(GLFWwindow* window, int key, int scancode, int action, int mods)> keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {};
        std::function<void(GLFWwindow* w, int button, int action, int mods)> mouseCallback = [](GLFWwindow* w, int button, int action, int mods) {};
        std::function<void(GLFWwindow* window, double xoffset, double yoffset)> scrollCallback = [](GLFWwindow* window, double xoffset, double yoffset) {};

        std::function<void()> beforeDrawingFunc = []() {};
        std::function<void()> afterDrawingFunc = []() {};

        unsigned int shadowWidth = 1024, shadowHeight = 1024;

        unsigned int depthMapFBO, depthMap;
        fvect4 borderColor = fvect4(1.0, 1.0, 1.0, 1.0);


        //bool refreshStaticText = false;
    };
}


#include <sstream>

#include "Lshaders.h"
#include "LApp.h"
#include "LLogger.h"
#include "constants.h"

VkShaderModule LShaders::VulkanShader::genShaderModule(const char* shader, size_t size)
{
    LOG_CALL
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shader);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(app->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("failed to create shader module!");
    return shaderModule;
}

LShaders::VulkanShader::VulkanShader(LGraphics::LApp* app, const char* vertPath, const char* fragPath)
    :Shader(app)
{
    LOG_CALL
    size_t vSize, fSize;
    auto vertexShader = loadShader(vertPath, vSize);
    if (!vSize)
        std::runtime_error("can't load vertex shader by " + std::string(vertPath) + " path!");
    auto framgentShader = loadShader(fragPath, fSize);
    if (!fSize)
        std::runtime_error("can't load fragment shader by " + std::string(fragPath) + " path!");
    initShaders(vertexShader, framgentShader, vSize, fSize);
}

LShaders::OpenGLShader::OpenGLShader(LGraphics::LApp* app, const char* vShader, const char* fShader, const char* tessControlShader,
    const char* tessEvalShader, const char* geometryShader)
    :Shader(app)
{
    LOG_CALL
    size_t size;
    auto vertexShader = loadShader(vShader, size);
    if (!size)
        std::runtime_error("can't load vertex shader by " + std::string(vShader) + " path!");
    auto framgentShader = loadShader(fShader, size);
    if (!size)
        std::runtime_error("can't load fragment shader by " + std::string(fShader) + " path!");
    auto tcShader = loadShader(tessControlShader, size);
    if (!size && strlen(tcShader))
        std::runtime_error("can't load tesselation control shader by " + std::string(tessControlShader) + " path!");
    auto tesShader = loadShader(tessEvalShader, size);
    if (!size && strlen(tesShader))
        std::runtime_error("can't load tesselation evaluation by " + std::string(tessEvalShader) + " path!");
    auto geomShader = loadShader(geometryShader, size);
    if (!size && strlen(geometryShader))
        std::runtime_error("can't load geometry shader by " + std::string(geometryShader) + " path!");


    initShaders(vertexShader, framgentShader, tcShader, tesShader, geomShader);
}

LShaders::VulkanShader::~VulkanShader()
{
    LOG_CALL
    vkDestroyPipeline(app->g_Device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(app->g_Device, pipelineLayout, nullptr);
}

void LShaders::VulkanShader::initShaders(const char* v_shader, const char* f_shader, size_t vSize, size_t fSize)
{
    LOG_CALL

    VkShaderModule vertShader = genShaderModule(v_shader, vSize);
    VkShaderModule fragShader = genShaderModule(f_shader, fSize);

    delete[] v_shader;
    delete[] f_shader;

    //struct SpecializationData
    //{
    //    uint32_t materialsCount;
    //    LGraphics::Material* materials;
    //}data;

    //data.materialsCount = app->info.materials.size();
    //data.materials = app->info.materials.data();

    //std::array<VkSpecializationMapEntry, 2> specializationMapEntries;
    //specializationMapEntries[0].constantID = 0;
    //specializationMapEntries[0].size = sizeof(data.materialsCount);
    //specializationMapEntries[0].offset = 0;

    //specializationMapEntries[1].constantID = 1;
    //specializationMapEntries[1].size = data.materialsCount * sizeof(LGraphics::Material);
    //specializationMapEntries[1].offset = sizeof(uint32_t);

    /*VkSpecializationInfo specializationInfo{};
    specializationInfo.dataSize = data.materialsCount * sizeof(LGraphics::Material) + sizeof(uint32_t);
    specializationInfo.mapEntryCount = static_cast<uint32_t>(specializationMapEntries.size());
    specializationInfo.pMapEntries = specializationMapEntries.data();
    specializationInfo.pData = &data;*/

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShader;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShader;
    fragShaderStageInfo.pName = "main";
    //fragShaderStageInfo.pSpecializationInfo = &specializationInfo;

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = LGraphics::LApp::Vertex::getBindingDescription();
    auto attributeDescriptions = LGraphics::LApp::Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)app->wd->Width;
    viewport.height = (float)app->wd->Height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    auto swapChainSupport = app->querySwapChainSupport(app->g_PhysicalDevice);
    VkExtent2D extent = app->chooseSwapExtent(swapChainSupport.capabilities);
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    //rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.blendEnable = VK_TRUE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &app->descriptorSetLayout;

    VkPushConstantRange push_constant;
    push_constant.offset = 0;
    push_constant.size = sizeof(LGraphics::LApp::ShaderConstants);
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    pipelineLayoutInfo.pPushConstantRanges = &push_constant;
    pipelineLayoutInfo.pushConstantRangeCount = 1;

    if (vkCreatePipelineLayout(app->g_Device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
       throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = app->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.pDepthStencilState = &depthStencil;

    if (vkCreateGraphicsPipelines(app->g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(app->g_Device, vertShader, nullptr);
    vkDestroyShaderModule(app->g_Device, fragShader, nullptr);
}

void LShaders::OpenGLShader::initShaders(const char* vShader, const char* fShader, const char* tessControlShader,
    const char* tessEvalShader, const char* geometryShader)
{
    LOG_CALL
    char infoLog[512];
    GLint success;
    const GLchar* vert = (GLchar*)vShader;
    const GLchar* frag = (GLchar*)fShader;
    const GLchar* tc = (GLchar*)tessControlShader;
    const GLchar* tes = (GLchar*)tessEvalShader;
    const GLchar* geom = (GLchar*)geometryShader;

    auto vertexProg = compileShader(vert,GL_VERTEX_SHADER);
    auto fragmentProg = compileShader(frag, GL_FRAGMENT_SHADER);
    auto tcProg = compileShader(tc, GL_TESS_CONTROL_SHADER);
    auto tesProg = compileShader(tes, GL_TESS_EVALUATION_SHADER);
    auto geomProg = compileShader(geom, GL_GEOMETRY_SHADER);

    this->program = glCreateProgram();
    glAttachShader(this->program, vertexProg);
    glAttachShader(this->program, fragmentProg);
    if (tcProg != UNINITIALIZED)
        glAttachShader(this->program, tcProg);
    if (tesProg != UNINITIALIZED)
        glAttachShader(this->program, tesProg);
    if (geomProg != UNINITIALIZED)
        glAttachShader(this->program, geomProg);

    glLinkProgram(this->program);
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->program, 512, NULL, infoLog);
        throw std::runtime_error("shader program linking failed!\n" + std::string(infoLog));
    }

    glDetachShader(this->program,vertexProg);
    glDetachShader(this->program, fragmentProg);
    if (tcProg != UNINITIALIZED)
        glDetachShader(this->program, tcProg);
    if (tesProg != UNINITIALIZED)
        glDetachShader(this->program, tesProg);
    if (geomProg != UNINITIALIZED)
        glDetachShader(this->program, geomProg);
}

void LShaders::OpenGLShader::checkCreation(GLuint shader, const std::string& shaderType) const
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        PRINTLN(infoLog);
        throw std::runtime_error(shaderType + " shader compilation failed!\n");
    }
}

GLuint LShaders::OpenGLShader::compileShader(const GLchar* source, int shaderType) const
{
    if (!source || !strlen(source))
        return -1;
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    switch (shaderType)
    {
    case GL_VERTEX_SHADER:
        checkCreation(shader,"vertex");
        break;
    case GL_FRAGMENT_SHADER:
        checkCreation(shader, "fragment");
        break;
    case GL_TESS_CONTROL_SHADER:
        checkCreation(shader, "tesselation control");
        break;
    case GL_TESS_EVALUATION_SHADER:
        checkCreation(shader, "tesselation evaluation");
        break;
    }
    return shader;
}

const char* LShaders::Shader::loadShader(const char* shader, size_t& shaderSize)
{
    LOG_CALL
    if (!shader || !strlen(shader))
    {
        shaderSize = 0;
        return "";
    }
    std::ifstream in(shader, std::ios::ate | std::ios::binary);

    if (!in.is_open())
        throw std::runtime_error("failed to open file!");

    size_t fileSize = (size_t)in.tellg();
    shaderSize = fileSize + (app->info.api == LGraphics::L_VULKAN ? 0 : 1);
    char* shaderSource = new char[shaderSize];
    in.seekg(0);
    in.read(shaderSource, shaderSize);
    in.close();
    if (app->info.api == LGraphics::L_OPENGL)
        shaderSource[shaderSize - 1] = '\0';
    return shaderSource;
}

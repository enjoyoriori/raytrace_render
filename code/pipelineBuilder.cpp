#include "pipelineBuilder.hpp"


void PipelineBuilder::clear(){
    shaderStages.clear();
    vertexInputInfo = vk::PipelineVertexInputStateCreateInfo();
    inputAssembly = vk::PipelineInputAssemblyStateCreateInfo();
    viewportState = vk::PipelineViewportStateCreateInfo();
    rasterizer = vk::PipelineRasterizationStateCreateInfo();
    depthStencil = vk::PipelineDepthStencilStateCreateInfo();
    multisampling = vk::PipelineMultisampleStateCreateInfo();
    colorBlending = vk::PipelineColorBlendStateCreateInfo();
    dynamicState = vk::PipelineDynamicStateCreateInfo();
    pipelineLayout = vk::UniquePipelineLayout();
}

vk::UniquePipeline PipelineBuilder::buildPipeline(vk::Device device, std::vector<vk::PipelineShaderStageCreateInfo> shaderStagesInput, uint32_t WIDTH, uint32_t HEIGHT){
    
    // 入力シェーダーステージを設定
    shaderStages = shaderStagesInput;

    //VertexInputStateの設定
    vk::VertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
    vk::VertexInputBindingDescription instanceBindingDescription = Object::getBindingDescription();

    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = Vertex::getAttributeDescriptions();
    std::vector<vk::VertexInputAttributeDescription> instanceAttributeDescriptions = Object::getAttributeDescriptions();

    std::vector<vk::VertexInputBindingDescription> bindingDescriptions = {bindingDescription, instanceBindingDescription};
    attributeDescriptions.insert(attributeDescriptions.end(), instanceAttributeDescriptions.begin(), instanceAttributeDescriptions.end());
    
    vertexInputInfo = vk::PipelineVertexInputStateCreateInfo(
        {},//flags
        bindingDescriptions.size(),//vertexBindingDescriptionCount
        bindingDescriptions.data(),//pVertexBindingDescriptions
        attributeDescriptions.size(),//vertexAttributeDescriptionCount
        attributeDescriptions.data()//pVertexAttributeDescriptions
    );

    //InputAssemblyStateの設定
    inputAssembly = vk::PipelineInputAssemblyStateCreateInfo(
        {},//flags
        vk::PrimitiveTopology::eTriangleList,//topology
        VK_FALSE//primitiveRestartEnable
    );

    vk::Viewport viewport(
        0.0f, 0.0f,
        static_cast<float>(WIDTH), static_cast<float>(HEIGHT),
        0.0f, 1.0f
    );

    vk::Rect2D scissor(
        {0, 0},
        {WIDTH, HEIGHT}
    );

    viewportState = vk::PipelineViewportStateCreateInfo(
        {},//flags
        1,//viewportCount
        &viewport,//pViewports
        1,//scissorCount
        &scissor//pScissors
    );

    rasterizer = vk::PipelineRasterizationStateCreateInfo(
        {},//flags
        VK_FALSE,//depthClampEnable
        VK_FALSE,//rasterizerDiscardEnable
        vk::PolygonMode::eFill,//polygonMode
        vk::CullModeFlagBits::eBack,//cullMode
        vk::FrontFace::eCounterClockwise,//frontFace
        VK_FALSE,//depthBiasEnable
        0.0f,//depthBiasConstantFactor
        0.0f,//depthBiasClamp
        0.0f,//depthBiasSlopeFactor
        1.0f//lineWidth
    );

    multisampling = vk::PipelineMultisampleStateCreateInfo(
        {},//flags
        vk::SampleCountFlagBits::e1,//rasterizationSamples
        VK_FALSE,//sampleShadingEnable
        1.0f,//minSampleShading
        nullptr,//pSampleMask
        VK_FALSE,//alphaToCoverageEnable
        VK_FALSE//alphaToOneEnable
    );

    vk::PipelineColorBlendAttachmentState colorBlendAttachment(
        VK_FALSE,//blendEnable
        vk::BlendFactor::eOne,//srcColorBlendFactor
        vk::BlendFactor::eZero,//dstColorBlendFactor
        vk::BlendOp::eAdd,//colorBlendOp
        vk::BlendFactor::eOne,//srcAlphaBlendFactor
        vk::BlendFactor::eZero,//dstAlphaBlendFactor
        vk::BlendOp::eAdd,//alphaBlendOp
        vk::ColorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)//colorWriteMask
    );

    colorBlending = vk::PipelineColorBlendStateCreateInfo(
        {},//flags
        VK_FALSE,//logicOpEnable
        vk::LogicOp::eCopy,//logicOp
        1,//attachmentCount
        &colorBlendAttachment,//pAttachments
        {0.0f, 0.0f, 0.0f, 0.0f}//blendConstants
    );

    //DynamicStateの設定
    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicState(
        {},//flags
        dynamicStates.size(),//dynamicStateCount
        dynamicStates.data()//pDynamicStates
    );

    vk::UniquePipelineLayout pipelineLayout = device.createPipelineLayoutUnique({});

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo(
        {},//flags
        shaderStages.size(),//stageCount
        shaderStages.data(),//pStages
        &vertexInputInfo, // pVertexInputState
        &inputAssembly, // pInputAssemblyState
        nullptr, // pTessellationState
        &viewportState, // pViewportState
        &rasterizer, // pRasterizationState
        &multisampling, // pMultisampleState
        nullptr, // pDepthStencilState
        &colorBlending, // pColorBlendState
        &dynamicState, // pDynamicState
        pipelineLayout.get(), // layout
        VK_NULL_HANDLE, // renderPass
        0, // subpass
        VK_NULL_HANDLE, // basePipelineHandle
        -1 // basePipelineIndex
    );

    vk::UniquePipeline pipeline = device.createGraphicsPipelineUnique(VK_NULL_HANDLE, pipelineCreateInfo).value;
    return pipeline;
}
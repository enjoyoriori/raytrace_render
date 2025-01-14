#include "app.hpp"

// VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

void Application::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void Application::initVulkan() {
    // インスタンスの初期化
    auto requiredLayers = { "VK_LAYER_KHRONOS_validation" };
    uint32_t instanceExtensionCount = 0;
    const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&instanceExtensionCount);
    vk::InstanceCreateInfo instCreateInfo(
        {},
        nullptr,
        requiredLayers.size(),
        requiredLayers.begin() ,
        instanceExtensionCount,
        requiredExtensions
    );

    instance = vk::createInstanceUnique(instCreateInfo);
    // 物理デバイスの初期化
    auto deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };    //拡張機能のリスト
    vk::PhysicalDeviceFeatures deviceFeatures = {}; // DeviceFeaturesの設定
    deviceFeatures.geometryShader = VK_TRUE;

    // 物理デバイスの選択
    physicalDevice = pickPhysicalDevice(deviceExtensions, deviceFeatures);

    // デバイスの初期化
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = findQueues();

    vk::DeviceCreateInfo deviceCreateInfo(
        {},
        static_cast<uint32_t>(queueCreateInfos.size()),
        queueCreateInfos.data(),
        0,
        nullptr,
        deviceExtensions.size(),
        deviceExtensions.begin(),
        &deviceFeatures
    );
    device = physicalDevice.createDeviceUnique(deviceCreateInfo);

    // キューの取得
    for(uint32_t i = 0 ; i<queueCreateInfos[0].queueCount ; i++) {//グラフィックスキューの取得
        graphicsQueues.push_back(device->getQueue(queueCreateInfos[0].queueFamilyIndex, i));
    }

    for(uint32_t i = 0 ; i<queueCreateInfos[1].queueCount ; i++) {//コンピュートキューの取得
        computeQueues.push_back(device->getQueue(queueCreateInfos[1].queueFamilyIndex, i));
    }

    // コマンドプールの作成
    vk::CommandPoolCreateInfo graphicCommandPoolCreateInfo({}, queueCreateInfos[0].queueFamilyIndex);
    graphicCommandPool = device->createCommandPoolUnique(graphicCommandPoolCreateInfo);
    vk::CommandPoolCreateInfo computeCommandPoolCreateInfo({}, queueCreateInfos[1].queueFamilyIndex);
    computeCommandPool = device->createCommandPoolUnique(computeCommandPoolCreateInfo);

    // コマンドバッファの作成
    vk::CommandBufferAllocateInfo graphicCmdBufAllocInfo(graphicCommandPool.get(), vk::CommandBufferLevel::ePrimary, 1);
    graphicCommandBuffers = device->allocateCommandBuffersUnique(graphicCmdBufAllocInfo);
    vk::CommandBufferAllocateInfo computeCmdBufAllocInfo(computeCommandPool.get(), vk::CommandBufferLevel::ePrimary, 1);
    computeCommandBuffers = device->allocateCommandBuffersUnique(computeCmdBufAllocInfo);

    // イメージの作成
    image = createImage(WIDTH, HEIGHT, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
    
    // シェーダーモジュールの作成
    vk::UniqueShaderModule vertShaderModule = createShaderModule("../shaders/shader.vert.spv");
    vk::UniqueShaderModule fragShaderModule = createShaderModule("../shaders/shader.frag.spv");

    // パイプラインの作成
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertShaderModule.get(), "main"),
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragShaderModule.get(), "main")
    };

    pipelineBuilder = std::make_unique<PipelineBuilder>();
    pipeline = pipelineBuilder->buildPipeline(device.get(), shaderStages, WIDTH, HEIGHT);
}

//物理デバイスの選択
vk::PhysicalDevice Application::pickPhysicalDevice(const std::vector<const char*>& deviceExtensions, vk::PhysicalDeviceFeatures deviceFeatures) {
    for (const auto& device : instance->enumeratePhysicalDevices()) {
        if(checkDeviceExtensionSupport(device, deviceExtensions) && checkDeviceFeatures(device, deviceFeatures)) {
            return device;
        }
    }
    throw std::runtime_error("適切な物理デバイスが見つかりませんでした");
}

//物理デバイスのextensionをチェック
bool Application::checkDeviceExtensionSupport(vk::PhysicalDevice device,  const std::vector<const char*>& deviceExtensions) {
    std::set<std::string> requiredExtensions{deviceExtensions.begin(),
                                             deviceExtensions.end()};
    for (const auto& extension : device.enumerateDeviceExtensionProperties()) {
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

//物理デバイスのfeaturesをチェック
bool Application::checkDeviceFeatures(vk::PhysicalDevice device, vk::PhysicalDeviceFeatures requiredFeatures) {
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();
    if(!deviceFeatures.robustBufferAccess && requiredFeatures.robustBufferAccess) {
        return false;
    }
    else if(!deviceFeatures.geometryShader && requiredFeatures.geometryShader) {
        return false;
    }
    return true;
}

//キューの検索
std::vector<vk::DeviceQueueCreateInfo> Application::findQueues() {
    std::vector<vk::QueueFamilyProperties> queueProps = physicalDevice.getQueueFamilyProperties();
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    
    uint32_t graphicsQueueIndex = 0;
    uint32_t graphicsQueueCount = 0;
    uint32_t computeQueueIndex = 0;
    uint32_t computeQueueCount = 0;

    for(uint32_t i = 0; i < queueProps.size(); i++) {//キューを持つ数が最大のものを選択
        if(queueProps[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphicsQueueCount = std::max(graphicsQueueCount, queueProps[i].queueCount);
            if(graphicsQueueCount == queueProps[i].queueCount) {
                graphicsQueueIndex = i;
            }
        }
        else if(queueProps[i].queueFlags & vk::QueueFlagBits::eCompute) {
            computeQueueCount = std::max(computeQueueCount, queueProps[i].queueCount);
            if(computeQueueCount == queueProps[i].queueCount) {
                computeQueueIndex = i;
            }
        }
    }

    if(graphicsQueueIndex >= 0) {
        std::vector<float> queuePriorities(graphicsQueueCount, 0.0);
        for(uint32_t i = 0; i < graphicsQueueCount; i++) {
            queuePriorities[i] = (graphicsQueueCount - 1 - i) * 0.1f;
        }
        queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, 
                                                             graphicsQueueIndex, 
                                                             graphicsQueueCount, 
                                                             queuePriorities.data()));
    }
    if(computeQueueIndex >= 0 && computeQueueIndex != graphicsQueueIndex) {
        std::vector<float> queuePriorities(computeQueueCount, 0.0);
        for(uint32_t i = 0; i < computeQueueCount; i++) {
            queuePriorities[i] = (computeQueueCount - 1 - i) * 0.1f;
        }
        queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, 
                                                             computeQueueIndex, 
                                                             computeQueueCount, 
                                                             queuePriorities.data()));
    }
    else{
        throw std::runtime_error("適切なキューが見つかりませんでした");
    }
    return queueCreateInfos;
}    

//イメージの作成
vk::UniqueImage Application::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage) {
    vk::ImageCreateInfo imageCreateInfo(
        {},
        vk::ImageType::e2D,
        format,
        vk::Extent3D(width, height, 1),
        1,
        1,
        vk::SampleCountFlagBits::e1,
        tiling,
        usage,
        vk::SharingMode::eExclusive,
        0,
        nullptr,
        vk::ImageLayout::eUndefined
    );

    vk::UniqueImage image = device->createImageUnique(imageCreateInfo);

    //メモリの割り当て
    vk::MemoryRequirements memRequirements = device->getImageMemoryRequirements(image.get());
    vk::MemoryAllocateInfo allocInfo(
        memRequirements.size,
        findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
    );
    vk::UniqueDeviceMemory imageMemory = device->allocateMemoryUnique(allocInfo);
    device->bindImageMemory(image.get(), imageMemory.get(), 0);

    return image;
}

//メモリタイプの検索
uint32_t Application::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("適切なメモリタイプが見つかりませんでした");
}

vk::UniqueShaderModule Application::createShaderModule(std::string filename) {
    size_t spvFileSz = std::filesystem::file_size(filename);

    std::ifstream spvFile(filename, std::ios::binary);
    if (!spvFile.is_open()) {
        throw std::runtime_error("シェーダーファイルを開けませんでした: " + filename);
    }

    std::vector<char> spvFileData(spvFileSz);
    spvFile.read(spvFileData.data(), spvFileSz);
    if (!spvFile) {
        throw std::runtime_error("シェーダーファイルの読み込みに失敗しました: " + filename);
    }

    vk::ShaderModuleCreateInfo shaderCreateInfo{};
    shaderCreateInfo.codeSize = spvFileSz;
    shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(spvFileData.data());

    return device->createShaderModuleUnique(shaderCreateInfo);
}

void Application::mainLoop() {

}

void Application::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

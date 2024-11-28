#include "app.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

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

    for(uint32_t i = 0; i < queueProps.size(); i++) {
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

void Application::mainLoop() {

}

void Application::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
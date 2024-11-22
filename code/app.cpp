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
    auto deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    physicalDevice = pickPhysicalDevice(deviceExtensions);

}

//物理デバイスの選択
vk::PhysicalDevice Application::pickPhysicalDevice(const std::vector<const char*>& deviceExtensions) {
    for (const auto& device : instance->enumeratePhysicalDevices()) {
        if(checkDeviceExtensionSupport(device, deviceExtensions) && checkDeviceFeatures(device)) {
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
bool Application::checkDeviceFeatures(vk::PhysicalDevice device) {
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();
    if(!deviceFeatures.geometryShader) {
        return false;
    }
    return true;
}
    

void Application::mainLoop() {

}

void Application::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
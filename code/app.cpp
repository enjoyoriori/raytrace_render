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
    std::vector<vk::PhysicalDevice> physicalDevices = instance->enumeratePhysicalDevices();

}

void Application::mainLoop() {

}

void Application::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
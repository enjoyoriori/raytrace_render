#pragma once
#include "header.hpp"

class Application {

    public:
        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:
        const uint32_t WIDTH = 1920;
        const uint32_t HEIGHT = 1080;

        GLFWwindow* window;

        vk::UniqueInstance instance;
        vk::PhysicalDevice physicalDevice;

        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();

        //vulkan初期化用関数
        vk::PhysicalDevice pickPhysicalDevice(const std::vector<vk::PhysicalDevice>& physicalDevices, const std::vector<vk::PhysicalDeviceType>& deviceTypes);
};
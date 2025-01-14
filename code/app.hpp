#pragma once
#include "header.hpp"
#include "geometry.hpp"
#include "pipelineBuilder.hpp"

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
        vk::UniqueDevice device;
        std::vector<vk::Queue> graphicsQueues;
        std::vector<vk::Queue> computeQueues;

        vk::UniqueCommandPool graphicCommandPool;
        std::vector<vk::UniqueCommandBuffer> graphicCommandBuffers;
        vk::UniqueCommandPool computeCommandPool;
        std::vector<vk::UniqueCommandBuffer> computeCommandBuffers;

        vk::UniquePipeline pipeline;
        std::unique_ptr<PipelineBuilder> pipelineBuilder;

        //イメージ
        vk::UniqueImage image;

        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();

        //vulkan初期化用関数
        vk::PhysicalDevice pickPhysicalDevice(const std::vector<const char*>& deviceExtensions, vk::PhysicalDeviceFeatures deviceFeatures);
        bool checkDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*>& deviceExtensions);
        bool checkDeviceFeatures(vk::PhysicalDevice device, vk::PhysicalDeviceFeatures deviceFeatures);

        std::vector<vk::DeviceQueueCreateInfo> findQueues();
        
        //イメージの作成
        vk::UniqueImage createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage);
        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        //シェーダーモジュールの作成
        vk::UniqueShaderModule createShaderModule(std::string filename);
};
#include "header.hpp"
#ifndef APP_HPP
#define APP_HPP

class Application {

    public:
        void run();

    private:
        const uint32_t WIDTH = 1920;
        const uint32_t HEIGHT = 1080;

        GLFWwindow* window;

        vk::Instance instance;
        vk::PhysicalDevice physicalDevice;

        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();
        
};

#endif
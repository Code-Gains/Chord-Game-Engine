#pragma once
#include <VkBootstrap.h>
#include "Log.h"
#include "WindowGLFW.h"

namespace Engine {
    struct FrameData {

        VkCommandPool _commandPool;
        VkCommandBuffer _mainCommandBuffer;
    };
    constexpr unsigned int FRAME_OVERLAP = 2;

    class Core {
        std::unique_ptr<WindowGLFW> _window;
        void InitWindow();
        void InitVulkan();
        void InitSwapchain();
        void InitCommands();
        void InitSyncStructures();
        bool _appMinimized = false;
        bool _isInitialized = false;

        // Vulkan
        VkInstance _instance;// Vulkan library handle
        VkDebugUtilsMessengerEXT _debugMessenger;// Vulkan debug output handle
        VkPhysicalDevice _chosenGPU;// GPU chosen as the default device
        VkDevice _device; // Vulkan device for commands
        VkSurfaceKHR _surface; // Vulkan window surface

        VkSwapchainKHR _swapchain;
        VkFormat _swapchainImageFormat;

        std::vector<VkImage> _swapchainImages;
        std::vector<VkImageView> _swapchainImageViews;
        VkExtent2D _swapchainExtent;

        void CreateSwapchain(uint32_t width, uint32_t height);
	    void DestroySwapchain();

        // Graphics
        uint32_t _frameNumber = 0;
        FrameData _frames[FRAME_OVERLAP];
        FrameData& GetCurrentFrame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

        VkQueue _graphicsQueue;
        uint32_t _graphicsQueueFamily;

    public:
        Core() = default;
        void Init();
        void Run(); // main loop
        void Shutdown();
    };
} // namespace Engine
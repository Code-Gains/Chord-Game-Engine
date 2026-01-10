#pragma once
#include <VkBootstrap.h>
#include <deque>
#include <vector>
#include <functional>
#include "Log.h"
#include "WindowGLFW.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include "vk_mem_alloc.h"
#include "vk_types.h"
#pragma clang diagnostic pop
// #define VMA_IMPLEMENTATION
// #include "vk_mem_alloc.h"


namespace Engine {
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void push_function(std::function<void()>&& function) {
            deletors.push_back(function);
        }

        void flush() {
            // reverse iterate the deletion queue to execute all the functions
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
                (*it)(); //call functors
            }

            deletors.clear();
        }
    };

    struct FrameData {
        VkCommandPool _commandPool;
        VkCommandBuffer _mainCommandBuffer;
	    VkFence _renderFence;
        DeletionQueue _deletionQueue;
    };
    constexpr unsigned int FRAME_OVERLAP = 2; // Swapchain image count? TODO

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

        VmaAllocator _allocator;

        void CreateSwapchain(uint32_t width, uint32_t height);
        void RecreateSwapchain(uint32_t width, uint32_t height);
        void CleanupSwapchainResources();
	    void DestroySwapchain();

        // Graphics
        uint32_t _frameNumber = 0;
        FrameData _frames[FRAME_OVERLAP];
        FrameData& GetCurrentFrame() { return _frames[_frameNumber % FRAME_OVERLAP]; };
        std::vector<VkSemaphore> _imageAvailableSemaphores; // size = FRAME_OVERLAP
        std::vector<VkSemaphore> _renderFinishedSemaphores; // size = to swapchain image count

        VkQueue _graphicsQueue;
        uint32_t _graphicsQueueFamily;
        DeletionQueue _mainDeletionQueue;

        AllocatedImage _drawImage;
	    VkExtent2D _drawExtent;
        void Draw();

    public:
        Core() = default;
        void Init();
        void Run(); // main loop
        void Shutdown();
    };
} // namespace Engine
#pragma once
#include <VkBootstrap.h>
#include <deque>
#include <vector>
#include <functional>
#include "Log.h"
#include "WindowGLFW.h"
#include <filesystem>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include "vk_mem_alloc.h"
#include "vk_types.h"
#pragma clang diagnostic pop
// #define VMA_IMPLEMENTATION
// #include "vk_mem_alloc.h"


namespace Engine {


    struct GeoSurface {
        uint32_t startIndex;
        uint32_t count;
    };

    struct MeshAsset {
        std::string name;
    
        std::vector<GeoSurface> surfaces;
        GPUMeshBuffers meshBuffers;
    };


    typedef struct VkGraphicsPipelineCreateInfo {
        VkStructureType                                  sType;
        const void*                                      pNext;
        VkPipelineCreateFlags                            flags;
        uint32_t                                         stageCount;
        const VkPipelineShaderStageCreateInfo*           pStages;
        const VkPipelineVertexInputStateCreateInfo*      pVertexInputState;
        const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState;
        const VkPipelineTessellationStateCreateInfo*     pTessellationState;
        const VkPipelineViewportStateCreateInfo*         pViewportState;
        const VkPipelineRasterizationStateCreateInfo*    pRasterizationState;
        const VkPipelineMultisampleStateCreateInfo*      pMultisampleState;
        const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState;
        const VkPipelineColorBlendStateCreateInfo*       pColorBlendState;
        const VkPipelineDynamicStateCreateInfo*          pDynamicState;
        VkPipelineLayout                                 layout;
        VkRenderPass                                     renderPass;
        uint32_t                                         subpass;
        VkPipeline                                       basePipelineHandle;
        int32_t                                          basePipelineIndex;
    } VkGraphicsPipelineCreateInfo;
    

    struct ComputePushConstants {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };

    struct ComputeEffect {
        const char* name;

        VkPipeline pipeline;
        VkPipelineLayout layout;

        ComputePushConstants data;
    };
    
    struct DescriptorLayoutBuilder {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        void AddBinding(uint32_t binding, VkDescriptorType type);
        void Clear();
        VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
    };

    struct DescriptorAllocator {
        struct PoolSizeRatio{
            VkDescriptorType type;
            float ratio;
        };

        VkDescriptorPool pool;

        void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
        void ClearDescriptor(VkDevice device);
        void DestroyPool(VkDevice device);

        VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);
    };


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
        void InitPipelines();
	    void InitBackgroundPipelines();
        void InitDescriptors();
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

        void CreateDrawImages(uint32_t width, uint32_t height);
        void CleanupDrawImages();
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
        AllocatedImage _depthImage;
	    VkExtent2D _drawExtent;


        VkDescriptorSet _drawImageDescriptors;
        VkDescriptorSetLayout _drawImageDescriptorLayout;
        //VkPipeline _gradientPipeline;
	    VkPipelineLayout _gradientPipelineLayout;

        AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
        void DestroyBuffer(const AllocatedBuffer& buffer);
        GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);


        void Draw();
        void DrawUi();

        // Drawing helpers
        void DrawBackground(VkCommandBuffer cmd);
        void DrawGeometry(VkCommandBuffer cmd);
        void DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView);

        // Imgui
        VkFence _immFence;
        VkCommandBuffer _immCommandBuffer;
        VkCommandPool _immCommandPool;

        
        void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

        void InitImgui();

        // temp
        std::vector<ComputeEffect> _backgroundEffects;
        int _currentBackgroundEffect{0};
        VkPipelineLayout _trianglePipelineLayout;
        VkPipeline _trianglePipeline;

        void InitTrianglePipeline();

        VkPipelineLayout _meshPipelineLayout;
        VkPipeline _meshPipeline;

        GPUMeshBuffers rectangle;

        void InitMeshPipeline();
        void InitDefaultData();
        std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGltfMeshes(Core* engine, std::filesystem::path filePath);
        
        std::vector<std::shared_ptr<MeshAsset>> testMeshes;

    public:
        Core() = default;
        void Init();
        void Run(); // main loop
        void Shutdown();

        DescriptorAllocator globalDescriptorAllocator;
    };
} // namespace Engine
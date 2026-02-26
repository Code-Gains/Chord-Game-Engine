#include <chrono>
#include <thread>
#include <cmath>
#include "Core.h"
//#include "VkInit.h"
#include "vk_initializers.h"
#include "vk_images.h"
#include "vk_pipelines.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#pragma clang diagnostic pop

namespace Engine {
#ifdef DEBUG
    constexpr bool gUseValidationLayers = true; // TODO move to global settings later
#else
    constexpr bool gUseValidationLayers = false; // TODO move to global settings later
#endif
    // TODO Disable on NON Debug Builds

    void Core::InitWindow() {
        _window = std::make_unique<WindowGLFW>(1280, 720, "Vulkan Engine");
    }

    void Core::InitVulkan() {
        vkb::InstanceBuilder builder;
        // make the vulkan instance, with basic debug features
	    auto inst_ret = builder.set_app_name("Example Vulkan Application")
            .request_validation_layers(gUseValidationLayers)
            .use_default_debug_messenger()
            .require_api_version(1, 3, 0)
            .build();
        vkb::Instance vkb_inst = inst_ret.value();

	    // grab the instance 
	    _instance = vkb_inst.instance;
	    _debugMessenger = vkb_inst.debug_messenger;

        // GLFW Vulkan Surface
        _surface = _window->CreateAndGetWindowSurface(_instance);

        //vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        features.dynamicRendering = true;
        features.synchronization2 = true;

        //vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;


        //use vkbootstrap to select a gpu. 
        //We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features
        vkb::PhysicalDeviceSelector selector{ vkb_inst };
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 3)
            .set_required_features_13(features)
            .set_required_features_12(features12)
            .set_surface(_surface)
            .select()
            .value();


        //create the final vulkan device
        vkb::DeviceBuilder deviceBuilder{ physicalDevice };

        vkb::Device vkbDevice = deviceBuilder.build().value();

        // Get the VkDevice handle used in the rest of a vulkan application
        _device = vkbDevice.device;
        _chosenGPU = physicalDevice.physical_device;

        // use vkbootstrap to get a Graphics queue
        _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        _graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

         // initialize the memory allocator
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = _chosenGPU;
        allocatorInfo.device = _device;
        allocatorInfo.instance = _instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocatorInfo, &_allocator);

        _mainDeletionQueue.push_function([&]() {
            vmaDestroyAllocator(_allocator);
        });
    }

    void Core::InitSwapchain() {
        CreateSwapchain(_window->GetWidth(), _window->GetHeight());
            //draw image size will match the window
        VkExtent3D drawImageExtent = {
            static_cast<uint32_t>(_window->GetWidth()),
            static_cast<uint32_t>(_window->GetHeight()),
            1
        };

        //hardcoding the draw format to 32 bit float
        _drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        _drawImage.imageExtent = drawImageExtent;

        VkImageUsageFlags drawImageUsages{};
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkImageCreateInfo rimg_info = vkinit::image_create_info(_drawImage.imageFormat, drawImageUsages, drawImageExtent);

        //for the draw image, we want to allocate it from gpu local memory
        VmaAllocationCreateInfo rimg_allocinfo = {};
        rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        //allocate and create the image
        vmaCreateImage(_allocator, &rimg_info, &rimg_allocinfo, &_drawImage.image, &_drawImage.allocation, nullptr);

        //build a image-view for the draw image to use for rendering
        VkImageViewCreateInfo rview_info = vkinit::imageview_create_info(_drawImage.imageFormat, _drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_CHECK(vkCreateImageView(_device, &rview_info, nullptr, &_drawImage.imageView));

        //add to deletion queues
        _mainDeletionQueue.push_function([this]() {
            vkDestroyImageView(_device, _drawImage.imageView, nullptr);
            vmaDestroyImage(_allocator, _drawImage.image, _drawImage.allocation);
        });

    }

    void Core::InitCommands() {
        //create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
        VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        for (int i = 0; i < FRAME_OVERLAP; i++) {

            VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

            // allocate the default command buffer that we will use for rendering
            VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

            VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));
        }
    }

    void Core::InitSyncStructures() {
        //create syncronization structures
        //one fence to control when the gpu has finished rendering the frame,
        //and 2 semaphores to syncronize rendering with swapchain
        //we want the fence to start signalled so we can wait on it on the first frame
        VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
        VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();
        _imageAvailableSemaphores.resize(FRAME_OVERLAP);
        for (int i = 0; i < FRAME_OVERLAP; i++) {
            VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_frames[i]._renderFence));
            VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphores[i]));
            //VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._swapchainSemaphore));
            //VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));
        }

        // per swapchain image sync
        _renderFinishedSemaphores.resize(_swapchainImages.size());
        for (int i = 0; i < _swapchainImages.size(); i++) {
            VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderFinishedSemaphores[i]));
        }
    }

    void Core::CreateSwapchain(uint32_t width, uint32_t height) {
        vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

        _swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            //.use_default_format_selection()
            .set_desired_format(VkSurfaceFormatKHR{ .format = _swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            //use vsync present mode
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

        _swapchainExtent = vkbSwapchain.extent;
        // store swapchain and its related images
        _swapchain = vkbSwapchain.swapchain;
        _swapchainImages = vkbSwapchain.get_images().value();
        _swapchainImageViews = vkbSwapchain.get_image_views().value();
    }

     void Core::RecreateSwapchain(uint32_t width, uint32_t height) {
        // Destroy current swapchain
        vkDeviceWaitIdle(_device);

        // Cleanup resources tied to current swapchain
        CleanupSwapchainResources();

        // Create new swapchain
        CreateSwapchain(width, height);

        // Create new swapchain resources
        //uint32_t imageCount = 0;
        //vkGetSwapchainImagesKHR
    }

    void Core::CleanupSwapchainResources() {
        for (auto imageView : _swapchainImageViews) {
            vkDestroyImageView(_device, imageView, nullptr);
        }
        _swapchainImageViews.clear();

        if (_swapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(_device, _swapchain, nullptr);
            _swapchain = VK_NULL_HANDLE;
        }
    }

    void Core::DestroySwapchain() {
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);

        // destroy swapchain resources
        for (int i = 0; i < _swapchainImageViews.size(); i++) {
            vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
        }
    }

    void Core::InitPipelines()
    {
        InitBackgroundPipelines();
    }

    void Core::InitBackgroundPipelines()
    {
        VkPipelineLayoutCreateInfo computeLayout{};
        computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        computeLayout.pNext = nullptr;
        computeLayout.pSetLayouts = &_drawImageDescriptorLayout;
        computeLayout.setLayoutCount = 1;

        VK_CHECK(vkCreatePipelineLayout(_device, &computeLayout, nullptr, &_gradientPipelineLayout));
        VkShaderModule computeDrawShader;
        if (!vkutil::load_shader_module("C:\\Users\\CodeGains\\Documents\\Github\\DX11-Engine\\shaders\\gradient.comp.spv", _device, &computeDrawShader)) {
            ENGINE_LOG_ERROR("Error when building the compute shader");
        }

        VkPipelineShaderStageCreateInfo stageinfo{};
        stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageinfo.pNext = nullptr;
        stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stageinfo.module = computeDrawShader;
        stageinfo.pName = "main";

        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.pNext = nullptr;
        computePipelineCreateInfo.layout = _gradientPipelineLayout;
        computePipelineCreateInfo.stage = stageinfo;
        
        VK_CHECK(vkCreateComputePipelines(_device,VK_NULL_HANDLE,1,&computePipelineCreateInfo, nullptr, &_gradientPipeline));

        vkDestroyShaderModule(_device, computeDrawShader, nullptr);
        
        _mainDeletionQueue.push_function([&]() {
            vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
            vkDestroyPipeline(_device, _gradientPipeline, nullptr);
		});
    }

    void Core::InitDescriptors()
    {
        //create a descriptor pool that will hold 10 sets with 1 image each
        std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
        {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
        };

        globalDescriptorAllocator.InitPool(_device, 10, sizes);

        //make the descriptor set layout for our compute draw
        {
            DescriptorLayoutBuilder builder;
            builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
            _drawImageDescriptorLayout = builder.Build(_device, VK_SHADER_STAGE_COMPUTE_BIT);
        }

        //allocate a descriptor set for our draw image
        _drawImageDescriptors = globalDescriptorAllocator.Allocate(_device,_drawImageDescriptorLayout);	

        VkDescriptorImageInfo imgInfo{};
        imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        imgInfo.imageView = _drawImage.imageView;
        
        VkWriteDescriptorSet drawImageWrite = {};
        drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        drawImageWrite.pNext = nullptr;
        
        drawImageWrite.dstBinding = 0;
        drawImageWrite.dstSet = _drawImageDescriptors;
        drawImageWrite.descriptorCount = 1;
        drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        drawImageWrite.pImageInfo = &imgInfo;

        vkUpdateDescriptorSets(_device, 1, &drawImageWrite, 0, nullptr);

        //make sure both the descriptor allocator and the new layout get cleaned up properly
        _mainDeletionQueue.push_function([&]() {
            globalDescriptorAllocator.DestroyPool(_device);

            vkDestroyDescriptorSetLayout(_device, _drawImageDescriptorLayout, nullptr);
        });
    }

    void Core::Draw()
    {
        FrameData& frameData = GetCurrentFrame();
        // wait until the gpu has finished rendering
        VK_CHECK(vkWaitForFences(_device, 1, &frameData._renderFence, true, UINT64_MAX));
        GetCurrentFrame()._deletionQueue.flush();
        VK_CHECK(vkResetFences(_device, 1, &frameData._renderFence));

        // pick per frame semaphore
        VkSemaphore imageAvailableSemaphore = _imageAvailableSemaphores[_frameNumber % FRAME_OVERLAP];
	    uint32_t swapchainImageIndex;
        VkResult result = vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX,
            imageAvailableSemaphore, nullptr, &swapchainImageIndex);
        if (result == VK_SUBOPTIMAL_KHR) {
            RecreateSwapchain(_window->GetWidth(), _window->GetHeight());
            VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX,
            imageAvailableSemaphore, nullptr, &swapchainImageIndex));
        }

        // -----------------------------------------------------------------------
        VkCommandBuffer cmd = GetCurrentFrame()._mainCommandBuffer;
        // begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
        VkCommandBufferBeginInfo cmdBeginInfo =
            vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        // start the command buffer recording
        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        _drawExtent.width = _drawImage.imageExtent.width;
        _drawExtent.height = _drawImage.imageExtent.height;

        // transition our main draw image into general layout so we can write into it
        // we will overwrite it all so we dont care about what was the older layout
        vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        DrawBackground(cmd);

        //transition the draw image and the swapchain image into their correct transfer layouts
        vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // execute a copy from the draw image into the swapchain
        vkutil::copy_image_to_image(cmd, _drawImage.image, _swapchainImages[swapchainImageIndex], _drawExtent, _swapchainExtent);

        // set swapchain image layout to Present so we can show it on the screen
        vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        //finalize the command buffer (we can no longer add commands, but it can now be executed)
        VK_CHECK(vkEndCommandBuffer(cmd));
        // ---------------------------------------------------------------------------------

        // prepare the submission to the queue. 
        // we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
        // we will signal the _renderSemaphore, to signal that rendering has finished

        VkCommandBufferSubmitInfo cmdinfo = vkinit::command_buffer_submit_info(cmd);	
        
        VkSemaphore renderFinishedSemaphore = _renderFinishedSemaphores[swapchainImageIndex];
        VkSemaphoreSubmitInfo waitInfo =
            vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
            imageAvailableSemaphore);
        VkSemaphoreSubmitInfo signalInfo =
            vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            renderFinishedSemaphore); // always synchronize swapchain image with
                                                             // render semaphore	
        
        VkSubmitInfo2 submit = vkinit::submit_info(&cmdinfo, &signalInfo, &waitInfo);	
        // submit command buffer to the queue and execute it.
        // _renderFence will now block until the graphic commands finish execution
        VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, GetCurrentFrame()._renderFence));

        // prepare present
        // this will put the image we just rendered to into the visible window.
        // we want to wait on the _renderSemaphore for that, 
        // as its necessary that drawing commands have finished before the image is displayed to the user
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount = 1;
        //presentInfo.pNext = nullptr;
        presentInfo.pSwapchains = &_swapchain;
        presentInfo.pImageIndices = &swapchainImageIndex;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
        presentInfo.waitSemaphoreCount = 1;

        VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

        // increase the number of frames drawn
        _frameNumber++;
    }

    void Core::DrawBackground(VkCommandBuffer cmd)
    {

        // //make a clear-color from frame number. This will flash with a 120 frame period.
        // VkClearColorValue clearValue;
        // float flash = std::abs(std::sin(_frameNumber / 120.f));
        // clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

        // VkImageSubresourceRange clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
        
        // //clear image
        // vkCmdClearColorImage(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
        // bind the gradient drawing compute pipeline
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipeline);

        // bind the descriptor set containing the draw image for the compute pipeline
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipelineLayout, 0, 1, &_drawImageDescriptors, 0, nullptr);

        // execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
        vkCmdDispatch(cmd, std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);
    }

    void Core::Init()
    {
#ifdef DEBUG
        ENGINE_LOG_INFO("Engine initializing in DEBUG mode!");
    #else
        ENGINE_LOG_INFO("Engine initializing in RELEASE mode!");
    #endif
        // Sets up GLFW Window with Vulkan Prerequisites
        InitWindow();
        InitVulkan();
        InitSwapchain();
        InitCommands();
        InitSyncStructures();
        InitDescriptors();
        InitPipelines();

        //everything went fine
        _isInitialized = true;
    }

    void Core::Run() {
        constexpr auto targetMinimizedFrameDuration = std::chrono::milliseconds(50); // 20 FPS while minimized
        ENGINE_LOG_INFO("Starting Engine Main Loop.");
        bool running = true;
        while (!_window->ShouldClose()) {
            auto frameStartTime = std::chrono::high_resolution_clock::now();
            _window->PollEvents();

            // Update
            if (_window->WasResized()) {
                uint32_t width = _window->GetWidth();
                uint32_t height = _window->GetHeight();
                if (width == 0 || height == 0)
                    _appMinimized = true; // ignore swapchain
                else {
                    _appMinimized = false;
                    // resize swapchain
                    RecreateSwapchain(width, height);
                }
            }

            if (_appMinimized) {
                auto frameEndTime = std::chrono::high_resolution_clock::now();
                auto frameDuration = frameEndTime - frameStartTime;
                auto sleepDuration = targetMinimizedFrameDuration - frameDuration;

                if (sleepDuration > std::chrono::milliseconds(0))
                {
                    std::this_thread::sleep_for(sleepDuration);
                }
                continue;
            }
            Draw();
        }
        ENGINE_LOG_INFO("Engine Shutting Down.");
        Shutdown();
    }

    void Core::Shutdown() {
        if (!_isInitialized)
            return;

        vkDeviceWaitIdle(_device);

        // 1 destroy per frame resources
        for (int i = 0; i < FRAME_OVERLAP; i++) {
            // destroy command pool
            vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
            // destroy fence
            vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
            // deletion queuq
            _frames[i]._deletionQueue.flush();
        }
        // 2 destroy semaphores
        for (int i = 0; i < FRAME_OVERLAP; i++) {
            vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
        }

        for (int i = 0; i < _renderFinishedSemaphores.size(); i++) {
            vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
        }

        // 3 destroy main deletion queue
        _mainDeletionQueue.flush();

        // 4 destroy swapchain images
        DestroySwapchain();

        // 5 destroy surface and device
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        vkDestroyDevice(_device, nullptr);
    
        // 6 destroy debug messenger
        vkb::destroy_debug_utils_messenger(_instance, _debugMessenger);
        vkDestroyInstance(_instance, nullptr);
        //vkDestroyShaderModule(_device, computeDrawShader, nullptr);

        // 7 shutdown GLFW library
        _window->ShutdownGLFW();
    }

    void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type) {
        VkDescriptorSetLayoutBinding newbind {};
        newbind.binding = binding;
        newbind.descriptorCount = 1;
        newbind.descriptorType = type;

        bindings.push_back(newbind);
    }

    void DescriptorLayoutBuilder::Clear() {
        bindings.clear();
    }
    VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shaderStages, void *pNext, VkDescriptorSetLayoutCreateFlags flags) {
        for (auto& b : bindings) {
            b.stageFlags |= shaderStages;
        }

        VkDescriptorSetLayoutCreateInfo info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        info.pNext = pNext;

        info.pBindings = bindings.data();
        info.bindingCount = (uint32_t)bindings.size();
        info.flags = flags;

        VkDescriptorSetLayout set;
        VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

        return set;
    }

    void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;
        for (PoolSizeRatio ratio : poolRatios) {
            poolSizes.push_back(VkDescriptorPoolSize{
                .type = ratio.type,
                .descriptorCount = uint32_t(ratio.ratio * maxSets)
            });
        }

        VkDescriptorPoolCreateInfo pool_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        pool_info.flags = 0;
        pool_info.maxSets = maxSets;
        pool_info.poolSizeCount = (uint32_t)poolSizes.size();
        pool_info.pPoolSizes = poolSizes.data();

        vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
    }

    void DescriptorAllocator::ClearDescriptor(VkDevice device)
    {
        vkResetDescriptorPool(device, pool, 0);
    }

    void DescriptorAllocator::DestroyPool(VkDevice device)
    {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }

    VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
    {
        VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet ds;
        VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));

        return ds;
    }
    // void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
    // {
    //     std::vector<VkDescriptorPoolSize> poolSizes;
    //     for (PoolSizeRatio ratio : poolRatios) {
    //         poolSizes.push_back(VkDescriptorPoolSize{
    //             .type = ratio.type,
    //             .descriptorCount = uint32_t(ratio.ratio * maxSets)
    //         });
    //     }

    //     VkDescriptorPoolCreateInfo pool_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    //     pool_info.flags = 0;
    //     pool_info.maxSets = maxSets;
    //     pool_info.poolSizeCount = (uint32_t)poolSizes.size();
    //     pool_info.pPoolSizes = poolSizes.data();

    //     vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
    // }

    // void DescriptorAllocator::ClearDescriptor(VkDevice device)
    // {
    //     vkResetDescriptorPool(device, pool, 0);
    // }

    // void DescriptorAllocator::DestroyPool(VkDevice device)
    // {
    //     vkDestroyDescriptorPool(device,pool,nullptr);
    // }
    // VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
    // {
    //     VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    //     allocInfo.pNext = nullptr;
    //     allocInfo.descriptorPool = pool;
    //     allocInfo.descriptorSetCount = 1;
    //     allocInfo.pSetLayouts = &layout;

    //     VkDescriptorSet ds;
    //     VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));

    //     return ds;
    // }
} // namespace engine
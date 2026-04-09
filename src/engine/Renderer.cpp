#include "Renderer.h"
#include <VkBootstrap.h>
#include <stdexcept>

namespace Engine {

void Renderer::Init(VulkanDevice& vulkanDevice)
{
    // store Vulkan handles
    device = vulkanDevice.GetLogicalDevice();
    physicalDevice = vulkanDevice.GetPhysicalDevice();

    // Example: you can now access VkBootstrap device too
    vkb::Device vkbDevice = vulkanDevice.GetVkBootstrapDevice();

    // Normally here you would create swapchain, command buffers, render passes...
}

void Renderer::DrawFrame()
{
    // placeholder
}

void Renderer::Cleanup()
{
    // placeholder
}

} // namespace Engine

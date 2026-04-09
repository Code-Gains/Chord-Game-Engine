#pragma once
#include "VulkanDevice.h"
#include <vulkan/vulkan.hpp>

namespace Engine {

class Renderer {
public:
    Renderer() = default;
    void Init(VulkanDevice& device);
    void DrawFrame();
    void Cleanup();

private:
    vk::Device device;             // logical device from VulkanDevice
    vk::PhysicalDevice physicalDevice;
};

} // namespace Engine

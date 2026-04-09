#pragma once
#include <vulkan/vulkan.hpp>
#include <VkBootstrap.h>

namespace Engine {

class VulkanDevice {
public:
    VulkanDevice() = default;

    // Initialize Vulkan instance and logical device
    void Init();
    void Cleanup();

    // Getters
    vk::Instance GetVulkanInstance() const { return instance.instance; }
    vk::PhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
    vk::Device GetLogicalDevice() const { return device; }

    vkb::Device GetVkBootstrapDevice() const { return vkbDevice; }

private:
    vkb::Instance instance;      // VkBootstrap instance wrapper
    vkb::Device vkbDevice;       // VkBootstrap device wrapper
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
};

} // namespace Engine

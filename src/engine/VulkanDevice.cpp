#include "VulkanDevice.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h> // for vkDestroyInstance / vkDestroyDevice

namespace Engine {

void VulkanDevice::Init()
{
    // ----------------------------
    // 1) Create Vulkan instance using VkBootstrap
    // ----------------------------
    vkb::InstanceBuilder instBuilder;
    auto inst_ret = instBuilder
        .set_app_name("MyVulkanEngine")
        .request_validation_layers(true)
        .build();

    if (!inst_ret) {
        throw std::runtime_error("Failed to create Vulkan instance");
    }

    instance = inst_ret.value();

    // ----------------------------
    // 2) Pick physical device
    // ----------------------------
    vkb::PhysicalDeviceSelector physSelector{ instance };
    auto phys_ret = physSelector
        .set_minimum_version(1, 0)
        .select();

    if (!phys_ret) {
        throw std::runtime_error("Failed to select Vulkan physical device");
    }

    vkb::PhysicalDevice vkbPhysDevice = phys_ret.value();
    physicalDevice = vkbPhysDevice.physical_device;

    // ----------------------------
    // 3) Create logical device
    // ----------------------------
    vkb::DeviceBuilder devBuilder{ vkbPhysDevice };
    auto dev_ret = devBuilder.build();

    if (!dev_ret) {
        throw std::runtime_error("Failed to build Vulkan logical device");
    }

    vkbDevice = dev_ret.value();
    device = vkbDevice.device;

    // You can also query graphics queue if needed
    // auto graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
}

void VulkanDevice::Cleanup()
{
    // Destroy Vulkan logical device first
    if (vkbDevice.device) {
        vkDestroyDevice(vkbDevice.device, nullptr);
        vkbDevice.device = VK_NULL_HANDLE;
    }

    // Destroy Vulkan instance
    if (instance.instance) {
        vkDestroyInstance(instance.instance, nullptr);
        instance.instance = VK_NULL_HANDLE;
    }
}

} // namespace Engine

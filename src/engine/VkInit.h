#pragma once
#include <vulkan/vulkan.hpp>

namespace VkInit {

// Create info for a command pool
inline VkCommandPoolCreateInfo CommandPoolCreateInfo(
    uint32_t queueFamilyIndex,
    VkCommandPoolCreateFlags flags = 0)
{
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.queueFamilyIndex = queueFamilyIndex;
    info.flags = flags;
    return info;
}

// Allocate info for command buffers
inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
    VkCommandPool pool,
    uint32_t count = 1)
{
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return info;
}

} // namespace VkInit

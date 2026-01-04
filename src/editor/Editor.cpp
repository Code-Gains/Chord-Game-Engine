#include "engine/Core.h"
#include "engine/Log.h"
#include "engine/VulkanDevice.h"
#include "engine/Renderer.h"

int main() {
    ENGINE_LOG_INFO("Editor starting.");
    Engine::Core core;
    core.Init();
    core.Run();
    //Engine::VulkanDevice device;
    //Engine::Renderer renderer;
    // try {
    //     device.Init();
    //     renderer.Init(device);

    //     std::cout << "Vulkan + VkBootstrap initialized successfully!" << std::endl;

    //     // main loop placeholder
    //     // renderer.DrawFrame();

    //     renderer.Cleanup();
    //     device.Cleanup();
    // }
    // catch (const std::exception& e) {
    //     std::cerr << "ERROR: " << e.what() << std::endl;
    //     return 1;
    // }

    ENGINE_LOG_INFO("Editor finished.");
    return 0;
}

#define _CRT_SECURE_NO_WARNINGS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "vulkan/vulkan.hpp"
#include "iostream"
#include "src/easy3d.h"
#include "src/window/sdlwindow.h"

int main(int argc, char* argv[]) {
    try {
        auto& window = easy3d::Window::GetInstance();
        bool shouldClose = false;
        auto& event = window.enent;
        auto extensions = window.Get_SDL_Vulkan_GetInstanceExtensions();
        easy3d::Init(extensions, [&](vk::Instance instance) {
            VkSurfaceKHR surface;
            if (SDL_Vulkan_CreateSurface(window.window_, instance, &surface))
                return surface;
            throw std::runtime_error("can not create surface");
            }, window.window_);

        auto& renderer = easy3d::GetRenderer();
        while (!shouldClose) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    std::cout << "close" << std::endl;
                    shouldClose = true;
                }
            }
            renderer.Render();
        }
        easy3d::Quit();
    }
    catch(const std::exception& e){
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    return 0;

}


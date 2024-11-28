#pragma once
#include "sdl2/SDL.h"
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <memory>
#include <vector>
#include <iostream>

namespace easy3d {
	class Window {
	public:
		SDL_Window* window_;
		SDL_Event enent;
		std::vector<const char*> Get_SDL_Vulkan_GetInstanceExtensions();
		static Window& GetInstance(const char* title = "learn Vulkan", int width = 1000, int height = 600, Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

		~Window();
	private:
		Window(const char* title = "learn Vulkan", int width = 1000, int height = 600, Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
		static std::unique_ptr<Window> instance_;
	};
}
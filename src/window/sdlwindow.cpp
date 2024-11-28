#include "sdlwindow.h"

namespace easy3d {
	std::unique_ptr<Window> Window::instance_ = nullptr;
	Window::Window(const char* title, int width, int height, Uint32 flags)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

		if (!window_) {
			throw std::runtime_error("init sdl_window failed");
		}

	}
	Window::~Window()
	{
		SDL_DestroyWindow(window_);
		SDL_Quit();
	}
	std::vector<const char*> Window::Get_SDL_Vulkan_GetInstanceExtensions()
	{
		unsigned int extensioncount;
		SDL_Vulkan_GetInstanceExtensions(window_, &extensioncount, nullptr);
		std::vector<const char*> extensionnames(extensioncount);
		SDL_Vulkan_GetInstanceExtensions(window_, &extensioncount, extensionnames.data());
		return extensionnames;
	}
	Window& Window::GetInstance(const char* title, int width, int height, Uint32 flags)
	{
		static Window instance_(title, width, height, flags);
		return instance_;
	}
}
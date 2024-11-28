#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "vulkan/vulkan.hpp"
#include "context.h"
#include "shader/shader.h"
#include "renderer/renderer.h"
namespace easy3d {

	void Init(std::vector<const char*>& extensions, CreateSurfaceFunc func, SDL_Window* window);
	void Quit();
	Renderer& GetRenderer();
}
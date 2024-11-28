#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "vulkan/vulkan.hpp"
#include <functional>
#include <fstream>
#include <iostream>
namespace easy3d {
	using CreateSurfaceFunc = std::function<vk::SurfaceKHR(vk::Instance)>;
	std::string read_ShaderFile(const std::string& filename);
}
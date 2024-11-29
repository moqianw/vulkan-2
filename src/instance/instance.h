#pragma once

#include "vulkan/vulkan.hpp"

namespace easy3d {
	class Instance {
	public:
		vk::Instance instance;
		Instance();
		~Instance();
	private:
		VkDebugUtilsMessengerEXT debugMessenger;
		VkResult CreateDebugMessenger();
		void create_vkInstance();
	};
}
#pragma once

#include "vulkan/vulkan.hpp"

namespace easy3d {
	class Instance {
	public:
		vk::Instance instance;
		Instance();
		~Instance();
	private:
		void create_vkInstance();
	};
}
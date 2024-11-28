#pragma once

#include "vulkan/vulkan.hpp"
#include "../device/device.h"
namespace easy3d {
	class Buffer final {
	public:
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		Device& device;
		size_t size;
		Buffer(Device& device, size_t size,vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property);
		~Buffer();
	private:
		struct MemoryInfo {
			size_t size;
			uint32_t index;
		};
		void createBuffer(size_t size, vk::BufferUsageFlags usage);
		void allocateMemory(Buffer::MemoryInfo info);
		void bindingMemorytoBuffer();
		MemoryInfo quiryMemoryInfo(vk::MemoryPropertyFlags property);
	};
}
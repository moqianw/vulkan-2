#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>
#include "../queue/queue.h"
namespace easy3d {
	class Device {
	public:
		vk::Device device;
		vk::PhysicalDevice physicaldevice;
		Device();
		void pickupPhysicalDevice(const vk::Instance& instance);
		void createDevice(const Queue& queues);
		~Device();
	};
}
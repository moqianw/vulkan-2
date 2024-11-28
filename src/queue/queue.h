#pragma once

#include "vulkan/vulkan.hpp"
#include <vector>
namespace easy3d {
	class Queue {
	public:
		struct QueueFamilyIndex {
			std::optional<uint32_t> graphicsqueue;//��ͼ
			std::optional<uint32_t> presentqueue;//��ʾ

			operator bool() const {
				if (graphicsqueue.has_value() && presentqueue.has_value()) return true;
				return false;
			}
		};
		QueueFamilyIndex queuefamilyindex;
		std::vector<vk::Queue> queues;
		void queryQueueFamilyIndex(const vk::PhysicalDevice& physicaldevice, const vk::SurfaceKHR& surface);
		void getQueues(const vk::Device& device);

		Queue();
		~Queue();
	};
}
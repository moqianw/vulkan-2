#include "queue.h"
namespace easy3d {
	void Queue::queryQueueFamilyIndex(const vk::PhysicalDevice& physicaldevice, const vk::SurfaceKHR& surface)
	{
		auto properities = physicaldevice.getQueueFamilyProperties();
		for (int i = 0; i < properities.size(); i++) {
			auto& properity = properities[i];
			if (properity.queueFlags & vk::QueueFlagBits::eGraphics) {
				queuefamilyindex.graphicsqueue = i;
			}
			if (physicaldevice.getSurfaceSupportKHR(i, surface)) {
				queuefamilyindex.presentqueue = i;
			}
			if (queuefamilyindex) break;
		}
	}
	void Queue::getQueues(const vk::Device& device)
	{
		queues.push_back(device.getQueue(queuefamilyindex.graphicsqueue.value(), 0));//graphics
		queues.push_back(device.getQueue(queuefamilyindex.presentqueue.value(), 0));//present
	}
	Queue::Queue()
	{
	}
	Queue::~Queue()
	{
	}
}
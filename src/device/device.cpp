#include "device.h"
namespace easy3d {
	Device::Device()
	{
	}
	void Device::pickupPhysicalDevice(const vk::Instance& instance)
	{
		auto devices = instance.enumeratePhysicalDevices();
		for (auto& device : devices) {
			//auto feature = device.getFeatures();
			auto property = device.getProperties();
			std::cout << property.deviceName << std::endl;


		}
		if (devices.size())	physicaldevice = devices[0];
	}
	void Device::createDevice(const Queue& queues)
	{
		if (!queues.queuefamilyindex.graphicsqueue || !queues.queuefamilyindex.presentqueue) {
			throw std::runtime_error("Queue family indices are not set.");
		}
		std::array extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		vk::DeviceCreateInfo createinfo;
		std::vector<vk::DeviceQueueCreateInfo> queuecreateinfos;
		float priorite = 1.0f;
		vk::DeviceQueueCreateInfo queuecreateinfo;
		queuecreateinfos.push_back(vk::DeviceQueueCreateInfo().setPQueuePriorities(&priorite)
			.setQueueCount(1)
			.setQueueFamilyIndex(queues.queuefamilyindex.graphicsqueue.value()));
		if (queues.queuefamilyindex.graphicsqueue.value() != queues.queuefamilyindex.presentqueue.value()) {
			queuecreateinfos.push_back(vk::DeviceQueueCreateInfo().setPQueuePriorities(&priorite)
				.setQueueCount(1)
				.setQueueFamilyIndex(queues.queuefamilyindex.presentqueue.value()));
		}
		createinfo.setQueueCreateInfos(queuecreateinfos)
			.setPEnabledExtensionNames(extensions);
		try {
			device = physicaldevice.createDevice(createinfo);
		}
		catch (const std::exception& e) {
			throw std::runtime_error(std::string("Failed to create logical device: ") + e.what());
		}
	}
	Device::~Device()
	{
		device.destroy();
	}
}
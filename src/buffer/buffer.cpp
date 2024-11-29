#include "buffer.h"

namespace easy3d {
	Buffer::Buffer(Device& device, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property):device(device),size(size)
	{
		createBuffer(size, usage);
		allocateMemory(quiryMemoryInfo(property));
		bindingMemorytoBuffer();
		if (property & vk::MemoryPropertyFlagBits::eHostVisible) map = device.device.mapMemory(memory, 0, size);
		else map = nullptr;
	}
	Buffer::~Buffer()
	{
		device.device.destroyBuffer(buffer);
		device.device.freeMemory(memory);
	}
	void Buffer::createBuffer(size_t size, vk::BufferUsageFlags usage)
	{
		vk::BufferCreateInfo createinfo;
		createinfo.setSize(size)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive);
		buffer = device.device.createBuffer(createinfo);
	}
	void Buffer::allocateMemory(Buffer::MemoryInfo info)
	{
		vk::MemoryAllocateInfo alloinfo;
		alloinfo.setAllocationSize(info.size)
			.setMemoryTypeIndex(info.index);
		memory = device.device.allocateMemory(alloinfo);
	}
	void Buffer::bindingMemorytoBuffer()
	{
		device.device.bindBufferMemory(buffer, memory, 0);
	}
	Buffer::MemoryInfo Buffer::quiryMemoryInfo(vk::MemoryPropertyFlags property)
	{
		MemoryInfo info;
		auto requirements = device.device.getBufferMemoryRequirements(buffer);
		info.size = requirements.size;
		auto properties = device.physicaldevice.getMemoryProperties();
		for (int i = 0; i < properties.memoryTypeCount; i++) {
			if ((1 << i) & requirements.memoryTypeBits && properties.memoryTypes[i].propertyFlags & property) {
				info.index = i;
				break;
			}

		}
		return info;
	}
}
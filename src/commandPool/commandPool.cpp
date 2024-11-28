#include "commandPool.h"

namespace easy3d {
	CommandPool::CommandPool(vk::Device& device):device(device)
	{
		init_CommandPool();
	}
	CommandPool::~CommandPool()
	{
		device.destroyCommandPool(cmdPool_);
	}
	std::vector<vk::CommandBuffer> CommandPool::allocateCommandBuffer(uint32_t size)
	{
		vk::CommandBufferAllocateInfo cmdAllocInfo;
		cmdAllocInfo
			.setCommandPool(cmdPool_)
			.setCommandBufferCount(size)
			.setLevel(vk::CommandBufferLevel::ePrimary);
		return device.allocateCommandBuffers(cmdAllocInfo);
	}
	void CommandPool::freeCommandBuffer(vk::CommandBuffer& cmdBuffer)
	{
		device.freeCommandBuffers(cmdPool_, cmdBuffer);
	}
	void CommandPool::init_CommandPool()
	{
		vk::CommandPoolCreateInfo createinfo;
		createinfo
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		cmdPool_ = device.createCommandPool(createinfo);
	}
}
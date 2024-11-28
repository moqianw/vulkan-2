#include "vulkan/vulkan.hpp"

namespace easy3d {
	class CommandPool final {
	public:
		CommandPool(vk::Device& device);
		~CommandPool();
		std::vector<vk::CommandBuffer> allocateCommandBuffer(uint32_t size);
		void freeCommandBuffer(vk::CommandBuffer& cmdBuffer);
	private:
		vk::CommandPool cmdPool_;
		vk::Device& device;
		void init_CommandPool();
	};
}
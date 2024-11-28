#pragma once

#include "vulkan/vulkan.hpp"
#include "../buffer/buffer.h"
#include "../commandPool/commandPool.h"
#include "../descriptorPool/descriptorPool.h"
#include "../uniform/uniform.hpp"
namespace easy3d {
	class Renderer final {
	public:
		Renderer();
		~Renderer();
		void Render();
	private:
		std::unique_ptr<CommandPool> cmdPool_;
		std::unique_ptr<DescriptorPool> desPool_;
		struct FrameResources {
			vk::CommandBuffer cmdBuffer;
			vk::Semaphore imageAvailableSemaphore;
			vk::Semaphore renderFinishedSemaphore;
			vk::Fence inFlightFence;
		};
		constexpr static int MAX_FRAMES_IN_FLIGHT = 2; // Ë«»º³å
		std::array<FrameResources, MAX_FRAMES_IN_FLIGHT> frameResources_;
		size_t currentFrame_ = 0; // µ±Ç°Ö¡Ë÷Òý
		struct Buffers{
			std::unique_ptr<Buffer> host;
			std::unique_ptr<Buffer> device;
		}VertexBuffer;
		std::vector<Buffers> UniformBuffer;
		std::vector<vk::DescriptorSet> sets_;
		void createVertexBuffer();
		void bufferVertexData();
		void createUniformBuffer();
		void bufferUniformData();
		void initCommandPool();
		void initDescriptorPool();
		void updateSets();
		void initFrameResources();

		void cleanupFrameResources();

	};
}
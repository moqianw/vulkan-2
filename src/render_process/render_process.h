#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include "../shader/shader.h"
#include "../uniform/uniform.hpp"
namespace easy3d {
	class RenderProcess final {
	public:
		vk::Pipeline pipeline;
		vk::PipelineLayout layout;
		vk::RenderPass renderpass;
		vk::Device& device;
		vk::DescriptorSetLayout setlayouts;
		void Init_PipeLine(Shader& shader, vk::PipelineVertexInputStateCreateInfo inputstate);
		void Init_Layout();
		void Init_Renderpass();
		RenderProcess(SDL_Window* window, Shader& shader, vk::Device& device, vk::PipelineVertexInputStateCreateInfo inputstate);
		~RenderProcess();
	private:
		SDL_Window* window;
	};
}
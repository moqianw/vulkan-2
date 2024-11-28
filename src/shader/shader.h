#pragma once
#include "vulkan/vulkan.hpp"

namespace easy3d {
	class Shader final {
	public:
		vk::ShaderModule vertexmodule;
		vk::ShaderModule fragmentmodule;
		std::vector<vk::ShaderModule> shadermodules;
		vk::Device& device;
		std::vector<vk::PipelineShaderStageCreateInfo> get_ShaderStage();
		void init_ShaderStage();
		Shader(const std::string& vs, const std::string& fs, vk::Device& device);
		~Shader();
	private:
		std::vector<vk::PipelineShaderStageCreateInfo> stage_;
	};
}
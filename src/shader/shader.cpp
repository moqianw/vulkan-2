#include "shader.h"
namespace easy3d {
	Shader::Shader(const std::string& vs, const std::string& fs, vk::Device& device):device(device) {
		vk::ShaderModuleCreateInfo createinfo;
		createinfo.codeSize = vs.size();
		createinfo.pCode = (uint32_t*)vs.data();
		vertexmodule = device.createShaderModule(createinfo);

		createinfo.codeSize = fs.size();
		createinfo.pCode = (uint32_t*)fs.data();
		fragmentmodule = device.createShaderModule(createinfo);
		init_ShaderStage();
	}
	std::vector<vk::PipelineShaderStageCreateInfo> Shader::get_ShaderStage()
	{
		return stage_;
	}
	void Shader::init_ShaderStage()
	{
		stage_.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(vertexmodule)
			.setPName("main"));
		stage_.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(fragmentmodule)
			.setPName("main"));

	}
	Shader::~Shader()
	{
		device.destroyShaderModule(vertexmodule);
		device.destroyShaderModule(fragmentmodule);
	}

}
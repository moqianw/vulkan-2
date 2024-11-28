#include "render_process.h"
#include "../shader/shader.h"
#include "../context.h"
namespace easy3d {
	void RenderProcess::Init_PipeLine(Shader& shader, vk::PipelineVertexInputStateCreateInfo inputstate)
	{
		vk::GraphicsPipelineCreateInfo createinfo;
		//1 顶点输入
		
		
		createinfo.setPVertexInputState(&inputstate);
		//2 顶点聚集
		vk::PipelineInputAssemblyStateCreateInfo inputasm;
		inputasm.setPrimitiveRestartEnable(false)
			.setTopology(vk::PrimitiveTopology::eTriangleList);
		createinfo.setPInputAssemblyState(&inputasm);
		//3 shader
		auto stages = shader.get_ShaderStage();
		createinfo.setStages(stages);

		//4 viewport
		vk::PipelineViewportStateCreateInfo viewportstate;
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		vk::Viewport viewport(0, 0, width, height, 0, 1);
		vk::Rect2D rect({ 0,0 }, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
		viewportstate.setViewports(viewport)
			.setScissors(rect);
		createinfo.setPViewportState(&viewportstate);

		//5 光栅化
		vk::PipelineRasterizationStateCreateInfo rastinfo;
		rastinfo.setRasterizerDiscardEnable(false)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eClockwise)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1);
		createinfo.setPRasterizationState(&rastinfo);

		//6 多重采样

		vk::PipelineMultisampleStateCreateInfo multisample;
		multisample.setSampleShadingEnable(false)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);
		createinfo.setPMultisampleState(&multisample);
		//7 test

		//8 颜色混合
		vk::PipelineColorBlendStateCreateInfo colorblend;
		vk::PipelineColorBlendAttachmentState attachs;
		attachs.setBlendEnable(false)
			.setColorWriteMask(vk::ColorComponentFlagBits::eA |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eR);
		colorblend.setLogicOpEnable(false)
			.setAttachments(attachs);
		createinfo.setPColorBlendState(&colorblend);

		//9 renderpass and layout

		createinfo.setRenderPass(renderpass)
			.setLayout(layout);

		auto result = device.createGraphicsPipeline(nullptr, createinfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("create graphics pipeline failed");
		}
		pipeline = result.value;
	}
	void RenderProcess::Init_Layout()
	{
		vk::PipelineLayoutCreateInfo createinfo;

		vk::DescriptorSetLayoutCreateInfo setcreateinfo;
		std::vector<vk::DescriptorSetLayoutBinding> bindings = { 
			Uniform<glm::vec3>::getBinding(0, vk::ShaderStageFlagBits::eFragment) ,
			Uniform<glm::mat4>::getBinding(1, vk::ShaderStageFlagBits::eVertex)
		};
		setcreateinfo.setBindings(bindings);
		setlayouts = device.createDescriptorSetLayout(setcreateinfo);//set = 0 binding = 0

		createinfo.setSetLayouts(setlayouts);
		layout = Context::GetInstance().device->device.createPipelineLayout(createinfo);
	}
	void RenderProcess::Init_Renderpass()
	{
		vk::RenderPassCreateInfo createinfo;
		vk::AttachmentDescription attach;
		attach.setFormat(Context::GetInstance().swapchain->swapchaininfo.format.format)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setSamples(vk::SampleCountFlagBits::e1);

		createinfo.setAttachments(attach);

		vk::AttachmentReference reference;
		reference.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setAttachment(0);
		vk::SubpassDescription subpass;
		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachments(reference);
		createinfo.setSubpasses(subpass);

		vk::SubpassDependency dependency;
		dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		createinfo.setDependencies(dependency);

		renderpass = device.createRenderPass(createinfo);
	}
	RenderProcess::RenderProcess(SDL_Window* window, Shader& shader, vk::Device& device, vk::PipelineVertexInputStateCreateInfo inputstate):window(window),device(device)
	{
		Init_Renderpass();
		Init_Layout();
		Init_PipeLine(shader,inputstate);
	}
	RenderProcess::~RenderProcess()
	{
		device.destroyRenderPass(renderpass);
		device.destroyPipelineLayout(layout);
		device.destroyPipeline(pipeline);
	}
}
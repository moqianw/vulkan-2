#include "context.h"
namespace easy3d {
	std::unique_ptr<Context> Context::instance_ = nullptr;
	void Context::Init(CreateSurfaceFunc func) {
		instance_.reset(new Context(func));
	}
	void Context::createInstance() {
		instance.reset(new Instance);
	}
	void Context::destroyInstance()
	{
		instance.reset();
	}

	void Context::pickupPhysicaldevice() {
		device.reset(new Device);
		device->pickupPhysicalDevice(instance->instance);
	}
	void Context::queryQueueFamilyIndex() {
		queues.reset(new Queue);
		queues->queryQueueFamilyIndex(device->physicaldevice, surface);
	}
	void Context::createDevice() {
		device->createDevice(*queues);
	}

	void Context::getQueues() {
		queues->getQueues(device->device);
	}

	Context& Context::GetInstance() {
		return *instance_;
	}
	void Context::Quit() {
		instance_.reset();
	}
	Context::Context(CreateSurfaceFunc func) {
		createInstance();
		pickupPhysicaldevice();
		surface = func(instance->instance);
		queryQueueFamilyIndex();
		createDevice();
		getQueues();
	}
	void Context::InitShader(const std::string& vs, const std::string& fs, vk::Device& device)
	{
		shader.reset(new Shader(vs, fs, device));
	}
	void Context::DestroyShader()
	{
		shader.reset();
	}
	void Context::InitSwapchain(SDL_Window* window) {
		swapchain.reset(new Swapchain(window));
	}
	void Context::InitRenderprocess(SDL_Window* window)
	{
		vk::PipelineVertexInputStateCreateInfo inputstate;
		vk::VertexInputAttributeDescription attr;
		attr.setBinding(0)
			.setFormat(vk::Format::eR32G32Sfloat)
			.setLocation(0)
			.setOffset(0);
		vk::VertexInputBindingDescription binding;
		binding.setBinding(0)
			.setInputRate(vk::VertexInputRate::eVertex)
			.setStride(sizeof(glm::vec2));
		inputstate.setVertexAttributeDescriptions(attr)
			.setVertexBindingDescriptions(binding);
		renderprocess.reset(new RenderProcess(window,*shader,device->device, inputstate));
	}
	void Context::DestroyRenderprocess()
	{
		renderprocess.reset();
	}
	void Context::DestorySwapchain()
	{
		swapchain.reset();
	}
	void Context::InitRenderer()
	{
		renderer.reset(new Renderer);
	}
	void Context::DestroyRender()
	{
		renderer.reset();
	}
	Context::~Context() {
		instance->instance.destroySurfaceKHR(surface);
		device.reset();
		destroyInstance();
	}
}

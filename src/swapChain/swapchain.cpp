#include "swapchain.h"
#include "../context.h"
namespace easy3d {
	Swapchain::Swapchain(SDL_Window* window):window(window)
	{
		querySwapchainInfo();
		vk::SwapchainCreateInfoKHR createinfo;
		createinfo.setClipped(true)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setSurface(Context::GetInstance().surface)
			.setImageColorSpace(swapchaininfo.format.colorSpace)
			.setImageFormat(swapchaininfo.format.format)
			.setImageExtent(swapchaininfo.imageExtent)
			.setMinImageCount(swapchaininfo.imageCount)
			.setPresentMode(swapchaininfo.present);

		auto& queueindixes = Context::GetInstance().queues->queuefamilyindex;
		if (queueindixes.graphicsqueue.value()==queueindixes.presentqueue.value()) {
			createinfo.setQueueFamilyIndices(queueindixes.graphicsqueue.value())
				.setImageSharingMode(vk::SharingMode::eExclusive);

		}
		else {
			std::array<uint32_t,2> indices = { queueindixes.graphicsqueue.value(),queueindixes.presentqueue.has_value() };
			createinfo.setQueueFamilyIndices(indices)
				.setImageSharingMode(vk::SharingMode::eConcurrent);
		}
		swapchain = Context::GetInstance().device->device.createSwapchainKHR(createinfo);
		get_Images();
		create_ImageViews();

	}
	Swapchain::~Swapchain()
	{
		auto& device = Context::GetInstance().device->device;
		for (auto& framebuffer : framebuffers) {
			device.destroyFramebuffer(framebuffer);
		}
		for (auto& view : imageViews) {
			device.destroyImageView(view);
		}
		device.destroySwapchainKHR(swapchain);

	}
	void Swapchain::querySwapchainInfo()
	{
		auto& physicaldevice = Context::GetInstance().device->physicaldevice;
		auto& surface = Context::GetInstance().surface;
		auto formats = physicaldevice.getSurfaceFormatsKHR(surface);
		swapchaininfo.format = formats[0];
		for (const auto& format : formats) {
			if (format.format == vk::Format::eR8G8B8A8Srgb &&
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				swapchaininfo.format = format;
				break;
			}
		}
		auto capabilities = physicaldevice.getSurfaceCapabilitiesKHR(surface);
		//Ë«»º³å
		swapchaininfo.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		swapchaininfo.imageExtent.width = std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		swapchaininfo.imageExtent.height = std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		swapchaininfo.transform = capabilities.currentTransform;
		auto presents = physicaldevice.getSurfacePresentModesKHR(surface);
		swapchaininfo.present = vk::PresentModeKHR::eFifo;
		for (const auto& present : presents) {
			if (present == vk::PresentModeKHR::eMailbox) {
				swapchaininfo.present = present;
				break;
			}
		}

	}
	void Swapchain::get_Images()
	{
		images = Context::GetInstance().device->device.getSwapchainImagesKHR(swapchain);
	}
	void Swapchain::create_ImageViews()
	{
		imageViews.resize(images.size());
		for (int i = 0; i < imageViews.size(); i++) {
			vk::ImageViewCreateInfo createinfo;
			vk::ComponentMapping mapping;
			vk::ImageSubresourceRange range;
			range.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
				.setAspectMask(vk::ImageAspectFlagBits::eColor);
			createinfo.setImage(images[i])
				.setViewType(vk::ImageViewType::e2D)
				.setComponents(mapping)
				.setFormat(swapchaininfo.format.format)
				.setSubresourceRange(range);
			imageViews[i] = Context::GetInstance().device->device.createImageView(createinfo);
		}
	}
	void Swapchain::create_Framebuffers()
	{
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		framebuffers.resize(images.size());
		for (int i = 0; i < framebuffers.size(); i++) {
			vk::FramebufferCreateInfo createinfo;
			createinfo.setAttachments(imageViews[i])
				.setWidth(width)
				.setHeight(height)
				.setRenderPass(Context::GetInstance().renderprocess->renderpass)
				.setLayers(1);
			framebuffers[i] = Context::GetInstance().device->device.createFramebuffer(createinfo);
		}
	}
}
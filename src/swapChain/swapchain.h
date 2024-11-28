#pragma once

#include "vulkan/vulkan.hpp"
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>


namespace easy3d {
	class Swapchain {
	public:
		vk::SwapchainKHR swapchain;
		Swapchain(SDL_Window* window);
		~Swapchain();
		struct SwapchainInfo {
			vk::Extent2D imageExtent;//ͼ���С
			uint32_t imageCount;//����
			vk::SurfaceFormatKHR format;//��ʽ
			vk::SurfaceTransformFlagsKHR transform;
			vk::PresentModeKHR present;
		};
		SwapchainInfo swapchaininfo;
		void querySwapchainInfo();

		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
		std::vector<vk::Framebuffer> framebuffers;
		void get_Images();
		void create_ImageViews();
		void create_Framebuffers();
	private:
		SDL_Window* window;
	};
}
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "vulkan/vulkan.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include "tool.h"
#include "swapChain/swapchain.h"
#include "render_process/render_process.h"
#include "renderer/renderer.h"
#include "window/sdlwindow.h"
#include "instance/instance.h"
#include "queue/queue.h"
#include "device/device.h"
#include "shader/shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
namespace easy3d {
	class Context final {
	private:
		Context(CreateSurfaceFunc func);
		static std::unique_ptr<Context> instance_;

		void pickupPhysicaldevice();
		void createDevice();
		void queryQueueFamilyIndex();
		void getQueues();
	public:
		static Context& GetInstance();
		static void Quit();
		static void Init(CreateSurfaceFunc func);
		void InitSwapchain(SDL_Window* window);
		void InitRenderprocess(SDL_Window* window);
		void DestroyRenderprocess();
		void DestorySwapchain();

		void InitRenderer();
		void DestroyRender();
		~Context();
		std::unique_ptr<Instance> instance;
		void createInstance();
		void destroyInstance();

		std::unique_ptr<Queue> queues;
		std::unique_ptr<Device> device;
		vk::SurfaceKHR surface;
		std::unique_ptr<Shader> shader;
		void InitShader(const std::string& vs, const std::string& fs, vk::Device& device);
		void DestroyShader();
		std::unique_ptr<Swapchain> swapchain;
		std::unique_ptr<RenderProcess> renderprocess;
		std::unique_ptr<Renderer> renderer;

	};
}

#include "easy3d.h"
namespace easy3d {
	void Init(std::vector<const char*>& extensions, CreateSurfaceFunc func, SDL_Window* window){

		Context::Init(func);
		auto& context = Context::GetInstance();
		context.InitShader(read_ShaderFile("./shader/shader.vert.spv"), read_ShaderFile("./shader/shader.frag.spv"), context.device->device);
		context.InitSwapchain(window);
		context.InitRenderprocess(window);
		context.swapchain->create_Framebuffers();
		context.InitRenderer();
	}

	void Quit(){
		auto& context = Context::GetInstance();
		context.device->device.waitIdle();
		context.DestroyRender();
		context.DestroyRenderprocess();
		context.DestroyShader();
		context.DestorySwapchain();
		Context::Quit();
	}

	Renderer& GetRenderer()
	{
		return *Context::GetInstance().renderer;
	}

}

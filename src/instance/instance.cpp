#include "instance.h"
#include "../window/sdlwindow.h"
namespace easy3d {

	Instance::Instance()
	{
		create_vkInstance();
	}

	Instance::~Instance()
	{
		instance.destroy();
	}

	void Instance::create_vkInstance()
	{
		//auto extensions = vk::enumerateInstanceExtensionProperties();
//for (auto& extension : extensions) {
//	std::cout << extension << std::endl;
//}
/*
	VK_KHR_device_group_creation
	VK_KHR_display
	VK_KHR_external_fence_capabilities
	VK_KHR_external_memory_capabilities
	VK_KHR_external_semaphore_capabilities
	VK_KHR_get_display_properties2
	VK_KHR_get_physical_device_properties2
	VK_KHR_get_surface_capabilities2
	VK_KHR_surface
	VK_KHR_surface_protected_capabilities
	VK_KHR_win32_surface
	VK_EXT_debug_report
	VK_EXT_debug_utils
	VK_EXT_direct_mode_display
	VK_EXT_surface_maintenance1
	VK_EXT_swapchain_colorspace
	VK_NV_external_memory_capabilities
	VK_KHR_portability_enumeration
	VK_LUNARG_direct_driver_loading


*/
//auto layer = vk::enumerateInstanceLayerProperties();
//for (auto& lay : layer) {
//	std::cout << lay.layerName << std::endl;
//}
/*
	VK_LAYER_NV_optimus
	VK_LAYER_VALVE_steam_overlay
	VK_LAYER_VALVE_steam_fossilize
	VK_LAYER_EOS_Overlay
	VK_LAYER_EOS_Overlay
	VK_LAYER_LUNARG_api_dump
	VK_LAYER_LUNARG_gfxreconstruct
	VK_LAYER_KHRONOS_synchronization2
	VK_LAYER_KHRONOS_validation
	VK_LAYER_LUNARG_monitor
	VK_LAYER_LUNARG_screenshot
	VK_LAYER_KHRONOS_profiles
	VK_LAYER_KHRONOS_shader_object
*/
		try {
			std::vector<const char*> extensions = Window::GetInstance().Get_SDL_Vulkan_GetInstanceExtensions();
			vk::InstanceCreateInfo createinfo;
			vk::ApplicationInfo appinfo;


			std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };//—È÷§≤„
			appinfo.setApiVersion(VK_API_VERSION_1_3);
			createinfo.setPApplicationInfo(&appinfo)
				.setPpEnabledLayerNames(layers.data())
				.setPEnabledExtensionNames(extensions);
			instance = vk::createInstance(createinfo);
			std::cout << "vk instance created" << std::endl;
		}
		catch (const vk::SystemError& e) {
			throw std::runtime_error(std::string("Failed to create Vulkan instance: ") + e.what());
		}
		catch (const std::exception& e) {
			throw std::runtime_error(std::string("Unknown error: ") + e.what());
		}

	}

}
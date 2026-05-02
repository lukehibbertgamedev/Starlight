#ifndef __GPU_H
#define __GPU_H

#include "Errors.h"
#include "Logger.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <string>

#define PACK_VERSION(major, minor, issue) VK_MAKE_API_VERSION(0, major, minor, issue)


namespace Starlight
{


	struct GpuInfo
	{
		std::string appName;
		uint32_t appVersion;


	};

	class Gpu
	{
	public:

		StarlightErr Init(const GpuInfo* pGpuInfo);

		StarlightErr Terminate();

	private:

		VkInstance instance;		
		VkPhysicalDevice physicalDevice;
		VkDevice device;		

		VkDebugUtilsMessengerEXT debugMessenger;

		VmaAllocator allocator;
	};
}

#endif // ! __GPU_H
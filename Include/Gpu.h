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

		bool enableValidationLayers;
	};

	class Gpu
	{
	public:

		Gpu();

		StarlightErr Init(GpuInfo* pGpuInfo);

		StarlightErr Terminate();

	private:

		VkResult CheckVkResult(const VkResult result, const char* msg);

		VkResult LoadVulkanFunctions();

		VkResult CreateInstance(const GpuInfo* pGpuInfo);
		void DestroyInstance();

		VkResult SelectPhysicalDevice();
		bool CheckPhysicalDeviceSuitability();
		void DestroyPhysicalDevice();

		VkResult CreateLogicalDevice();
		void DestroyLogicalDevice();

		VkResult CreateVmaAllocator();
		void DestroyVmaAllocator();

		VkResult CreateDebugMessenger();
		void DestroyDebugMessenger();

		VkResult SetDebugObjectName(const uint64_t handle, const VkObjectType type, const std::string& name) const;

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	private:



		VkInstance instance;		
		VkPhysicalDevice physicalDevice;
		VkDevice device;		

		VkDebugUtilsMessengerEXT debugMessenger;

		VmaAllocator allocator;
		VkAllocationCallbacks* allocatorCallback;

		GpuInfo* pGpuInfo;


		static PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT;
		static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
		static PFN_vkSetDebugUtilsObjectNameEXT    vkSetDebugUtilsObjectNameEXT;
	};
}

#endif // ! __GPU_H
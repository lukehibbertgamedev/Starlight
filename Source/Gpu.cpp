#include "../Include/Gpu.h"

#if WIN32
#include <vulkan/vk_enum_string_helper.h>
#endif

#include <unordered_map>
#include <vector>

#include "../Include/Window.h"
#include "../Include/Profiler.h"

PFN_vkCreateDebugUtilsMessengerEXT  Starlight::Gpu::vkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT Starlight::Gpu::vkDestroyDebugUtilsMessengerEXT = nullptr;
PFN_vkSetDebugUtilsObjectNameEXT    Starlight::Gpu::vkSetDebugUtilsObjectNameEXT = nullptr;

#define LOAD_VULKAN_FUNCTION(x) { \
    auto fn = (PFN_##x)vkGetInstanceProcAddr(instance, #x);\
    if(fn != nullptr) Starlight::Gpu::##x = fn; \
    else CheckVkResult(VK_ERROR_EXTENSION_NOT_PRESENT, "[Vulkan]\tUnable to load Function " #x " !\n"); \
}\

namespace Starlight
{
    Gpu::Gpu()
    {
        StarlightZoneScoped;

        instance = VK_NULL_HANDLE;
		device = VK_NULL_HANDLE;
		physicalDevice = VK_NULL_HANDLE;
		debugMessenger = VK_NULL_HANDLE;
		allocator = VK_NULL_HANDLE;

        allocatorCallback = nullptr;
		pGpuInfo = nullptr;
    }

    StarlightErr Starlight::Gpu::Init(GpuInfo* pGpuInfo)
    {
        StarlightZoneScoped;
		Starlight::Logger::Log(Starlight::eLogLevel::Info, "Gpu::Init\n");

        if (!pGpuInfo)
        {
			Starlight::Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "GpuInfo pointer is null\n");
			return StarlightErr::Cannot_Init;
        }

        this->pGpuInfo = pGpuInfo;

		StarlightErr err = StarlightErr::Success;

        // Vulkan instance.
        err = (StarlightErr)CheckVkResult(CreateInstance(pGpuInfo), "CreateInstance");
		Logger::Log(Starlight::eLogLevel::Info, "Created Vulkan instance\n");

        // Load functions.
        err = (StarlightErr)CheckVkResult(LoadVulkanFunctions(), "LoadVulkanFunctions");
        Logger::Log(Starlight::eLogLevel::Info, "Loaded Vulkan functions\n");

        // Debug messenger.
        if (pGpuInfo->enableValidationLayers)
        {
			err = (StarlightErr)CheckVkResult(CreateDebugMessenger(), "CreateDebugMessenger");
			Logger::Log(Starlight::eLogLevel::Info, "Created debug messenger\n");
        }

		err = (StarlightErr)CheckVkResult(SelectPhysicalDevice(), "SelectPhysicalDevice");
		Logger::Log(Starlight::eLogLevel::Info, "Selected physical device\n");

        err = (StarlightErr)CheckVkResult(CreateLogicalDevice(), "CreateLogicalDevice");
        Logger::Log(Starlight::eLogLevel::Info, "Created logical device\n");

        return StarlightErr::Success;
    }

    StarlightErr Starlight::Gpu::Terminate()
    {
        StarlightZoneScoped;

		Logger::Log(Starlight::eLogLevel::Info, "Gpu::Terminate\n");

        if (pGpuInfo->enableValidationLayers)
        {
            DestroyDebugMessenger();
        }

        DestroyInstance();

		return StarlightErr::Success;
    }

    VkResult Gpu::CheckVkResult(const VkResult result, const char* msg)
    {
        StarlightZoneScoped;

        if (result > VK_SUCCESS)
        {
		    Logger::Log(Starlight::eLogLevel::Warning, string_VkResult(result), msg);
        }

		else if (result < VK_SUCCESS)
        {
			Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Vulkan error: %s, in %s\n", string_VkResult(result), msg);
        }

        return result;
    }

    VkResult Gpu::LoadVulkanFunctions()
    {
        StarlightZoneScoped;

        if (!instance)
        {
			Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Cannot load Vulkan functions, instance is null\n");
            return VK_ERROR_DEVICE_LOST;
        }

        if (!pGpuInfo->enableValidationLayers)
        {
			return VK_SUCCESS;
        }

		LOAD_VULKAN_FUNCTION(vkCreateDebugUtilsMessengerEXT);
		LOAD_VULKAN_FUNCTION(vkDestroyDebugUtilsMessengerEXT);
		LOAD_VULKAN_FUNCTION(vkSetDebugUtilsObjectNameEXT);

        if (!vkCreateDebugUtilsMessengerEXT || !vkDestroyDebugUtilsMessengerEXT || !vkSetDebugUtilsObjectNameEXT)
        {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

		return VK_SUCCESS;
    }

    VkResult Gpu::CreateInstance(const GpuInfo* pGpuInfo)
    {
        StarlightZoneScoped;

		std::vector<const char*> instanceLayers;
		std::vector<const char*> instanceExtensions;

        VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = pGpuInfo->appName.c_str();
		appInfo.applicationVersion = pGpuInfo->appVersion;
		appInfo.pEngineName = "Starlight Engine";
		appInfo.engineVersion = pGpuInfo->appVersion;
		appInfo.apiVersion = VK_API_VERSION_1_4;

        if (pGpuInfo->enableValidationLayers)
        {
            instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
            instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

#if WIN32

        // GLFW required extensions
        uint32_t glfwExtensionCount = 0u;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (size_t i = 0; i < glfwExtensionCount; ++i)
        {
            instanceExtensions.push_back(glfwExtensions[i]);
        }

#endif

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
		createInfo.ppEnabledLayerNames = instanceLayers.data();

        if (pGpuInfo->enableValidationLayers)
        {
			Logger::Log(Starlight::eLogLevel::Info, "Enabling validation layers\n");

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugCreateInfo.pUserData = this;
			debugCreateInfo.pfnUserCallback = DebugCallback;

			createInfo.pNext = &debugCreateInfo;
        }


		return CheckVkResult(vkCreateInstance(&createInfo, allocatorCallback, &instance), "vkCreateInstance"); 
    }

    void Gpu::DestroyInstance()
    {
        StarlightZoneScoped;

        if (!instance || !allocatorCallback)
        {
            Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Cannot destroy instance, instance or allocator callback is null\n");
        }

		vkDestroyInstance(instance, allocatorCallback);

        instance = nullptr;
    }

    VkResult Gpu::SelectPhysicalDevice()
    {
        StarlightZoneScoped;

		uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount <= 0)
        {
            Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "No physical devices found\n");
			return VK_ERROR_FEATURE_NOT_PRESENT;
        }

		std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		VkPhysicalDeviceFeatures requiredFeatures = {}; 
        

        //Prefer Discrete GPUs -> Integrated GPUs -> CPUs -> Software Driver
        //Require Vulkan 1.0 support. 
        VkPhysicalDevice deviceCandidate = VK_NULL_HANDLE;
        VkPhysicalDeviceType candidateType = VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM;
        {
            for (const VkPhysicalDevice device : devices)
            {
                //Query the Physical Device Properties for the relevant information. 
                VkPhysicalDeviceProperties deviceProperties = {};
                vkGetPhysicalDeviceProperties(device, &deviceProperties);

                //Skip any devices that don't support Vulkan 1.2
                if (deviceProperties.apiVersion < VK_API_VERSION_1_2) {
                    Logger::Log(eLogLevel::Info, "Device [%s](%s) Skipped: Vulkan API Version (%d.%d.%d) was unsupported!\n", deviceProperties.deviceName, string_VkPhysicalDeviceType(deviceProperties.deviceType), VK_API_VERSION_MAJOR(deviceProperties.apiVersion), VK_API_VERSION_MINOR(deviceProperties.apiVersion), VK_API_VERSION_PATCH(deviceProperties.apiVersion));
                    continue;
                }

                {
                    //Evaluate device feature compatibility
                    VkPhysicalDeviceFeatures deviceFeatures = {};
                    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

                    bool featuresValid = true;

                    for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++) {
                        VkBool32* a = ((VkBool32*)&requiredFeatures) + i;
                        VkBool32* b = ((VkBool32*)&deviceFeatures) + i;

                        if (*a == VK_TRUE) {
                            if (*b != VK_TRUE) {
                                featuresValid = false;
                            }
                        }
                    }

                    if (!featuresValid) {
                        Logger::Log(eLogLevel::Warning, "Not all required physical device features were available!\n");
                        continue;
                    }
                }

                //Cache the physical device candidate
                {
                    //Convenience Lambda
                    auto selectCandidate = [&]() {
                        Logger::Log(eLogLevel::Info, "Device [%s](%s) is the current best candidate!\nVulkan API Version (%d.%d.%d)\tDriver Version (%d.%d.%d)\n", deviceProperties.deviceName, string_VkPhysicalDeviceType(deviceProperties.deviceType), VK_API_VERSION_MAJOR(deviceProperties.apiVersion), VK_API_VERSION_MINOR(deviceProperties.apiVersion), VK_API_VERSION_PATCH(deviceProperties.apiVersion), VK_API_VERSION_MAJOR(deviceProperties.driverVersion), VK_API_VERSION_MINOR(deviceProperties.driverVersion), VK_API_VERSION_PATCH(deviceProperties.driverVersion));

                        deviceCandidate = device;
                        candidateType = deviceProperties.deviceType;
                        };

                    //Only evaluate if there IS a candidate already present
                    if (deviceCandidate != VK_NULL_HANDLE)
                    {
                        if (candidateType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                        {
                            selectCandidate();
                        }
                        else
                        {
                            selectCandidate();
                        }
                    }
                    else
                    {
                        selectCandidate(); //This is the first candidate, so just select it anyway!
                    }
                }
            }

            //Complain if there were no supported devices. 
            if (deviceCandidate == VK_NULL_HANDLE) {
                Logger::Log(eLogLevel::Warning, "No physical device candidates were valid!\n");
            }

			physicalDevice = deviceCandidate;
        }	

        if (!physicalDevice)
        {
			Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Failed to find a suitable GPU!\n");
        }

		return VK_SUCCESS;

    }
    void Gpu::DestroyPhysicalDevice()
    {
        StarlightZoneScoped;

    }

    VkResult Gpu::CreateLogicalDevice()
    {
        StarlightZoneScoped;

		VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = {};
		descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
		descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
		descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;

		std::vector<const char*> deviceExtensions = 
        { 
            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, 
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, 
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, 
            VK_KHR_SPIRV_1_4_EXTENSION_NAME, 
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
            VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        };

        // TODO: Expose 1 graphics queue for now.

        auto FindGraphicsQueueFamilyIndex = [](VkPhysicalDevice physicalDevice) -> uint32_t
            {
                uint32_t propCount = 0u;
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propCount, nullptr);

                std::vector<VkQueueFamilyProperties> families(propCount);
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propCount, families.data());

                int i = 0;
                for (const auto& queueFamily : families)
                {
                    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
                    {
                        return i;
                    }

                    ++i;
                }
            };

        float queuePriorities = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = 1u;
        queueCreateInfo.queueFamilyIndex = FindGraphicsQueueFamilyIndex(physicalDevice);
        queueCreateInfo.pQueuePriorities = &queuePriorities;

        // Backwards feature pNext chain.

        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
        accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        accelerationStructureFeatures.accelerationStructure = VK_TRUE;        
        
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR raytracingPipelineFeatures = {};
        raytracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        raytracingPipelineFeatures.pNext = &accelerationStructureFeatures;
        raytracingPipelineFeatures.rayTracingPipeline = VK_TRUE;

        VkPhysicalDeviceVulkan12Features features12 = {};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features12.pNext = &raytracingPipelineFeatures;
        features12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        features12.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
        features12.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
        features12.bufferDeviceAddress = VK_TRUE;
        features12.descriptorIndexing = VK_TRUE;
        features12.bufferDeviceAddress = VK_TRUE;

        VkPhysicalDeviceFeatures2 deviceFeatures = {};
        deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures.pNext = &features12;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;        
        createInfo.pNext = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		return CheckVkResult(vkCreateDevice(physicalDevice, &createInfo, allocatorCallback, &device), "vkCreateDevice");
    }

    void Gpu::DestroyLogicalDevice()
    {
        StarlightZoneScoped;

    }

    VkResult Gpu::CreateVmaAllocator()
    {
        StarlightZoneScoped;

        return VkResult();
    }

    void Gpu::DestroyVmaAllocator()
    {
        StarlightZoneScoped;

    }

    VkResult Gpu::CreateDebugMessenger()
    {
        StarlightZoneScoped;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pUserData = this;
        debugCreateInfo.pfnUserCallback = DebugCallback;

		return CheckVkResult(Gpu::vkCreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, allocatorCallback, &debugMessenger), "::vkCreateDebugUtilsMessengerEXT");
    }

    void Gpu::DestroyDebugMessenger()
    {
        StarlightZoneScoped;

		if (!instance || !debugMessenger)
        {
			Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Cannot destroy debug messenger, instance or messenger is null\n");
        }

		Gpu::vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, allocatorCallback);
		
        debugMessenger = nullptr;
    }

    VkResult Gpu::SetDebugObjectName(const uint64_t handle, const VkObjectType type, const std::string& name) const
    {
        StarlightZoneScoped;

        return VkResult();
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Gpu::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        StarlightZoneScoped;

        Logger::Log(Starlight::eLogLevel::Info, "Debug callback: %s\n", pCallbackData->pMessage);
        return VK_FALSE;
    }

}
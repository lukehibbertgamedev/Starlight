#include <cstdio>

#include "../Include/Logger.h"
#include "../Include/Utility.h"
#include "../Include/Gpu.h"
#include "../Include/Profiler.h"
#include "../Include/Window.h"

#include <vulkan/vulkan.h>

#define PATH_TO_ASSETS "../../../Assets/"

int main(int argc, const char** argv)
{
	Starlight::Logger::Log(Starlight::eLogLevel::Info, "Starlight...\n");

	Starlight::GpuInfo gpuInfo = {};
	gpuInfo.appName = "Starlight";
	gpuInfo.appVersion = PACK_VERSION(0, 1, 0);
	gpuInfo.enableValidationLayers = true;

	Starlight::Gpu gpu;

	Starlight::CheckError(gpu.Init(&gpuInfo));

	glfwInit();	

	Starlight::Window window;
	window.Create(1280, 720, "Starlight Engine");
	window.SetIcon(PATH_TO_ASSETS + std::string("Icon/Icon.png"));

	// render probably.

	uint64_t frameCount = 0;

	while (window.PollEvents())
	{
		{
			StarlightZoneScoped;
			Starlight::Logger::Log(Starlight::eLogLevel::Info, "Frame %d \r", ++frameCount);
		}
		Starlight::Profiler::EndFrame;
	}

	//Starlight::CheckError(gpu.Terminate());

	glfwTerminate();

	return 0;
}
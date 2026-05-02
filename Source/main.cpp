#include <cstdio>

#include "../Include/Logger.h"
#include "../Include/Utility.h"
#include "../Include/Gpu.h"

#include <vulkan/vulkan.h>

int main(int argc, const char** argv)
{
	Starlight::Logger::Log(Starlight::eLogLevel::Info, "Starlight...\n");

	Starlight::GpuInfo gpuInfo = {};
	gpuInfo.appName = "Starlight";
	gpuInfo.appVersion = PACK_VERSION(0, 1, 0);

	Starlight::Gpu gpu;

	Starlight::CheckError(gpu.Init(&gpuInfo));

	return 0;
}
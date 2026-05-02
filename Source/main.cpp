#include <cstdio>

#include "../Include/Logger.h"

#include <vulkan/vulkan.h>

int main(int argc, const char** argv)
{
	VkInstanceCreateInfo createInfo = {};

	Starlight::Logger::Log(Starlight::eLogLevel::Info, "Starlight is not a problem\n");
	Starlight::Logger::Log(Starlight::eLogLevel::Warning, "Starlight is maybe a problem\n");

	Starlight::Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Starlight is a problem\n");

	return 0;
}
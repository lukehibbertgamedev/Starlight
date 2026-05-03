#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <tracy/Tracy.hpp>
//#include <tracy/TracyVulkan.hpp>

#include "Logger.h"

#define VulkanZoneScoped ZoneScopedC(0xFFFFFF)
#define StarlightZoneScoped ZoneScopedC(0x00a4b4)
#define SystemZoneScoped ZoneScopedC(0xff6219)
#define ResourceZoneScoped ZoneScopedC(0x7affc2)

namespace Starlight
{
	class Profiler
	{
	public:

		static StarlightErr Init();
		static StarlightErr Terminate();

		static void EndFrame();

		static const bool IsEnabled();
		static const bool IsConnected();
	};
}

#endif 
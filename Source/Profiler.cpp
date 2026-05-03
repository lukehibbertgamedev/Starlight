#include "Profiler.h"
#include "Profiler.h"
#include "Profiler.h"
#include "Profiler.h"
#include "../Include/Profiler.h"

namespace Starlight
{
	StarlightErr Starlight::Profiler::Init()
	{
		StarlightZoneScoped;


		return StarlightErr::Success;
	}

	StarlightErr Starlight::Profiler::Terminate()
	{
		StarlightZoneScoped;


		return StarlightErr::Success;
	}

	void Profiler::EndFrame()
	{
		StarlightZoneScoped;
		FrameMark;
	}

	const bool Starlight::Profiler::IsEnabled()
	{
#ifdef TRACY_ENABLE
		return true;
#else
		return false;
#endif
	}

	const bool Starlight::Profiler::IsConnected()
	{
		StarlightZoneScoped;

		return false;
	}
}
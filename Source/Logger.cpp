#include "../Include/Logger.h"

#include "../Include/Errors.h"

#include <map>
#include <assert.h>

#if WIN32
#include <windows.h>
#include "../Include/Profiler.h"

const std::map<Starlight::eLogLevel, int32_t> ColourMapping =
{
	{Starlight::eLogLevel::Info, 0xf},
	{Starlight::eLogLevel::Warning, 0xe},
	{Starlight::eLogLevel::Error, 0x4},
};

// #else for other platforms.

#endif

namespace Starlight
{
	void Logger::Log(const eLogLevel severity, const char* format, ...)
	{
		StarlightZoneScoped;

		SetConsoleColour(severity);

		va_list args;
		va_start(args, format);
		Output(format, args);
		va_end(args);

		// Back to normal
		SetConsoleColour(eLogLevel::Info);
	}

	void Logger::Error(const char* file, uint16_t line, const char* function, const char* format, ...)
	{
		StarlightZoneScoped;

		SetConsoleColour(eLogLevel::Error);

		Output("[Error]\t");

		va_list args;
		va_start(args, format);

		Output(format, args);

		va_end(args);

		Output("File: %s, Line: %d, Function: %s\n", file, line, function);

		// Back to normal
		SetConsoleColour(eLogLevel::Info);

		assert(false && format);
	}

	void Logger::SetConsoleColour(const eLogLevel severity)
	{
		StarlightZoneScoped;

		if (severity >= eLogLevel::MAX)
		{
			Starlight::Unreachable();

			return;
		}
		
#if WIN32
		// Console reference
		const HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		const WORD textColor = ColourMapping.at(severity);

		SetConsoleTextAttribute(console, textColor);

//#else // -- other platforms.

#endif
	}

	void Logger::Output(const char* format, va_list args)
	{
		StarlightZoneScoped;

		if (format == nullptr)
		{
			return;
		}

		assert(strlen(format) > 0 && "Format string is empty\n");

		vprintf(format, args);
	}

	void Logger::Output(const char* format, ...)
	{
		StarlightZoneScoped;

		va_list args;
		va_start(args, format);
		Output(format, args);
		va_end(args);
	}
}
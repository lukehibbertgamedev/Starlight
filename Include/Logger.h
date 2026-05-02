#ifndef __LOGGER_H
#define __LOGGER_H

#include "Errors.h"

#include <cstdarg>
#include <cstdio>
#include <cstdint>

namespace Starlight
{
	enum class eLogLevel : uint8_t
	{
		Info,
		Warning,
		Error,
		MAX
	};

	class Logger
	{
	public:
		static void Log(const eLogLevel severity, const char* format, ...);
		static void Error(const char* file, uint16_t line, const char* function, const char* format, ...);

	private: 
		static void SetConsoleColour(const eLogLevel severity);
		static void Output(const char* format, va_list args);
		static void Output(const char* format, ...);
	};
}

#endif // ! __LOGGER_H
#ifndef __ERRORS_H
#define __ERRORS_H

#include <type_traits>
#include <stdexcept>
#include <cstdint>
#include <assert.h>
#include <unordered_map>

namespace Starlight
{
	enum class StarlightErr : int32_t
	{
		Success = 0,

		Cannot_Init = -1,
		Not_Implemented = -2,

		Max = INT32_MAX
	};

	static const std::unordered_map<StarlightErr, const char*> ErrorMapping =
	{
		{StarlightErr::Success, "Success"},
		{StarlightErr::Cannot_Init, "Could not initialise"},
		{StarlightErr::Not_Implemented, "Not yet implemented"},
	};

	static std::string GetErrorString(const StarlightErr err)
	{
		if (err > StarlightErr::Max)
		{
			return { "Undefined" };
		}

		return ErrorMapping.at(err);
	}

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

	[[noreturn]] inline void Unreachable()
	{
#if defined(_MSC_VER) && !defined(__clang__)
		__assume(false);
#else
		__builtin_unreachable();
#endif

		if (std::_Is_constant_evaluated())
		{
			assert(false && "Reached unreachable code\n");
		}
	}
}

#endif // ! __ERRORS_H

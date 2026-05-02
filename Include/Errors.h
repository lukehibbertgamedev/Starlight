#ifndef __ERRORS_H
#define __ERRORS_H

#include <type_traits>
#include <stdexcept>
#include <cstdint>
#include <assert.h>

namespace Starlight
{
	typedef uint32_t StarlightErr;

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

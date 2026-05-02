#ifndef __UTILITY_H
#define __UTILITY_H

#include "Errors.h"
#include "Logger.h"

namespace Starlight
{
	static StarlightErr CheckError(const StarlightErr err)
	{
		if (err < StarlightErr::Success)
		{
			Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Error: %s\n", ErrorMapping.at(err));
		}

		else if (err > StarlightErr::Success)
		{
			Logger::Log(eLogLevel::Warning, "Warning: %s\n", ErrorMapping.at(err));
		}

		return err;
	}
}


#endif // !__UTILITY_H
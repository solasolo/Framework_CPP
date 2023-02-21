#pragma once

#ifndef _SF_OS_H_
#define _SF_OS_H_

#include <string>

#include "CommonDef.hpp"
#include "String.hpp"

namespace SoulFab::System
{
	class SHARED_EXPORT OS
	{
	public:
		static SoulFab::Base::String GetApplicationRootPath();

		static std::string OSPath(const SoulFab::Base::String path);
	};
}

#endif
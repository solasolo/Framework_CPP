#pragma once

#ifndef _SF_BASE_CONFIG_H_
#define _SF_BASE_CONFIG_H_

#include <string>
#include <memory>

#include "String.hpp"

namespace SoulFab::Config
{
	class IConfig
	{
	public:
		virtual SoulFab::Base::String get(const std::string key) const = 0;
		virtual std::shared_ptr<IConfig> getConfig(const std::string key) const = 0 ;

		virtual ~IConfig() {}
	};

	class BaseConfig: public IConfig
	{
	public:

	};
}

#endif
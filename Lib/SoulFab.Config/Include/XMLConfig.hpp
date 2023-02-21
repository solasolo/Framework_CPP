#pragma once

#ifndef _SF_XML_CONFIG_H_
#define _SF_XML_CONFIG_H_

#include "BaseConfig.hpp"

#include "SimpleXML.hpp"
#include "CommonDef.hpp"

namespace SoulFab::Config
{
	class SHARED_EXPORT XmlConfig : public BaseConfig
	{
	public:
		XmlConfig(SoulFab::Base::String path);
		XmlConfig(const SoulFab::Data::XMLNode* node);

		virtual ~XmlConfig();

		virtual SoulFab::Base::String get(std::string key) const;
		virtual std::shared_ptr<IConfig> getConfig(std::string key) const;

	private:
		bool Owned;
		SoulFab::Data::XMLNode* Root;

		const SoulFab::Data::XMLNode* SubNode(const SoulFab::Data::XMLNode* node, const std::string key) const;
		const SoulFab::Data::XMLNode* getNode(const std::vector<std::string>& key, int last) const;
	};

}

#endif
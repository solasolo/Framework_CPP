#pragma once

#ifndef TELEGRAMSCHEME_H
#define TELEGRAMSCHEME_H

#include <vector>
#include <map>
#include <string>
#include <strstream>

#include "SimpleXML.hpp"
#include "FieldDefine.hpp"
#include "CommonDef.hpp"

namespace SoulFab::Link
{
	struct TelegramDef
	{
		int Length;
		bool VariableLength;

		std::vector<SoulFab::Data::FieldDef> FieldDefs;
		std::map<std::string, std::vector<SoulFab::Data::FieldDef>> RepeatParts;
	};


	class SHARED_EXPORT TelegramScheme
	{
	public:
		bool IsBig;
		bool IsFix;
		bool PaddingLeft;

		TelegramScheme(const std::string& file_name);

		TelegramDef& GetTelgram(const std::string tel_name);
		std::vector<SoulFab::Data::FieldDef>& GetRepatParts(const std::string tel_name, const std::string Name);
	private:
		std::map<std::string, TelegramDef> ProtocolDefs;

		void ReadTelgrams(const SoulFab::Data::XMLNode& root);
		void ReadNode(const SoulFab::Data::XMLNode& node, SoulFab::Data::FieldDef& Value);
		void ReadTelgram(const SoulFab::Data::XMLNode& root, TelegramDef& FieldList);
	};

}

#endif
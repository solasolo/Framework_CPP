#include "TelegramScheme.hpp"
#include "Exception.hpp"
#include "StringHelper.hpp" 

#include <memory>
#include <utility>

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::Data;

namespace SoulFab::Link
{
	TelegramScheme::TelegramScheme(const string& file_name)
	{
		IsBig = false;
		IsFix = false;
		PaddingLeft = false;

		unique_ptr<XMLNode> root(XMLNode::Load(file_name));

		if (root == NULL)
		{
			throw Exception(string("Bad File:") + file_name, "Loading Telegram Scheme File");
		}

		string str = root->getValue("ByteOrder", "Big");
		if (str == "Big")
		{
			IsBig = true;
		}


		str = root->getValue("NubmerFormat", "Fix");
		if (str == "Fix")
		{
			IsFix = true;
		}


		str = root->getValue("Padding", "Left");
		if (str == "Left")
		{
			PaddingLeft = true;
		}

		ReadTelgrams(*root);
	}

	TelegramDef& TelegramScheme::GetTelgram(string tel_name)
	{
		map<string, TelegramDef>::iterator it = ProtocolDefs.find(tel_name);
		if (it == ProtocolDefs.end())
		{
			string msg = "Telegram is not exist! Telegram Name: " + tel_name;
			throw exception(msg.c_str());
		}

		return it->second;
	}

	void TelegramScheme::ReadTelgrams(const XMLNode& root)
	{
		auto child_nodes = root.getChildren();
		for (auto& n : child_nodes)
		{
			string TableName;

			if (n->hasValue("Name"))
			{
				TelegramDef td;
				td.Length = 0;

				string TelegramName = n->getValue("Name");
				auto item = make_pair(TelegramName, td);
				auto InsPtr = ProtocolDefs.insert(move(item));

				ReadTelgram(*n, InsPtr.first->second);
			}
		}
	}

	void TelegramScheme::ReadNode(const XMLNode& node, FieldDef& Value)
	{
		Value.Clear();

		if (node.hasValue("Name"))
		{
			Value.Name = node.getValue("Name");
		}

		if (node.hasValue("Type"))
		{
			Value.SetType(node.getValue("Type"));
		}

		if (node.hasValue("Length"))
		{
			Value.Length = node.getIntValue("Length");
		}

		if (node.hasValue("LengthField"))
		{
			Value.RepeatCountField = static_cast<const string>(node.getValue("LengthField"));
			StringHelper::toUpper(Value.RepeatCountField);
		}

		if (node.hasValue("Decimal"))
		{
			Value.Dec = node.getIntValue("Decimal");
		}

		if (node.hasValue("Format"))
		{
			Value.Format = node.getValue("Format");;
		}
	}

	void TelegramScheme::ReadTelgram(const XMLNode& root, TelegramDef& defs)
	{
		FieldDef Value;

		auto child_nodes = root.getChildren();
		for (auto& n : child_nodes)
		{
			ReadNode(*n, Value);
			defs.FieldDefs.push_back(Value);
			if (Value.RepeatCountField != "")
			{
				defs.VariableLength = true;
			}

			if (Value.Type == FieldType::Repeat)
			{
				string RepeatName = Value.Name;
				int RepeatLength = Value.Length;
				vector<FieldDef> Repeater;

				int SubLength = 0;
				auto child_nodes = n->getChildren();
				for (auto& rn : child_nodes)
				{
					ReadNode(*rn, Value);
					Repeater.push_back(Value);

					SubLength += Value.Length;
				}

				defs.RepeatParts[RepeatName] = Repeater;
				defs.Length += RepeatLength * SubLength;
			}
			else
			{
				defs.Length += Value.Length;
			}
		}
	}

	vector<FieldDef>& TelegramScheme::GetRepatParts(string tel_name, string Name)
	{
		map< string, vector<FieldDef> >::iterator  v = GetTelgram(tel_name).RepeatParts.find(Name);
		if (v == GetTelgram(tel_name).RepeatParts.end())
		{
			string msg = "Telegram is not exist! Telegram Name: " + tel_name + "RepeatParts: " + Name;
			throw Exception(msg);
		}
		return v->second;
	}
}

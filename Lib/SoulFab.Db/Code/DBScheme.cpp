#include "DBScheme.hpp"

#include "StringHelper.hpp"
#include "Exception.hpp"

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::Data;

namespace SoulFab::Db
{
	void DBScheme::Add(string table_name, const TableDef& def)
	{
		StringHelper::toLower(table_name);
		this->Scheme[table_name] = def;
	}

	const TableDef& DBScheme::GetTable(string table_name)
	{
		StringHelper::toLower(table_name);

		auto it = this->Scheme.find(table_name);
		if (it == this->Scheme.end())
			throw Exception("Table(" + String(table_name) + ") is NOT in the Database Schmem!", "Get Table");

		return it->second;
	}

	const FieldDef* DBScheme::GetFieldDef(string table_name, string field_name)
	{
		const FieldDef* ret = nullptr;

		StringHelper::toLower(field_name);
		StringHelper::toLower(table_name);

		const TableDef& td = GetTable(table_name);
		for (auto& item : td)
		{
			if (item.Name == field_name)
			{
				ret = &item;
				break;
			}
		}

		if (!ret)
		{
			// throw Exception("Field [" + String(field_name) + "] Not Found in Database Scheme!", "Get Field");
		}

		return ret;
	}

	XmlDbScheme::XmlDbScheme()
	{
	}

	DBScheme* XmlDbScheme::Build(String file_name)
	{
		unique_ptr<XMLNode> root(XMLNode::Load(file_name));

		return this->ReadTables(*root);
	}

	DBScheme* XmlDbScheme::ReadTables(const XMLNode& root)
	{
		DBScheme* ret = new DBScheme();

		for (auto node : root.getChildren())
		{
			string TableName = static_cast<const string>(node->getValue("Name"));

			TableDef td;
			this->ReadTable(*node, td);

			ret->Add(TableName, td);
		}

		return ret;
	}

	void XmlDbScheme::ReadTable(const XMLNode& root, list<FieldDef>& FieldList)
	{
		for (auto node : root.getChildren())
		{
			FieldDef Value;
			Value.Clear();

			Value.Name = node->getIntValue("Name");
			StringHelper::toLower(Value.Name);

			if (node->hasValue("Type"))
			{
				Value.SetType((const string)(node->getValue("Type")));
			}

			if (node->hasValue("Length"))
			{
				Value.Length = node->getIntValue("Length");
			}

			FieldList.push_back(Value);
		}
	}
}

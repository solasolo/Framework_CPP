#include "PgScheme.hpp"

#include "UniData.hpp"
#include "String.hpp"

#include <stdlib.h>

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::Data;

namespace SoulFab::Db
{
	void PgScheme::Build(DBAccessPtr& accessor)
	{
		String sql = "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'";
		UniData&& data = accessor->Query(sql);

		auto row = data.getList();
		for (auto r : *row)
		{	
			String table_name = r.getString("table_name");

			sql = "SELECT cl.column_name AS name, cl.is_nullable AS nullable, data_type AS type, cl.character_maximum_length AS length, st.constraint_type AS key FROM INFORMATION_SCHEMA.COLUMNS cl LEFT JOIN(SELECT kc.table_name, kc.column_name, ts.constraint_type FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE kc JOIN INFORMATION_SCHEMA.TABLE_CONSTRAINTS ts ON kc.constraint_name = ts.constraint_name) st ON st.column_name = cl.column_name and st.table_name = cl.table_name WHERE cl.table_name = '" + table_name + "'";
			UniData&& field_data = accessor->Query(sql);

			TableDef table;
			auto field_row = field_data.getList();
			for (auto fr : *field_row)
			{
				// printf(fr.toDebugString().Pointer());
				
				String type = fr.getString("type");

				FieldDef field;
				field.Name = fr.getString("name");
				if (!fr.isNull("length"))
				{
					field.Length = fr.getInt("length");
				}
				else
				{
					field.Length = 0;
				}
				field.Type = ConvertFieldType(type);

				if (!fr.isNull("key"))
				{
					string key = fr.getString("key");
					field.IsKey = (key == "PRIMARY KEY");
				}

				table.push_back(field);
			}

			this->Add(table_name, table);
		}
	}

	FieldType PgScheme::ConvertFieldType(string type_name)
	{
		FieldType ret = FieldType::String;

		if (type_name.find("character") != string::npos)
		{
			ret = FieldType::String;
		}
		else if (type_name.find("integer") != string::npos)
		{
			ret = FieldType::Int;
		}
		else if (type_name.find("double") != string::npos)
		{
			ret = FieldType::Float;
		}
		else if (type_name.find("timestamp") != string::npos)
		{
			ret = FieldType::DateTime;
		}

		return ret;
	}

}
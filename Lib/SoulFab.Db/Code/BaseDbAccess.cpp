#include "BaseDbAccess.hpp"

#include "CommonDef.hpp"

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::Data;

namespace SoulFab::Db
{
	BaseDB::BaseDB(LinkParam& param)
		: LogSource("DB"), ConnectionParams(param), Builder(nullptr)
	{
	}

	BaseDB::~BaseDB()
	{
		SafeDestroy(this->Builder);
	}

	SQLBuilder* BaseDB::getBuilder()
	{
		if (!this->Builder) 
		{
			this->Builder = this->CreateBuilder();
		}

		return this->Builder;
	}

	//
	//
	//

	bool BaseDbAccess::IfExist(const string table_name, const string field_name, const UniData& value)
	{
		UniData cond;
		cond.set(field_name, value);
		String sql = Builder->MakeNullSelect(table_name, cond);

		return this->Exist(sql);
	}

	UniData BaseDbAccess::Select(string table_name)
	{
		UniData cond;

		String sql = Builder->MakeSelect(table_name, cond);

		return this->Query(sql);
	}

	UniData BaseDbAccess::Select(string table_name, String condition)
	{
		UniData dump;

		String sql = Builder->MakeSelect(table_name, dump);
		sql = sql + " where " + condition;

		return this->Query(sql);
	}

	UniData BaseDbAccess::Select(string table_name, string field_name, const UniData& value)
	{
		UniData cond;
		cond.set(field_name, value);

		String sql = Builder->MakeSelect(table_name, cond);

		return this->Query(sql);
	}

	UniData BaseDbAccess::Select(string table_name, const UniData& cond)
	{
		String sql = Builder->MakeSelect(table_name, cond);

		return this->Query(sql);
	}

	UniData BaseDbAccess::SelectTop(string table_name)
	{
		UniData cond;

		String sql = Builder->MakeSelect(table_name, cond);

		return this->QueryTop(sql);
	}

	UniData BaseDbAccess::SelectTop(string table_name, String condition)
	{
		UniData dump;

		String sql = Builder->MakeSelect(table_name, dump);
		sql += " where " + condition;

		return this->QueryTop(sql);
	}

	UniData BaseDbAccess::SelectTop(string table_name, const string field_name, const UniData& value)
	{
		UniData cond;
		cond.set(field_name, value);

		String sql = Builder->MakeSelect(table_name, cond);

		return this->QueryTop(sql);
	}

	UniData BaseDbAccess::SelectTop(string table_name, const UniData& cond)
	{
		String sql = Builder->MakeSelect(table_name, cond);

		return this->QueryTop(sql);
	}

	int BaseDbAccess::Update(string table_name, const UniData& data, const string field_name, const UniData& value)
	{
		UniData Cond;
		Cond.set(field_name, value);
		String sql = Builder->MakeUpdate(table_name, data, Cond);

		return this->Execute(sql);
	}

	int BaseDbAccess::Update(string table_name, const UniData& data, const UniData& cond)
	{
		String sql = Builder->MakeUpdate(table_name, data, cond);

		return this->Execute(sql);
	}

	int BaseDbAccess::Insert(string table_name, const UniData& data)
	{
		String sql = Builder->MakeInsert(table_name, data);

		return this->Execute(sql);
	}

    int BaseDbAccess::Update(string table_name, const UniData& data)
    {
        String sql = Builder->MakeUpdate(table_name, data);

        return this->Execute(sql);
    }

    int BaseDbAccess::Add(string table_name, const UniData& data)
    {
        int Result = 0;

        if (!this->Update(table_name, data))
        {
            Result = this->Insert(table_name, data);
        }

        return Result;
    }

	int BaseDbAccess::Add(string table_name, const string key_field, const UniData& data)
	{
		int Result = 0;

		auto&& fields = StringHelper::Split(key_field, ';');

		UniData cond;
		for (auto& f : fields)
		{
			const UniData& value = ((UniData&)data)[f];
			cond.set(f, value);
		}

		if (!Update(table_name, data, cond))
		{
			Result = Insert(table_name, data);
		}

		return Result;
	}

	int BaseDbAccess::Delete(string table_name, const UniData& data)
	{
		String sql = Builder->MakeDelete(table_name, data);

		return this->Execute(sql);
	}

	int BaseDbAccess::Delete(string table_name, const string field_name, const UniData& value)
	{
		UniData Cond;
		Cond.set(field_name, value);
		String sql = Builder->MakeDelete(table_name, Cond);

		return this->Execute(sql);
	}
}

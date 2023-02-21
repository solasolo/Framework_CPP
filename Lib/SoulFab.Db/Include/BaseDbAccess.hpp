#pragma once

#ifndef _SF_BASE_DB_ACCESS_H_
#define _SF_BASE_DB_ACCESS_H_

#include <string>
#include <memory>
#include <list>

#include "BaseEvent.hpp"
#include "Exception.hpp"
#include "UniData.hpp"
#include "SQLBuilder.hpp"
#include "String.hpp"

namespace SoulFab::Db
{
	class SqlException : public SoulFab::Base::Exception
	{

	};

	struct LinkParam
	{
		std::string Host;
		int Port;
		std::string Db;
		std::string User;
		std::string Password;
	};

	class IBaseDBAccessor 
	{
	public:
		virtual ~IBaseDBAccessor() {};

		virtual SoulFab::Data::UniData Query(const SoulFab::Base::String& Sql) = 0;
		virtual SoulFab::Data::UniData QueryTop(const SoulFab::Base::String& Sql) = 0;
		virtual int Execute(const SoulFab::Base::String& Sql) = 0;
		virtual bool Exist(const SoulFab::Base::String& sql) = 0;
	};

	class SHARED_EXPORT BaseDbAccess : public IBaseDBAccessor
	{
	public:
		// ORM
		SoulFab::Data::UniData Select(std::string table_name);
		SoulFab::Data::UniData Select(std::string table_name, SoulFab::Base::String condition);
		SoulFab::Data::UniData Select(std::string table_name, std::string field_name, const SoulFab::Data::UniData& value);
		SoulFab::Data::UniData Select(std::string table_name, const SoulFab::Data::UniData& cond);

		SoulFab::Data::UniData SelectTop(std::string table_name);
		SoulFab::Data::UniData SelectTop(std::string table_name, SoulFab::Base::String condition);
		SoulFab::Data::UniData SelectTop(std::string table_name, std::string field_name, const SoulFab::Data::UniData& value);
		SoulFab::Data::UniData SelectTop(std::string table_name, const SoulFab::Data::UniData& cond);;

		int Insert(std::string table_name, const SoulFab::Data::UniData& data);
        int Update(std::string table_name, const SoulFab::Data::UniData& data);
        int Add(std::string table_name, const SoulFab::Data::UniData& data);

		int Update(std::string table_name, const SoulFab::Data::UniData& data, const SoulFab::Data::UniData& cond);
		int Update(std::string table_name, const SoulFab::Data::UniData& data, std::string field_name, const SoulFab::Data::UniData& value);
		
        int Add(std::string table_name, std::string key_field, const SoulFab::Data::UniData& data);

		int Delete(std::string table_name, const SoulFab::Data::UniData& data);
		int Delete(std::string table_name, std::string field_name, const SoulFab::Data::UniData& value);

		bool IfExist(std::string table_name, std::string field_name, const SoulFab::Data::UniData& value);

	protected:
		SoulFab::Base::String LastSQL;
		SQLBuilder* Builder;
	};

	using DBAccessPtr = std::unique_ptr<BaseDbAccess>;

	class IBaseDB
	{
	public:
		virtual DBAccessPtr getAccess() = 0;
	};

	class SHARED_EXPORT BaseDB : public IBaseDB, public SoulFab::Base::LogSource
	{
	public:
		BaseDB(LinkParam& param);
		virtual ~BaseDB();
		
		SQLBuilder* getBuilder();

	protected:
		LinkParam ConnectionParams;
		SQLBuilder* Builder;

		virtual SQLBuilder* CreateBuilder() = 0;
	};

}

#endif

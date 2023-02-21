#pragma once

#ifndef _SF_PG_DB_ACCESS_H_
#define _SF_PG_DB_ACCESS_H_

#include "BaseDbAccess.hpp"

#include <vector>
#include <libpq-fe.h>

#include "PgWrap.hpp"

namespace SoulFab::Db
{
	class PgSQLBuilder : public SQLBuilder
	{
	public:
		PgSQLBuilder(DBScheme* scheme);

	protected:
		virtual SoulFab::Base::String DateTimeValue(const SoulFab::Base::DateTime& time);
	};

	class SHARED_EXPORT PgDB : public BaseDB
	{
	public:
		PgDB(LinkParam& param);

		virtual DBAccessPtr getAccess();

		PgConnection getConnection();

	protected:
		SQLBuilder* CreateBuilder() override;

		SQLBuilder* Builder;
	};

	class PgDbAccess : public BaseDbAccess
	{
	public:
		PgDbAccess(PgDB* db, bool no_builder = false);

		virtual SoulFab::Data::UniData Query(const SoulFab::Base::String& Sql) override;
		virtual SoulFab::Data::UniData QueryTop(const SoulFab::Base::String& Sql) override;
		virtual int Execute(const SoulFab::Base::String& Sql) override;
		virtual bool Exist(const SoulFab::Base::String& sql) override;

	private:
		PgDB* Db;

		void Open();
		void Close();

		PgResult DoQuery(const PgConnection& conn, const SoulFab::Base::String& sql);
	};

}

#endif

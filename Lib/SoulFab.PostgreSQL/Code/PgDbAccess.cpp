#include "PgDbAccess.hpp"

#include <utility>

#include "PgWrap.hpp"
#include "PgScheme.hpp"

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::Data;

namespace SoulFab::Db
{
	PgSQLBuilder::PgSQLBuilder(DBScheme* scheme)
		: SQLBuilder(scheme)
	{
	}

	String PgSQLBuilder::DateTimeValue(const DateTime& time)
	{
		return "'" + time.ISOString() + "'";
	}


	PgDB::PgDB(LinkParam& param)
		: BaseDB(param)
	{
		if (this->ConnectionParams.Port == 0) this->ConnectionParams.Port = 5432;
	}

	DBAccessPtr PgDB::getAccess()
	{
		return unique_ptr<BaseDbAccess>(new PgDbAccess(this));
	}

	PgConnection PgDB::getConnection()
	{
		PgConnection conn;

		conn.Open(this->ConnectionParams);

		return conn;
	}

	SQLBuilder* PgDB::CreateBuilder()
	{
		auto accessor = unique_ptr<BaseDbAccess>(new PgDbAccess(this, true));

		PgScheme* scheme = new PgScheme();
		scheme->Build(accessor);

		return new PgSQLBuilder(scheme);
	}

	
	//
	//
	//

	PgDbAccess::PgDbAccess(PgDB* db, bool no_builder)
		: Db(db)
	{
		if (!no_builder)
		{
			this->Builder = db->getBuilder();
		}
	}

	UniData PgDbAccess::Query(const String& sql)
	{
		PgConnection&& conn = this->Db->getConnection();

		PgResult&& res = this->DoQuery(conn, sql);

		UniData&& ret = res.FetchReault();

		return ret;
	}

	UniData PgDbAccess::QueryTop(const String& sql)
	{
		PgConnection&& conn = this->Db->getConnection();

		PgResult&& res = this->DoQuery(conn, sql);
	}

	int PgDbAccess::Execute(const String& sql)
	{
		PgConnection&& conn = this->Db->getConnection();

		PgResult&& res = this->DoQuery(conn, sql);
		
		return res.Affected();
	}

	bool PgDbAccess::Exist(const String& sql)
	{
		PgConnection&& conn = this->Db->getConnection();

		PgResult&& res = this->DoQuery(conn, sql);
	}

    PgResult PgDbAccess::DoQuery(const PgConnection& conn, const String& sql)
	{
        PgResult res = PQexec(conn.get(), sql.Pointer());
		auto status = res.Status();

		if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
		{
			throw PgSqlException(res);
		}

		return res;
	}

}

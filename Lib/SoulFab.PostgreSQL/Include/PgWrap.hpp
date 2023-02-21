#pragma once

#ifndef _SF_PG_WRAP_H_
#define _SF_PG_WRAP_H_

#include <vector>
#include <string>
#include <memory>
#include <libpq-fe.h>

#include "BaseDbAccess.hpp"

namespace SoulFab::Db
{
	struct PgField
	{
		std::string Name;
		Oid Type;
	};

    class PgResult;

	class PgSqlException : public SoulFab::Db::SqlException
	{
	public:
		PgSqlException(const PgResult& res);
		PgSqlException(const PGconn* conn);

	};

	class PgConnection
	{
	public:
		PgConnection();
		~PgConnection();

		PgConnection(PgConnection&& other);
        PgConnection& operator=(PgConnection&& other);

		void Open(const SoulFab::Db::LinkParam& param);
		PGconn* get() const;

		PGconn* operator &();

	private:
		PGconn* Connection;
	};

	class PgCommand
	{
	private:

	};

	class PgResult
	{
	public:
		PgResult(PGresult* result);
        ~PgResult();
        
        PgResult(PgResult&& other);
        PgResult& operator=(PgResult&& other);

		SoulFab::Data::UniData FetchReault();
		void FetchFields(std::vector<PgField>& fields);
		SoulFab::Data::UniData FetchRow(int row, const std::vector<PgField>& fields);
		int Affected() const;
        int Status() const;
        SoulFab::Base::String ErrorMessage() const;

	private:
		PGresult* Result;

		static std::string StringToBin(const std::string& data);
	};


	inline PGconn* PgConnection::operator &()
	{
		return this->Connection;
	}

	inline PGconn* PgConnection::get() const
	{
		return this->Connection;
	}

}

#endif

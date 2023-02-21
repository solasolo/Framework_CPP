#include "PgWrap.hpp"

using namespace std;
using namespace SoulFab::Data;
using namespace SoulFab::Base;

namespace SoulFab::Db
{
	PgSqlException::PgSqlException(const PgResult& res)
	{
		this->Message = res.ErrorMessage();
	}

	PgSqlException::PgSqlException(const PGconn* conn)
	{
		this->Message = PQerrorMessage(conn);
	}


	PgConnection::PgConnection()
		: Connection(nullptr)
	{
	}

	PgConnection::PgConnection(PgConnection&& other)
	{
		this->Connection = other.Connection;
        other.Connection = nullptr;
	}

    PgConnection& PgConnection::operator=(PgConnection&& other)
    {
        this->Connection = other.Connection;
        other.Connection = nullptr;

        return *this;
    }
	
	PgConnection::~PgConnection()
	{
		if (this->Connection) 
		{
			PQfinish(this->Connection);
			
			this->Connection = nullptr;
		}
	}

	void PgConnection::Open(const LinkParam& param)
	{
		if (!this->Connection)
		{
			string port_str = to_string(param.Port);

			this->Connection = PQsetdbLogin(
				param.Host.c_str(),
				port_str.c_str(),
				nullptr,
				nullptr,
				param.Db.c_str(),
				param.User.c_str(),
				param.Password.c_str()
			);
		}

		auto status = PQstatus(this->Connection);
		if (status != ConnStatusType::CONNECTION_OK)
		{
			throw PgSqlException(this->Connection);
		}
	}

	//
	//
	//

	PgResult::PgResult(PGresult* result)
		: Result(nullptr)
	{
		this->Result = result;
	}

    PgResult::PgResult(PgResult&& other)
    {
        this->Result = other.Result;
        other.Result = nullptr;
    }

    PgResult& PgResult::operator=(PgResult&& other)
    {
        this->Result = other.Result;
        other.Result = nullptr;

        return *this;
    }


	PgResult::~PgResult()
	{
		if (this->Result)
		{
			PQclear(this->Result);
		}
	}

	UniData PgResult::FetchReault()
	{
		UniData ret = UniData::List();

		int row_count = PQntuples(this->Result);
		int col_count = PQnfields(this->Result);

		vector<PgField> fields;
		this->FetchFields(fields);

		for (int row = 0; row < row_count; row++)
		{
			auto&& data = this->FetchRow(row, fields);
			ret.Add(data);
		}

		return ret;
	}

	void PgResult::FetchFields(vector<PgField>& fields)
	{
		int col_count = PQnfields(this->Result);
		fields.resize(col_count);

		for (int i = 0; i < col_count; i++)
		{
			string name = PQfname(this->Result, i);
			auto type = PQftype(this->Result, i);
		
			fields[i] = PgField { name, type };
		}
	}

	UniData PgResult::FetchRow(int row, const vector<PgField>& fields)
	{
		UniData ret = UniData::Object();

		int count = fields.size();
		for (int i = 0; i < count; i++)
		{
			auto& field = fields[i];
			string name = field.Name;
			Oid type = field.Type;

			if (PQgetisnull(this->Result, row, i))
			{
				ret.setNull(name);
			}
			else
			{
				string value = PQgetvalue(this->Result, row, i);

				if (type == 17) // BYTEAOID
				{
					value = StringToBin(value);
				}

				ret.set(name, value);
			}
		}

		return ret;
	}

    int PgResult::Affected() const
	{
		int ret = 0;

		char* affected = PQcmdTuples(this->Result);
		if (affected)
		{
			ret = atoi(affected);
		}

 		return ret;
	}


    int PgResult::Status() const
    {
        return PQresultStatus(this->Result);
    }

    String PgResult::ErrorMessage() const
    {
        return PQresultErrorMessage(this->Result);
    }

	string PgResult::StringToBin(const string& data)
	{
		string ret;

		size_t bin_len = 0;
		unsigned char* bin = PQunescapeBytea((unsigned char*)data.c_str(), &bin_len);
		if (bin_len > 0)
		{
			ret.append((char*)bin, bin_len);
		}
		else
		{
			// TODO;
		}
		
		PQfreemem(bin);

		return ret;
	}
}

#pragma once

#ifndef _SF_PG_SCHEME_H_
#define _SF_PG_SCHEME_H_

#include "PgDbAccess.hpp"
#include "DBScheme.hpp"

namespace SoulFab::Db
{
	class PgScheme : public DBScheme
	{
	public:
		void Build(DBAccessPtr& accessor);

	private:
		static SoulFab::Data::FieldType ConvertFieldType(std::string type_name);
	};

}

#endif
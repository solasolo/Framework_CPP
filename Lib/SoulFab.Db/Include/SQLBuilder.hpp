#pragma once

#ifndef _SF_SQL_BUILDER_H_
#define _SF_SQL_BUILDER_H_

#include <string>

#include "CommonDef.hpp"
#include "UniData.hpp"
#include "DBScheme.hpp"
#include "String.hpp"

namespace SoulFab::Db
{
    class SHARED_EXPORT SQLBuilder
    {
    public:
        SQLBuilder(DBScheme* scheme);
        virtual ~SQLBuilder();

        SoulFab::Base::String MakeNullSelect(const std::string table_name, const SoulFab::Data::UniData& conds);
        SoulFab::Base::String MakeSelect(const std::string table_name, const SoulFab::Data::UniData& conds);
        SoulFab::Base::String MakeInsert(const std::string table_name, const SoulFab::Data::UniData& values);
        SoulFab::Base::String MakeUpdate(const std::string table_name, const SoulFab::Data::UniData& values);
        SoulFab::Base::String MakeUpdate(const std::string table_name, const SoulFab::Data::UniData& values, const SoulFab::Data::UniData& conds);
        SoulFab::Base::String MakeDelete(const std::string table_name, const SoulFab::Data::UniData& conds);

    private:
        DBScheme* Scheme; 

        SoulFab::Base::String MakeCondition(const std::string table_name, const SoulFab::Data::UniData& values);
        SoulFab::Base::String MakeValue(const SoulFab::Data::FieldDef& fd, const SoulFab::Data::UniData& values);

    protected:
        virtual  SoulFab::Base::String DateTimeValue(const SoulFab::Base::DateTime& time) = 0;
    };
}

#endif
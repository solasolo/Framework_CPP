#pragma once

#ifndef _SF_DB_SCHEME_H_
#define _SF_DB_SCHEME_H_

#include <string>
#include <map>
#include <list>

#include "StringHelper.hpp"
#include "BaseConfig.hpp"
#include "FieldDefine.hpp"
#include "SimpleXML.hpp"

namespace SoulFab::Db
{
    typedef std::list<SoulFab::Data::FieldDef> TableDef;

    class SHARED_EXPORT DBScheme
    {
    protected:
        std::map<std::string, TableDef> Scheme;

    public:
        void Add(std::string table_name, const TableDef& def);

        const TableDef& GetTable(std::string table_name);
        const SoulFab::Data::FieldDef* GetFieldDef(std::string table_name, std::string field_name);
    };

    class XmlDbScheme
    {
    private:
        DBScheme* ReadTables(const SoulFab::Data::XMLNode& node);
        void ReadTable(const SoulFab::Data::XMLNode& node, TableDef& FieldList);

    public:
        XmlDbScheme();
        
        DBScheme* Build(SoulFab::Base::String file_name);
    };
}

#endif

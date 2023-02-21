#include "SQLBuilder.hpp"

#include <sstream>

#include "DBScheme.hpp"
#include "CommonDef.hpp"

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::Data;

namespace SoulFab::Db
{
    SQLBuilder::SQLBuilder(DBScheme* scheme)
        : Scheme(nullptr)
    {
        this->Scheme = scheme;
    }

    SQLBuilder::~SQLBuilder()
    {
        SafeDestroy(Scheme);
    }

    String SQLBuilder::MakeValue(const FieldDef& fd, const UniData& values)
    {
        String Result;
        string FieldName = fd.Name;
        FieldType Type = fd.Type;

        const UniData& data = const_cast<UniData&>(values)[FieldName];

        if (values.isNull())
        {
            Result = "null";
        }
        else
        {
            char temp[100];
            DateTime time;

            switch (Type)
            {
            case FieldType::String:
                Result = Result + "'" + (String)data + "'";
                break;

            case FieldType::DateTime:
                time = data;
                Result = this->DateTimeValue(time);

                /*
                if(this->DBType == DbType::Oracle)
                {
                    double t = values.getFloat(FieldName);

                    Result = SQLBuilder::MakeOracleDate(t);
                }
                else if(this->DBType == DbType::MSQL)
                {
                    //转换为时间
                    //string str = DateTimeFormatToCTS(t);
                    //swprintf(temp, "'%s'", str.c_str());

                    sprintf(temp, "%f", values.getFloat(FieldName) - 2); // DBTime = SysTime - 2
                    Result = temp;
                }
                */
                break;

            case FieldType::Float:
                sprintf(temp, "%f", (double)data);
                Result = temp;
                break;

            case FieldType::Int:
                sprintf(temp, "%d", (int)data);
                Result = temp;
                break;

            default:
                Result = (String)data;
            }
        }

        return Result;
    }

    String SQLBuilder::MakeCondition(const string table_name, const UniData& values)
    {
        bool IsFound = false;;
        bool First = true;
        StringBuffer buffer;

        auto temp = values.getFiledNames();
        int FiledIndex = 0;

        if (temp.size() > 0)
        {
            buffer << " where ";

            do
            {
                if (!First) buffer << " and ";

                string FieldName = temp[FiledIndex];
                const FieldDef* fd = Scheme->GetFieldDef(table_name, FieldName);

                if (!fd)
                {
                    FiledIndex = FiledIndex + 1;
                    continue;
                }

                const UniData& data = values.get(FieldName);
                if (data.isNull())
                {
                    buffer << FieldName << " is null";
                }
                else
                {
                    buffer << FieldName << "=" << MakeValue(*fd, values);
                }

                FiledIndex = FiledIndex + 1;
                First = false;
            }             
            while (FiledIndex < temp.size());
        }

        return buffer.Str();
    }

    String SQLBuilder::MakeNullSelect(const string table_name, const UniData& conds)
    {
        StringBuffer buffer;

        const list<FieldDef>& fds = Scheme->GetTable(table_name);
        auto b = fds.begin();

        buffer << "select NULL ";
        buffer << "from " << table_name;
        buffer << MakeCondition(table_name, conds);

        return buffer.Str();
    }

    String SQLBuilder::MakeSelect(const string table_name, const UniData& conds)
    {
        StringBuffer buffer;

        const list<FieldDef>& fds = Scheme->GetTable(table_name);
        auto b = fds.begin();

        buffer << "select ";
        while (b != fds.end())
        {
            if (b != fds.begin()) buffer << ",";
            buffer << b->Name.c_str();
            b++;
        }
        buffer << " from " << table_name;
        buffer << MakeCondition(table_name, conds);

        return buffer.Str();
    }


    String SQLBuilder::MakeInsert(const string table_name, const UniData& values)
    {
        bool FirstItem;

        const list<FieldDef>& fds = Scheme->GetTable(table_name);

        FirstItem = true;
        StringBuffer FieldsBuffer;
        StringBuffer ValueBuffer;

        auto b = fds.begin();
        while (b != fds.end())
        {
            string FielsdName = b->Name;
            if (values.has(FielsdName))
            {
                if (!FirstItem) FieldsBuffer << ",";
                FieldsBuffer << b->Name;

                String ValueString = MakeValue(*b, values);
                if (!ValueString.Empty())
                {
                    if (!FirstItem)
                    {
                        ValueBuffer << ",";
                    }

                    ValueBuffer << ValueString;

                    FirstItem = false;
                }
                else
                {
                    ValueBuffer << "null";
                }

                FirstItem = false;
            }

            b++;
        }

        StringBuffer buffer;
        buffer << "insert into " << table_name.c_str() << " (" << FieldsBuffer.Str() << ") values (" << ValueBuffer.Str() << ")";

        return buffer.Str();
    }

    String SQLBuilder::MakeUpdate(const string table_name, const UniData& values)
    {
        bool FirstItem;
        StringBuffer buffer;
        UniData conds = UniData::Object();

        buffer << "update ";
        buffer << table_name;
        buffer << " set ";

        FirstItem = true;
        const list<FieldDef>& fds = Scheme->GetTable(table_name);
        auto b = fds.begin();
        while (b != fds.end())
        {
            string FielsdName = b->Name;

            if (values.has(FielsdName))
            {
                if (!b->IsKey)
                {
                    if (!FirstItem) buffer << ",";

                    buffer << b->Name.c_str() << "=";
                    buffer << MakeValue(*b, values);

                    FirstItem = false;
                }
                else
                {
                    conds[b->Name] = const_cast<UniData&>(values)[b->Name];
                }
            }

            b++;
        }

        buffer << MakeCondition(table_name, conds);

        return buffer.Str();
    }

    String SQLBuilder::MakeUpdate(const string table_name, const UniData& values, const UniData& conds)
    {
        bool FirstItem;
        StringBuffer buffer;

        buffer << "update ";
        buffer << table_name;
        buffer << " set ";

        FirstItem = true;
        const list<FieldDef>& fds = Scheme->GetTable(table_name);
        auto b = fds.begin();
        while (b != fds.end())
        {
            string FielsdName = b->Name;

            if (values.has(FielsdName))
            {
                if (!FirstItem) buffer << ",";

                buffer << b->Name.c_str() << "=";
                buffer << MakeValue(*b, values);

                FirstItem = false;
            }

            b++;
        }

        buffer << MakeCondition(table_name, conds);

        return buffer.Str();
    }


    String SQLBuilder::MakeDelete(const string table_name, const UniData& conds)
    {
        StringBuffer buffer;

        buffer << "delete from " << table_name;
        buffer << MakeCondition(table_name, conds);

        return buffer.Str();
    }
}

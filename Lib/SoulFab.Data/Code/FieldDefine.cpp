#include "FieldDefine.hpp"

#include "StringHelper.hpp"

using namespace std;
using namespace SoulFab::Base;

namespace SoulFab::Data
{
    FieldDef::FieldDef()
    {
        this->Clear();
    }

    void FieldDef::Clear()
    {
        No = 0;
        Length = 0;
        Dec = 0;
        Name = "";
        Type = FieldType::Unknown;
        IsKey = false;
        Format = "";
    }

    void FieldDef::SetType(string name)
    {
        StringHelper::toLower(name);

        if (name == "string")
        {
            Type = FieldType::String;
        }
        else if (name == "int")
        {
            Type = FieldType::Int;
        }
        else if (name == "float")
        {
            Type = FieldType::Float;
            this->Dec = 2;
        }
        else if (name == "datetime")
        {
            Type = FieldType::DateTime;
        }
        else if (name == "repeat")
        {
            Type = FieldType::Repeat;
        }
        else
        {
            Type = FieldType::Unknown;
        }
    }
}

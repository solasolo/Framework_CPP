#pragma once

#ifndef _SF_FIELD_DEF_H_
#define _SF_FIELD_DEF_H_

#include <string>

#include "CommonDef.hpp"
#include "String.hpp"

namespace SoulFab::Data
{
    enum class FieldType
    {
        Unknown,
        String,
        Int,
        Float,
        DateTime,
        Binary,

        Repeat = 100
    };

    struct SHARED_EXPORT FieldDef
    {
        short No;
        std::string Name;
        FieldType Type;
        bool IsKey;
        short Length;
        short Dec;
        std::string Format;
        std::string RepeatCountField;

        FieldDef();

        void Clear();
        void SetType(std::string name);
    };
}

#endif
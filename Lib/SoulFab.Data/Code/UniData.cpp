#include "UniData.hpp"

#include <sstream>

#include "StringHelper.hpp"
#include "Exception.hpp"

using namespace std;
using namespace SoulFab::Base;

namespace SoulFab::Data
{
#pragma region  Static Members

    UniData UniData::Object()
    {
        return UniData(UniTypes::Object);
    }


    UniData UniData::List()
    {
        return UniData(UniTypes::List);
    }

#pragma endregion

    UniData::UniData()
        : Type(UniTypes::Null)
    {
    }

    UniData::UniData(const UniData& item)
        : Type(UniTypes::Null)
    {
        this->Copy(item);
    }

    UniData::UniData(const UniData&& item)
        : Type(UniTypes::Null)
    {
        this->Copy(item);
    }

    UniData::UniData(UniTypes type)
    {
        this->Type = type;

        switch (type)
        {
        case UniTypes::String:
            this->PointerValue = (string*)nullptr;
            break;    

        case UniTypes::List:
            this->PointerValue = new vector<UniData>();
            break;

        case UniTypes::Object:
            this->PointerValue = new map<string, UniData>();
            break;

        case UniTypes::Int:
            this->PrimeValue.IntValue = 0;
            break;

        case UniTypes::Float:
            this->PrimeValue.FloatValue = 0;
            break;
        }
    }

    void UniData::Clean()
    {
        switch (this->Type)
        {
        case UniTypes::String:
            this->PointerValue = (string*)nullptr;
            break;

        case UniTypes::List:
            this->PointerValue = (ListType*)nullptr;
            break;

        case UniTypes::Object:
            this->PointerValue = (ObjectType*)nullptr;
            break;
        }

        this->Type = UniTypes::Null;
    }

    void UniData::Copy(const UniData& item)
    {
        this->Type = item.Type;

        switch (this->Type)
        {
        case UniTypes::String:
            this->PointerValue.Assign<string>(item.PointerValue);
            break;

        case UniTypes::List:
            this->PointerValue.Assign<ListType>(item.PointerValue);
            break;

        case UniTypes::Object:
            this->PointerValue.Assign<ObjectType>(item.PointerValue);
            break;

        default:
            this->PrimeValue = item.PrimeValue;
        }
    }

    UniData& UniData::operator = (const UniData& other)
    {
        this->Clean();
        this->Copy(other);

        return *this;
    }

    UniData::~UniData()
    {
        this->Clean();
    }

    bool UniData::isNull() const
    {
        return this->Type == UniTypes::Null;
    }

    bool UniData::isEmpty() const
    {
        bool ret = false;;

        switch (this->Type)
        {
        case UniTypes::Null:
            ret = true;
            break;

        case UniTypes::Object:
            break;

        case UniTypes::List:
            break;

        case UniTypes::String:
            break;

        default:
            ret = false;
        }

        return ret;
    }

    UniData::operator long() const
    {
        int ret = 0;

        UniTypes type = this->Type;

        if(type == UniTypes::Int)
        {
            ret = this->PrimeValue.IntValue;
        }
        else if(this->Type == UniTypes::String)
        {
            const string* s = const_cast<UniData*>(this)->PointerValue.get<string>();
            if (s)
            {
                ret = atoi(s->c_str());
            }
            else
            {
                throw Exception("Null String ");
            }
        }
        else if(this->Type == UniTypes::Float)
        {
            ret = int(this->PrimeValue.FloatValue);
        }
        else
        {
            throw Exception("Type UnMacth Int");
        }

        return ret;
    }

    UniData::operator double() const
    {
        double ret;

        if(this->Type == UniTypes::Float)
        {
            ret = this->PrimeValue.FloatValue;
        }
        else if(this->Type == UniTypes::String)
        {
            const string* s = const_cast<UniData*>(this)->PointerValue.get<string>();
            if (s)
            {
                ret = atof(s->c_str());
            }
            else
            {
                throw Exception("Null String ");
            }
        }
        else if(this->Type == UniTypes::Int)
        {
            ret = double(this->PrimeValue.IntValue);
        }
        else
        {
            throw Exception("Type UnMacth Float:");
        }

        return ret;
    }

    UniData::operator String() const
    {
        String ret;

        if(this->Type == UniTypes::String)
        {
            const string* s = const_cast<UniData*>(this)->PointerValue.get<string>();
            if (s) ret = *s;
        }
        else if(this->Type == UniTypes::Int)
        {
            ret = std::to_string(this->PrimeValue.IntValue);
        }
        else if(this->Type == UniTypes::Float)
        {
            ret = std::to_string(this->PrimeValue.FloatValue);
        }
        else if(this->Type == UniTypes::Null)
        {
            ret = "[NULL]";
        }
        else
        {
            throw Exception("Type UnMacth String:");
        }

        return ret;
    }

    UniData::operator string() const
    {
        string ret;

        if (this->Type == UniTypes::String)
        {
            const string* s = const_cast<UniData*>(this)->PointerValue.get<string>();
            if (s) ret = *s;
        }
        else if (this->Type == UniTypes::Int)
        {
            ret = std::to_string(this->PrimeValue.IntValue);
        }
        else if (this->Type == UniTypes::Float)
        {
            ret = std::to_string(this->PrimeValue.FloatValue);
        }
        else if (this->Type == UniTypes::Null)
        {
            ret = "[NULL]";
        }
        else
        {
            throw Exception("Type UnMacth String:");
        }

        return ret;
    }

    UniData::operator DateTime() const
    {
        double value = (*this);

        return DateTime(value);
    }

    UniData& UniData::operator = (short value)
    {
        (*this) = (long)value;

        return *this;
    }

    UniData& UniData::operator = (int value)
    {
        (*this) = (long)value;

        return *this;
    }

    UniData& UniData::operator =(long value)
    {
        this->Clean();

        this->Type = UniTypes::Int;
        this->PrimeValue.IntValue = value;

        return *this;
    }

    UniData& UniData::operator =(double value)
    {
        this->Clean();

        this->Type = UniTypes::Float;
        this->PrimeValue.FloatValue = value;

        return *this;
    }

    UniData& UniData::operator = (const char* value)
    {
        this->Clean();

        this->Type = UniTypes::String;

        string* p = new string();
        *p = value;

        this->PointerValue = p;

        return *this;
    }

    UniData& UniData::operator =(const string value)
    {
        this->Clean();

        this->Type = UniTypes::String;

        string* p = new string();
        *p = value;

        this->PointerValue = p;

        return *this;
    }

    UniData& UniData::operator =(const String value)
    {
        this->Clean();

        this->Type = UniTypes::String;

        string* p = new string();
        *p = value;

        this->PointerValue = p;

        return *this;
    }

    UniData& UniData::operator = (const DateTime& value)
    {
        (*this) = (double)const_cast<DateTime&>(value);

        return *this;
    }

    void UniData::setNull()
    {
        this->Clean();
    }

    UniData::ListType* UniData::getList()
    {
        ListType* ret = nullptr;

        if (this->Type == UniTypes::List)
        {
            if (this->PointerValue)
            {
                ret = this->PointerValue.get<ListType>();
            }
        }
        else
        {
            throw Exception("UniData Not a List");
        }

        return ret;
    }

    UniData::ObjectType* UniData::getObject()
    {
        ObjectType* ret = nullptr;

        if (this->Type == UniTypes::Object)
        {
            if (this->PointerValue)
            {
                ret = this->PointerValue.get<ObjectType>();
            }
        }
        else
        {
            throw Exception("UniData Not an Object");
        }

        return ret;
    }

    vector<string> UniData::getFiledNames() const 
    {
        vector<string> ret;

        const ObjectType* p = ((UniData*)this)->getObject();
        if (p)
        {
            for (auto &v : (*p))
            {
                ret.push_back(v.first);
            }
        }

        return ret;
    }

    bool UniData::has(string name) const
    {
        bool ret = false;

        const ObjectType* obj = ((UniData*)this)->getObject();

        if(obj)
        {
            auto it = obj->find(name);
            if(it != obj->end())
            {
                ret = true;
            }
        }

        return ret;
    }

    const UniData& UniData::TryGet(string name) const
    {
        const ObjectType* obj = ((UniData*)this)->getObject();

        if (obj)
        {
            return obj->at(name);
        }
        else
        {
            throw Exception("UniData Not an Object");
        }
    }
    
    bool UniData::isNull(std::string name) const
    {
        bool ret = false;

        const ObjectType* obj = ((UniData*)this)->getObject();

        if (obj)
        {
            auto it = obj->find(name);
            if (it != obj->end())
            {
                ret = it->second.isNull();
            }
        }

        return ret;
    }

	UniData& UniData::operator[](const string name)
    {
        UniData* ret = nullptr;

        ObjectType* obj = this->getObject();

        if(obj)
        {
            ret = &(*obj)[name];
        }
        else
        {
            throw Exception("Not Object");
        }

        return *ret;
    }

    void UniData::Add(UniData& value)
    {
        ListType* list =  this->getList();
        list->push_back(value);
    }

    void UniData::WriteDebugObject(StringBuffer& stream, const String& name) const
    {
        auto list = const_cast<UniData*>(this)->getObject();

        for (auto& item : *list)
        {
            UniData& data = item.second;

            data.WriteDebugItem(stream, item.first);
        }
    }

    void UniData::WriteDebugList(StringBuffer& stream, const String& name) const
    {
        int count = 1;
        auto list = const_cast<UniData*>(this)->getList();

        stream << "----<" << name << String::Format("[%d]----\r\n", list->size());

        for(auto& item : *list)
        {
            
           item.WriteDebugItem(stream, name);
    
            stream << "end of " << count++ << "----------------------------------\r\n";
        }
    }

    void UniData::WriteDebugItem(StringBuffer& stream, const String& name) const
    {
        if(this->Type == UniTypes::List)
        {
            this->WriteDebugList(stream, name);
        }
        else if (this->Type == UniTypes::Object)
        {
            this->WriteDebugObject(stream, name);
        }
        else
        {
            stream << name << String::Format("(%d) = ", this->Type) << (String)(*this) << "\r\n";
        }
    }

    String UniData::toDebugString() const
    {
        StringBuffer stream;

        stream << "\r\n";


        switch (this->Type)
        {
        case UniTypes::List:
            this->WriteDebugList(stream, "");
            break;

        case UniTypes::Object:
            this->WriteDebugObject(stream, "");
            break;

        case UniTypes::Null:
            stream << "[NULL]\r\n";
            break;

        default:
            this->WriteDebugItem(stream, "");
        }

        return stream.Str();
    } 


    String UniData::toJSON() const
    {
        StringBuffer stream;

        this->WriteJSONItem(stream);

        return stream.Str();
    }

    void UniData::WriteJSONObject(SoulFab::Base::StringBuffer& stream) const
    {
        bool first = true;
        auto list = const_cast<UniData*>(this)->getObject();

        stream << "{";
        for (auto& item : *list)
        {
            string name = item.first;
            UniData& data = item.second;

            if (!first) stream << ",";

            stream << "\"" << name << "\"" << ":";

            data.WriteJSONItem(stream);

            first = false;
        }
        stream << "}";
    }

    void UniData::WriteJSONList(SoulFab::Base::StringBuffer& stream) const
    {
        bool first = true;
        auto list = const_cast<UniData*>(this)->getList();

        stream << "[";
        for (auto& item : *list)
        {
            if (!first) stream << ",";
            item.WriteJSONItem(stream);
            
            first = false;
        }
        stream << "]";
    }
    
    void UniData::WriteJSONItem(SoulFab::Base::StringBuffer& stream) const
    {
        switch (this->Type)
        {
        case UniTypes::List:
            this->WriteJSONList(stream);
            break;

        case UniTypes::Object:
            this->WriteJSONObject(stream);
            break;

        case UniTypes::String:
            stream << "\"" << (String)(*this) << "\"";
            break;

        case UniTypes::Int:
            stream << (int)(*this);
            break;

        case UniTypes::Float:
            stream << (double)(*this);
            break;

        case UniTypes::Null:
            stream << "null";
            break;
        }
    }
}

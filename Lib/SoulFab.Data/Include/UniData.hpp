#pragma once

#ifndef _SF_COMMON_DATA_H
#define _SF_COMMON_DATA_H

#include <variant>
#include <string>
#include <list>
#include <vector>
#include <memory>
#include <map>

#include "DateTime.hpp"
#include "CommonDef.hpp"
#include "SmartPointer.hpp"
#include "String.hpp"

namespace SoulFab::Data
{

	class UniData;

	union UniValue
	{
		long IntValue;
		double FloatValue;
	};

	class SHARED_EXPORT UniData
	{
		enum class UniTypes
		{
			Null = 0,
			Int,
			Float,
			String,
			List,
			Object,
		};

		typedef std::vector<UniData> ListType;
		typedef std::map<std::string, UniData> ObjectType;
	
	public:
		static UniData Object();
		static UniData List();

	public:
		UniData();
		UniData(const UniData& value);
		UniData(const UniData&& value);

		~UniData();

		//
		bool isNull() const;
		bool isList() const;
		bool isObject() const;
		bool isEmpty() const;

		void setNull();
		int Size();

		//
        UniData& operator =(short value);
        UniData& operator =(int value);
        UniData& operator =(long value);
		UniData& operator =(double value);
		UniData& operator =(const char* value);
		UniData& operator =(const std::string value);
		UniData& operator =(const SoulFab::Base::String value);
		UniData& operator =(const SoulFab::Base::DateTime& value);
        UniData& operator =(const UniData& other);

		operator long() const;
		operator double() const;
		operator SoulFab::Base::String() const;
		operator std::string() const;
		operator SoulFab::Base::DateTime() const;

		ListType* getList();
		ObjectType* getObject();

		void setInt(std::string value);
		void setFloat(std::string value);
		void setDate(std::string value);

		// List
		UniData& operator[](int index);

		void Add(UniData& value);
		UniData& operator <<(long value);
		UniData& operator <<(double value);
		UniData& operator <<(const SoulFab::Base::DateTime& value);
		UniData& operator <<(const SoulFab::Base::String value);

		// Object
		bool has(std::string name) const;
		bool isNull(std::string name) const;
		UniData& operator [](const std::string name);

		int getInt(std::string name) const;
		double getFloat(std::string name) const;
		const SoulFab::Base::String getString(std::string name) const;
		const std::string getRawString(std::string name) const;
		const SoulFab::Base::DateTime getDate(std::string name) const;

        UniData get(const std::string name) const;
        template<class T> inline void set(const std::string name, T value);
		void setInt(const std::string name, std::string value);
		void setFloat(const std::string name, std::string value);
		void setDate(const std::string name, std::string value);
		void setNull(const std::string name);

		//
		std::vector<std::string> getFiledNames() const;
		SoulFab::Base::String toString() const;
        SoulFab::Base::String toJSON() const;
        SoulFab::Base::String toDebugString() const;

	private:	
		UniTypes Type;
		UniValue PrimeValue;
		SoulFab::Base::VariantPointer PointerValue;

		UniData(UniTypes type);
		void Clean();
		void Copy(const UniData& other);

        const UniData& TryGet(std::string name) const;

		void WriteDebugObject(SoulFab::Base::StringBuffer& stream, const SoulFab::Base::String& name) const;
		void WriteDebugList(SoulFab::Base::StringBuffer& stream, const SoulFab::Base::String& name) const;
		void WriteDebugItem(SoulFab::Base::StringBuffer& stream, const SoulFab::Base::String& name) const;

        void WriteJSONObject(SoulFab::Base::StringBuffer& stream) const;
        void WriteJSONList(SoulFab::Base::StringBuffer& stream) const;
        void WriteJSONItem(SoulFab::Base::StringBuffer& stream) const;
    };


	//
	//
	//
    
    inline UniData UniData::get(const std::string name) const
    {
        auto value = this->TryGet(name);

        return value;
    }

	inline int UniData::getInt(const std::string name) const
	{
        auto value = this->TryGet(name);

		return (int)value;
	}

	inline double UniData::getFloat(const std::string name) const
	{
        auto value = this->TryGet(name);

		return (double)value;
	}

	inline const SoulFab::Base::String UniData::getString(std::string name) const
	{
        auto value = this->TryGet(name);

		return (const SoulFab::Base::String)value;
	}

	inline const std::string UniData::getRawString(std::string name) const
	{
        auto value = this->TryGet(name);

		return (const std::string)value;
	}

	inline const SoulFab::Base::DateTime UniData::getDate(std::string name) const
	{
        auto value = this->TryGet(name);

		return SoulFab::Base::DateTime((double)value);
	}

	template<class T>
	inline void UniData::set(const std::string name, const T value)
	{
		UniData& ret = (*this)[name];

		ret = value;
	}

	inline void UniData::setInt(const std::string name, std::string value)
	{
		UniData& ret = (*this)[name];

		ret = atol(value.c_str());
	}

	inline void UniData::setFloat(const std::string name, std::string value)
	{
		UniData& ret = (*this)[name];

		ret = atof(value.c_str());
	}

	inline void UniData::setDate(const std::string name, std::string value)
	{
		UniData& ret = (*this)[name];

		// ret = GLEO::CTSToDateTime(value);
	}

	inline void UniData::setNull(const std::string name)
	{
		UniData& ret = (*this)[name];

		ret.setNull();
	}

}

#endif // _COMMON_DATA_H_

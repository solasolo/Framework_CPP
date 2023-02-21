#pragma once

#ifndef _SF_STRING_HELPER_H_
#define _SF_STRING_HELPER_H_

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#include "CommonDef.hpp"
#include "Exception.hpp"
#include "String.hpp"

namespace SoulFab::Base
{
    class SHARED_EXPORT StringHelper
    {
    public:
        static void toLower(std::string& str);
        static void toUpper(std::string& str);

        static std::vector<std::string> Split(const std::string& str, char c);
        static std::string Trim(const std::string& str);
		static std::string& PadRight(std::string& str, int len, char c = ' ');

        static std::string UTF82GBK(const SoulFab::Base::String& u8_str, const char* buffer = nullptr);
        static SoulFab::Base::String GBK2UTF8(const std::string& gbk_str, const char* buffer = nullptr);

        static SoulFab::Base::String ExtractFilePath(SoulFab::Base::String path);
    };

	template<typename _Elem, typename _Traits, typename _Ax>
	std::basic_string<_Elem, _Traits, _Ax> TrimRight(const std::basic_string<_Elem, _Traits, _Ax>& str)
	{
		std::basic_string<_Elem, _Traits, _Ax> Result = str;

		if (Result.size() > 0)
		{
			int findspace = Result.find_last_not_of(' ');

			if (findspace != (Result.size() - 1))
				Result = Result.substr(0, findspace + 1);
			else if (findspace >= Result.size())
				throw SoulFab::Base::Exception("Length Excced", "String Right Trim");
		}

		return Result;
	}

	template<typename _Elem, typename _Traits, typename _Ax>
	std::basic_string<_Elem, _Traits, _Ax> TrimLeft(const std::basic_string<_Elem, _Traits, _Ax>& str)
	{
		std::basic_string<_Elem, _Traits, _Ax> Result = str;
		int findspace = Result.find_first_not_of(' ');
		if (findspace == -1)
		{
			Result.clear();
		}
		else if (findspace != 0)
			Result = Result.substr(findspace, Result.size() - findspace);
		else if (findspace >= Result.size())
			throw SoulFab::Base::Exception("Length Excced", "String Left Trim");

		return Result;
	}

	inline void StringHelper::toLower(std::string& str)
	{
		// std::transform(str.begin(), str.end(), str.begin(), std::tolower);
		for(auto& c : str)
		{
			c = std::tolower(c);
		}
	}

	inline void StringHelper::toUpper(std::string& str)
	{
		// std::transform(str.begin(), str.end(), str.begin(), std::toupper);
		for(auto& c : str)
		{
			c = std::toupper(c);
		}
	}

}

#endif
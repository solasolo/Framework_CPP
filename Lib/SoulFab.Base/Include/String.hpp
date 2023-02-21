#pragma once

#ifndef _SF_STRING_H_
#define _SF_STRING_H_

#include <vector>
#include <string>
#include "CommonDef.hpp"

namespace SoulFab::Base
{
	const int STRING_FORMAT_BUFFER_SIZE = 4095;

	class SHARED_EXPORT String
	{
	public:
		static String Format(const char* fmt, ...);
		static String Format(const char8_t* fmt, ...);

	public:
		String();
		String(const char* str);
		String(const char8_t* str);
		String(const char* str, int len);
		String(const String& str);
		String(const std::string& str);
		String(String&& str);

		~String();

		String& operator=(const char* str);
		String& operator=(const char8_t* str);
		String& operator=(const String& str);
		String& operator=(const std::string& str);
        String& operator=(String&& str);

		String& operator+=(const char c);
		String& operator+=(const char* str);
		String& operator+=(const char8_t* str);
		String& operator+=(const String& str);


		// String operator+(const char* str);
		// String operator+(const char8_t* str);
		String operator+(const String& str);

		bool operator==(const char* str);
		bool operator==(const char8_t* str);
		bool operator==(const String& str);

		operator std::string() const;

		int Size() const noexcept;
		bool Empty() const noexcept;
		void Clear();
		const char* Pointer() const;

		int IndexOf(char ch, int start = 0) const;
		int LastIndexOf(char ch) const;
		String SubString(int start, int len) const;

		void Trim();
		void Upper();
		void Lower();
		void Capitalize();
		std::vector<String> Split(char delimiter);

		double ToFloat() const;
		long ToInt() const;

	private:
		friend String operator+(const char* str1, String str2);
		friend String operator+(const char8_t* str1, String str2);
		friend String operator+(String str1, const char* str2);
		friend String operator+(String str1, const char8_t* str2);

	private:
		char* Buffer;
		int Length;

		void Create(const char* buf, int len);
		void Asign(const char* buf, int len);
		void Replace(const char* buf, int len);
		bool Compare(const char* buf, int len);
		const char* Concat(const char* buf, int len, int& total);

	private:
		static const char* Concat(const char* buf1, int len1, const char* buf2, int len2, int& total);
	};

	class SHARED_EXPORT StringBuffer
	{
	public:
		StringBuffer();
		~StringBuffer();

		String Str();

	private:
		static const int INCREASE_SIZE;

        friend StringBuffer& operator <<(StringBuffer& sb, const int value);
        friend StringBuffer& operator <<(StringBuffer& sb, const double value);
        friend StringBuffer& operator <<(StringBuffer& sb, const char ch);
		friend StringBuffer& operator <<(StringBuffer& sb, const char* str);
		friend StringBuffer& operator <<(StringBuffer& sb, const SoulFab::Base::String&& str);
		friend StringBuffer& operator <<(StringBuffer& sb, const std::string&& str);

	private:
		char* Buffer;
		int BufferSize;
		int ContentSize;

		void Append(const char* str, int len);
	};


	SHARED_EXPORT String operator+(const char* str1, String str2);
	SHARED_EXPORT String operator+(const char8_t* str1, String str2);
	SHARED_EXPORT String operator+(String str1, const char* str2);
	SHARED_EXPORT String operator+(String str1, const char8_t*  str2);

    SHARED_EXPORT StringBuffer& operator <<(StringBuffer& sb, const int value);
    SHARED_EXPORT StringBuffer& operator <<(StringBuffer& sb, const double value);
    SHARED_EXPORT StringBuffer& operator <<(StringBuffer& sb, const char ch);
	SHARED_EXPORT StringBuffer& operator <<(StringBuffer& sb, const char* str);
	SHARED_EXPORT StringBuffer& operator <<(StringBuffer& sb, const SoulFab::Base::String&& str);
	SHARED_EXPORT StringBuffer& operator <<(StringBuffer& sb, const std::string&& str);
}

#endif

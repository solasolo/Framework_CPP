#include "String.hpp"

#include <stdarg.h>
#include <string.h>

using namespace std;

namespace SoulFab::Base
{
	thread_local char FormatBuffer[STRING_FORMAT_BUFFER_SIZE + 1];

	String String::Format(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		vsnprintf(FormatBuffer, STRING_FORMAT_BUFFER_SIZE, fmt, args);

		return String(FormatBuffer);
	}

	String String::Format(const char8_t* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		vsnprintf(FormatBuffer, STRING_FORMAT_BUFFER_SIZE, reinterpret_cast<const char*>(fmt), args);

		return String(FormatBuffer);
	}

	String operator+(const char* str1, String str2)
	{
		int l;
		const char* p;

		if (str1)
		{
			p = String::Concat(str1, strlen(str1), str2.Buffer, str2.Length, l);
		}
		else
		{
			l = str2.Length;
			p = str2.Buffer;
		}
		
		return String(p, l);
	}

	String operator+(const char8_t* str1, String str2)
	{
		return reinterpret_cast<const char*>(str1) + str2;
	}

	String operator+(String str1, const char* str2)
	{
		int l;
		const char* p;

		if (str2)
		{
			p = String::Concat(str1.Buffer, str1.Length, str2, strlen(str2), l);
		}
		else
		{
			l = str1.Length;
			p = str1.Buffer;
		}

		return String(p, l);
	}

	String operator+(String str1, const char8_t* str2)
	{
		return str1 + reinterpret_cast<const char*>(str2);
	}
	
	//
	//
	//


	String::String()
		: Buffer(nullptr), Length(0)
	{
	}

	String::String(const char* str)
		:  String()
	{
		if(str)
		{
			int len = strlen(str);
			this->Create(str, len);
		}
	}

	String::String(const char* str, int len)
		: String()
	{
		this->Create(str, len);
	}

	String::String(const char8_t* str)
		: String(reinterpret_cast<const char*>(str))
	{
	}

	String::String(const String& str)
		: String()
	{
		int len = str.Length;
		this->Create(str.Buffer, str.Length);
	}

	String::String(const string& str)
		: String()
	{
		int len = str.size();
		this->Create(str.c_str(), len);
	}

	String::String(String&& str)
	{
		this->Buffer = str.Buffer;
		this->Length = str.Length;

        str.Buffer = nullptr;
        str.Length = 0;
	}

	String::~String()
	{
		this->Clear();
	}

	String& String::operator=(const char* str)
	{
		if (str)
		{
			int len = strlen(str);
			this->Asign(str, len);
		}

		return *this;
	}

	String& String::operator=(const char8_t* str)
	{
		*this = reinterpret_cast<const char*>(str);

		return *this;
	}

	String& String::operator=(const String& str)
	{
		int len = str.Length;
		this->Asign(str.Buffer, len);

		return *this;
	}

	String& String::operator=(const string& str)
	{
		int len = str.size();
		this->Asign(str.c_str(), len);

		return *this;
	}

    String& String::operator=(String&& str)
    {
        this->Buffer = str.Buffer;
        this->Length = str.Length;

        str.Buffer = nullptr;
        str.Length = 0;

        return *this;
    }

	String& String::operator+=(const char c)
	{
		int len;
		const char* p = this->Concat(&c, 1, len);
		this->Replace(p, len);

		return *this;
	}

	String& String::operator+=(const char* str)
	{
		if (str)
		{
			int len;
			const char* p = this->Concat(str, strlen(str), len);
			this->Replace(p, len);
		}

		return *this;
	}

	String& String::operator+=(const char8_t* str)
	{
		*this += reinterpret_cast<const char*>(str);

		return *this;
	}

	String& String::operator+=(const String& str)
	{
		int len;
		const char* p = this->Concat(str.Buffer, str.Length, len);
		this->Replace(p, len);

		return *this;
	}

	String String::operator+(const String& str)
	{
		String ret;

		int len;
		const char* p = this->Concat(str.Buffer, str.Length, len);
		ret.Replace(p, len);

		return ret;
	}

	bool String::operator==(const char* str)
	{
		int len = str ? strlen(str) : 0;

		return this->Compare(str, len);
	}

	bool String::operator==(const char8_t* str)
	{
		return (*this == reinterpret_cast<const char*>(str));
	}

	bool String::operator==(const String& str)
	{
		return this->Compare(str.Buffer, str.Length);
	}

	String::operator string() const
	{
		return string(this->Buffer, this->Length);
	}

	int String::Size() const noexcept
	{
		return this->Length;
	}

	bool String::Empty() const noexcept
	{
		return this->Length == 0;
	}

	void String::Clear()
	{
		SafeDestroy(this->Buffer);
		this->Length = 0;
	}

	const char* String::Pointer() const
	{
		return this->Buffer ? this->Buffer : "";
	}

	int String::IndexOf(char ch, int start) const
	{
		int ret = -1;

		const char* p = this->Buffer;

		if (p)
		{
			for (int i = start; i < this->Length; i++)
			{
				if (p[i] == ch)
				{
					ret = i;
					break;
				}
			}
		}

		return ret;
	}

	int String::LastIndexOf(char ch) const
	{
		int ret = -1;

		const char* p = this->Buffer;

		if (p)
		{
			for (int i = this->Length - 1; i >= 0; i--)
			{
				if (p[i] == ch)
				{
					ret = i;
					break;
				}
			}
		}

		return ret;
	}

	String String::SubString(int start, int len) const
	{
		String ret;

		const char* p = this->Buffer;
		if (p)
		{
			ret.Asign(p + start, len);
		}

		return ret;
	}

	void String::Trim()
	{
	}
	
	void String::Upper()
	{
	}
	
	void String::Lower()
	{
	}
	
	void String::Capitalize()
	{
	}
	
	vector<String> String::Split(char delimiter)
	{
		vector<String> ret;
		int last = 0;
		int len = this->Length;

		while (last < len)
		{
			int pos = this->IndexOf(delimiter, last);

			if (pos != last)
			{
				ret.push_back(this->SubString(last, pos - last));
			}
			else
			{
				ret.push_back("");
			}

			if (pos == -1)
			{
				break;
			}

			last = pos + 1;
		}

		return ret;
	}

	double String::ToFloat() const
	{
		return atof(this->Buffer);
	}
	
	long String::ToInt() const
	{
		return atol(this->Buffer);
	}

	void String::Create(const char* buf, int len)
	{
		if (len > 0)
		{
			this->Buffer = new char[len + 1];
			memcpy(this->Buffer, buf, len);
			this->Buffer[len] = '\x0';

			this->Length = len;
		}
	}

	void String::Asign(const char* buf, int len)
	{
		this->Clear();
		this->Create(buf, len);
	}

	void String::Replace(const char* buf, int len)
	{
		this->Clear();
		this->Buffer = const_cast<char*>(buf);
		this->Length = len;
	}

	bool String::Compare(const char* buf, int len)
	{
		bool ret = (this->Length == len);
		const char* p = this->Buffer;

		if (ret && p && buf)
		{
			for (int i = 0; i < len; i++)
			{
				if (p[i] != buf[i])
				{
					ret = false;
					break;
				}
			}
		}

		return ret;
	}

	const char* String::Concat(const char* buf, int len, int& total)
	{
		return String::Concat(this->Buffer, this->Length, buf, len, total);
	}

	const char* String::Concat(const char* buf1, int len1, const char* buf2, int len2, int& total)
	{
		char* ret = nullptr;
		total = 0;

		int size = len1 + len2;
		if (size > 0)
		{
			ret = new char[size + 1];

			if (buf1 && len1 > 0)
			{
				memcpy(ret, buf1, len1);
				total = len1;
			}

			if (buf2 && len2 > 0)
			{
				memcpy(ret + total, buf2, len2);
				total += len2;
			}

			ret[total] = '\x0';
		}

		return ret;
	}


#pragma region StringBuffer
	const int StringBuffer::INCREASE_SIZE = 4096;

    StringBuffer& operator <<(StringBuffer& sb, const int value)
    {
        sb << to_string(value);

        return sb;
    }

    StringBuffer& operator <<(StringBuffer& sb, const double value)
    {
        sb << to_string(value);

        return sb;
    }

	StringBuffer& operator <<(StringBuffer& sb, const char ch)
	{
		sb.Append(&ch, 1);

		return sb;
	}

	StringBuffer& operator <<(StringBuffer& sb, const char* str)
	{
		if (str)
		{
			sb.Append(str, strlen(str));
		}

		return sb;
	}

	StringBuffer& operator <<(StringBuffer& sb, const SoulFab::Base::String&& str)
	{
		sb.Append(str.Pointer(), str.Size());

		return sb;
	}

	StringBuffer& operator <<(StringBuffer& sb, const std::string&& str)
	{
		sb.Append(str.c_str(), str.size());

		return sb;
	}

	StringBuffer::StringBuffer()
		: Buffer(nullptr), BufferSize(0), ContentSize(0)
	{
	}

	StringBuffer::~StringBuffer()
	{
		SafeDestroy(this->Buffer);
		this->BufferSize = 0;
		this->ContentSize = 0;
	}

	String StringBuffer::Str()
	{
		return String(this->Buffer, this->ContentSize);
	}

	void StringBuffer::Append(const char* str, int len)
	{

		int NewSize = this->ContentSize + len;

		if (NewSize > this->BufferSize)
		{
			NewSize = (NewSize / INCREASE_SIZE + 1) * INCREASE_SIZE;
			char* buf = new char[NewSize + 1];
			this->BufferSize = NewSize;

			if (this->Buffer)
			{
				memcpy(buf, this->Buffer, this->ContentSize);
			}

			SafeDestroy(this->Buffer);
			this->Buffer = buf;

			this->Buffer[this->ContentSize] = '\x0';
		}
		
		if(str)
		{
			memcpy(this->Buffer + this->ContentSize, str, len);
			this->ContentSize += len;
		}
	}

#pragma endregion

}

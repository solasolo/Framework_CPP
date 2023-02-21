#include "StringHelper.hpp"

#include <algorithm>

#include "CommonDef.hpp"
#include "UnicodeGBK.def"
#include "GBKUnicode.def"
#include "Exception.hpp"

using namespace std;

namespace SoulFab::Base
{
	int U8CharLen(unsigned char first_char)
	{
		int num = 0;
		unsigned char mask = 0x80;

		while (mask & first_char)
		{
			num++;
			mask = (mask >> 1);
		}

		return num;
	}

	void U8Char2Unicode(int len, const unsigned char* pu8, unsigned char puni[2])
	{
		switch (len)
		{
		case 2:
			puni[0] = (pu8[1] >> 2) & 0x07;
			puni[1] = (pu8[0] << 6) | (pu8[1] & 0x3f);
			break;

		case 3:
			puni[0] = (pu8[1] << 6) | (pu8[2] & 0x3f);
			puni[1] = (pu8[0] << 4) | ((pu8[1] >> 2) & 0x0f);
			break;

		case 4:

			break;
		case 5:

			break;
		case 6:

			break;

		default:
			break;
		}
	}

	bool Unicode2GBK(unsigned char u[2], unsigned char* pg)
	{
		bool ret = false;

		unsigned short UWord = *(unsigned short*)u;

		short index = (UWord & 0xff00) >> 8;
		int start = UnicodeGBKIndex[index][1];
		int end = UnicodeGBKIndex[index][2];

		if (end > 0)
		{
			while (start <= end)
			{
				int mid = (start + end) / 2;
				unsigned short AWord = UnicodeGBKMap[mid][0];

				if (AWord < UWord)
				{
					start = mid + 1;
				}
				else if (AWord > UWord)
				{
					end = mid - 1;
				}
				else
				{
					unsigned short g = UnicodeGBKMap[mid][1];
					*(unsigned short*)pg = g;

					ret = true;
					break;
				}
			}
		}

		return ret;
	}

	vector<string> StringHelper::Split(const string& str, char delimiter)
	{
		vector<string> ret;
		size_t last = 0;
		size_t len = str.size();

		while (last < len) 
		{
			auto pos = str.find_first_of(delimiter, last);
			
			if (pos != last)
			{
				ret.push_back(str.substr(last, pos - last));
			}
			else
			{
				ret.push_back("");
			}

			if (pos == string::npos)
			{
				break;
			}

			last = pos + 1;
		}

		return ret;
	}

	string StringHelper::Trim(const string& str)
	{
		string ret;

		ret = TrimRight(str);
		ret = TrimLeft(ret);

		return ret;
	}
	
	string& StringHelper::PadRight(string& str, int len, char c)
	{
		int size = str.size();

		if (size < len)
			str.resize(len, c);
		else if (size > len)
			throw Exception("Length Excced", "String Pad Right");

		return str;
	}

	string StringHelper::UTF82GBK(const String& u8_str, const char* buffer)
	{
		string ret;
		unsigned char tmp[2];

		int len = u8_str.Size();
		const unsigned char* p = (unsigned char*)u8_str.Pointer();
		const unsigned char* end = p + len;

		unsigned char* GBKBuf = buffer ? (unsigned char*)buffer : new unsigned char[len];
		unsigned char* pg = GBKBuf;

		int i = 0;
		while (i < len)
		{
			const unsigned char* pu = p + i;
			int size = U8CharLen(*pu);
			if ((i + size) > len) break;

			switch (size)
			{
			case 0:
				*pg = *pu;

				pg += 1;
				i += 1;
				break;

			case 2:
			case 3:
				U8Char2Unicode(size, pu, tmp);

				if (tmp[1] == 0)
				{
					*pg = tmp[0];
					pg += 1;
				}
				else
				{
					if(Unicode2GBK(tmp, pg)) pg += 2;
				}

				i += size;
				break;

			default:
				i += size;
				break;
			}

		}

		ret.append((char*)GBKBuf, (pg - GBKBuf));
		
		if (!buffer)
		{
			delete[] GBKBuf;
		}

		return ret;
	}

	String StringHelper::GBK2UTF8(const string& gbk_str, const char* buffer)
	{
		unsigned char tmp[2];

		int len = gbk_str.length();
		const unsigned char* p = (unsigned char*)gbk_str.c_str();
		const unsigned char* end = p + len;

		unsigned char* UTF8Buf = buffer ? (unsigned char*)buffer : new unsigned char[len / 2 * 3 + 1];
		unsigned char* pu = UTF8Buf;

		int i = 0;
		while (i < len)
		{
			const unsigned char* pg = p + i;

			if (*pg & 0x80)
			{
				int h = pg[0];
				int l = pg[1];

				if (h > 0x80 && h < 0xff && l > 0x3f && l < 0xff)
				{
					int n = (l - 0x40) + ((h - 0x81) * 0xbf);
					
					unsigned short ucode = GBKUnicodeMap[n];
					if (ucode != 0)
					{

						pu[0] = 0xe0 | (ucode >> 12);
						pu[1] = 0x80 | ((ucode >> 6) & 0x3f);
						pu[2] = 0x80 | (ucode & 0x3f);

						pu += 3;
					}
				}

				i += 2;
			}
			else
			{
				*pu = *pg;

				pu += 1;
				i += 1;
			}
		}

		String ret((char*)UTF8Buf, (pu - UTF8Buf));

		if (!buffer)
		{
			delete[] UTF8Buf;
		}

		return ret;

	}

	String StringHelper::ExtractFilePath(String path)
	{
		String Result;

		size_t p = path.LastIndexOf('\\');
		if (p == -1)
		{
			Result = ".";
		}
		else
		{
			Result = path.SubString(0, p);
		}

		return Result;
	}
}

#include "TelegramBuilder.hpp"

#include "Exception.hpp"
#include "StringHelper.hpp" 

#include <sstream>
#include <stdio.h>
#include <string>

using namespace SoulFab::Base;
using namespace SoulFab::Data;
using namespace std;

namespace SoulFab::Link
{

	BaseTelegramItemError::BaseTelegramItemError(const string& item, bool decode)
		: Exception("", decode ? "Decode" : "Encode")
	{
		this->ItemName = item;
	}

	BaseTelegramItemError& BaseTelegramItemError::operator =(const BaseTelegramItemError& ex) throw()
	{
		this->ItemName = ex.ItemName;
		
		return *this;
	}

	TelegramItemSizeError::TelegramItemSizeError(const string& item, bool decode)
		: BaseTelegramItemError(item, decode)
	{
	}

	const String TelegramItemSizeError::GetErrorDescription() const noexcept
	{
		return String("[") + this->ItemName + "] Error Field Size";
	}

	TelegramItemFormatError::TelegramItemFormatError(const string& item, bool decode)
		: BaseTelegramItemError(item, decode)
	{
	}

	const String TelegramItemFormatError::GetErrorDescription() const noexcept
	{
		return string("[") + this->ItemName + "] Error Format";
	}

	TelegramItemNullError::TelegramItemNullError(const string& item, bool decode)
		: BaseTelegramItemError(item, decode)
	{
	}
	const String TelegramItemNullError::GetErrorDescription() const noexcept
	{
		return String("[") + this->ItemName + "] Null Field";
	}

	//
	//
	//

	const int BaseTelegramBuilder::MAX_FIELD_LENGTH = 255;

	BaseTelegramBuilder::BaseTelegramBuilder(const String& file_name)
		: Scheme(nullptr)
	{
		Scheme = new TelegramScheme(file_name);
	}

	BaseTelegramBuilder::~BaseTelegramBuilder()
	{
		SafeDestroy(Scheme);
	}

	//
	// Class TextTelegramBuilder
	//

	TextTelegramBuilder::TextTelegramBuilder(const String& file_name)
		: BaseTelegramBuilder(file_name)
	{
		this->BlankChar = ' ';
		this->IsFix = this->Scheme->IsFix;
	}

	TextTelegramBuilder::~TextTelegramBuilder()
	{
	}

	bool TextTelegramBuilder::IsBlank(const string& part)
	{
		bool Result = true;

		int l = part.size();
		for (int i = 0; i < l; i++)
		{
			if (part[i] != this->BlankChar)
			{
				Result = false;
				break;
			}
		}

		return Result;
	}

	void TextTelegramBuilder::FormatPart(string& part, const UniData& data, const FieldDef& item)
	{
		char Temp[MAX_FIELD_LENGTH + 1];
		char format[20];
		const char* format2;

		Temp[MAX_FIELD_LENGTH] = '\x0';
		format[0] = format[19] = '\x0';

		part.clear();
		FieldType Type = item.Type;
		int Length = item.Length;

		double FloatValue;
		String StringValue;
		int InputLength;

		Temp[0] = '\x0';
		switch (Type)
		{
		case FieldType::Int:
			sprintf_s(format, 19, "%%0%dd", Length);
			sprintf_s(Temp, MAX_FIELD_LENGTH, format, (long)data);
			break;

		case FieldType::String:
			StringValue = static_cast<String>(data);
			InputLength = StringValue.Size();

			if (Length < MAX_FIELD_LENGTH)
			{
				format2 = (this->Scheme->PaddingLeft) ? "%%-%ds" : "%%%ds";
				sprintf_s(format, 19, format2, Length);

				sprintf_s(Temp, MAX_FIELD_LENGTH, format, StringValue.Pointer());
			}
			else
			{
				if (InputLength < Length)
				{
					//part = "";
					//part.resize(Length = InputLength, ' ');
					//part += StringValue;

					part = StringValue;
					StringHelper::PadRight(part, Length);
				}
				else
				{
					part = StringValue;
				}
			}
			break;

		case FieldType::Float:
			FloatValue = (double)data;

			if (this->IsFix)
			{
				for (int d = 0; d < item.Dec; d++)
				{
					FloatValue *= 10;
				}

				sprintf_s(format, 19, "%%0%dd", Length);
				sprintf_s(Temp, MAX_FIELD_LENGTH, format, int(FloatValue));
			}
			else
			{
				sprintf_s(format, 19, "%%0%d.%df", Length, item.Dec);
				sprintf_s(Temp, MAX_FIELD_LENGTH, format, FloatValue);
			}

			break;

		case FieldType::DateTime:
			part = ((DateTime)data).CTS();
			break;

		case FieldType::Repeat:
			break;
		}

		if (part.empty())
		{
			int l = strlen(Temp);
			part.resize(l, '\x0');
			strncpy((char*)part.c_str(), Temp, l);
		}

		if (part.length() != Length)
		{
			throw TelegramItemSizeError(item.Name, false);
		}
	}

	void TextTelegramBuilder::ExtractPart(string& part, const FieldDef& item, UniData& values)
	{
		UniData data;
		char format[20];
		format[19] = '\x0';

		int i;
		long IntData;
		double FloatData;
		FieldType Type = item.Type;
		short Length = item.Length;

		if (IsBlank(part))
		{
			values[item.Name].setNull();
		}
		else
		{
			switch (Type)
			{
			case FieldType::Int:
				//		data = (long)atoi(part.c_str());

				sprintf_s(format, 19, "%%%dd", Length);
				if (sscanf_s(part.c_str(), format, &IntData) == 1)
				{
					data = IntData;
					values.set(item.Name, data);
				}
				else
				{
					throw TelegramItemFormatError(item.Name, true);
				}
				break;

			case FieldType::Float:
				for (i = 0; i < part.size(); i++)
				{
					if (part[i] == ' ') part[i] = '0';
				}

				if (!this->IsFix)
				{
					sprintf_s(format, 19, "%%%dlf", Length);
					if (sscanf_s(part.c_str(), format, &FloatData) == 1)
					{
						data = FloatData;
					}
					else
					{
						throw TelegramItemFormatError(item.Name, true);
					}
				}
				else
				{
					sprintf_s(format, 19, "%%%dd", Length);
					if (sscanf_s(part.c_str(), format, &IntData) == 1)
					{
						FloatData = (double)IntData;
					}
					else
					{
						throw TelegramItemFormatError(item.Name, true);
					}

					for (int d = 0; d < item.Dec; d++)
					{
						FloatData /= 10;
					}

					data = FloatData;
				}

				values.set(item.Name, data);
				break;

			case FieldType::String:
				part = StringHelper::Trim(part);

				data = part;
				values.set(item.Name, data);
				break;
			}
		}
	}

	void TextTelegramBuilder::EmptyPart(string& part, const FieldDef& item)
	{
		char Temp[MAX_FIELD_LENGTH + 1];
		char format[20];

		Temp[MAX_FIELD_LENGTH] = '\x0';
		format[19] = '\x0';

		FieldType Type = item.Type;
		int Length = item.Length;

		switch (Type)
		{
		case FieldType::Int:
			sprintf_s(format, 19, "%%0%dd", Length);
			sprintf_s(Temp, MAX_FIELD_LENGTH, format, 0);
			break;

		case FieldType::String:
			sprintf_s(format, 19, "%%%ds", Length);
			sprintf_s(Temp, MAX_FIELD_LENGTH, format, " ");
			break;

		case FieldType::Float:
			sprintf_s(format, 19, "%%0%d.%df", Length, item.Dec);
			sprintf_s(Temp, MAX_FIELD_LENGTH, format, 0);
			break;
		}

		if (strlen(Temp) != Length)
		{
			throw TelegramItemSizeError(item.Name, false);
		}

		part = "";
		part.resize(Length, 0x0);
		strncpy((char*)part.c_str(), Temp, Length);
	}

	void TextTelegramBuilder::EncodeItem(ostringstream& buffer, const FieldDef& item, const UniData& values, const string& tel_name)
	{
		string part;
		unsigned short Length = item.Length;
		string FieldName = item.Name;

		if (item.Type != FieldType::Repeat)
		{
			const UniData& Value = const_cast<UniData&>(values)[FieldName];

			FormatPart(part, Value, item);
		}
		else
		{
			auto& fds = Scheme->GetRepatParts(tel_name, item.Name);
			auto V = const_cast<UniData&>(values)[FieldName].getList();

			for(auto &row : *V)
			{
				for (auto &fd : fds)
				{
					EncodeItem(buffer, fd, row, tel_name);
				}
			}

			//补齐不足的部分
			int difference = (int)Length - V->size();
			string temp;

			for (; difference > 0; difference--)
			{
				vector<FieldDef>::iterator itr = fds.begin();
				while (itr != fds.end())
				{
					EmptyPart(temp, *itr);
					buffer << temp;
					itr++;
				}
			}
		}

		buffer << part;
	}

	void TextTelegramBuilder::DecodeItem(const string& telegram, unsigned int& pos, const FieldDef& item, UniData& values, const string& tel_name)
	{
		string part;
		UniData data;
		string FieldName = item.Name;
		unsigned short Length = item.Length;

		if (item.Type != FieldType::Repeat)
		{
			if (telegram.length() >= pos + Length)
			{
				string part = telegram.substr(pos, Length);

				ExtractPart(part, item, values);

				pos += Length;
			}
			else
			{
				throw TelegramItemSizeError(item.Name, true);
			}
		}
		else
		{
			UniData&& T = UniData::List();

			vector<FieldDef>& fds = Scheme->GetRepatParts(tel_name, item.Name);

			vector<FieldDef>::iterator itr = fds.begin();
			int RepeatTimes = item.Length;
			if (RepeatTimes == 0 && item.RepeatCountField != "")
			{
				RepeatTimes = values.getInt(item.RepeatCountField);
			}

			for (int K = 0; K < RepeatTimes; K++)
			{
				UniData Temp;

				for (auto &fd : fds)
				{
					DecodeItem(telegram, pos, fd, Temp, tel_name);
				}

				T.Add(Temp);
			}

			values.set(FieldName, T);
		}
	}

	//对象-->电文
	void TextTelegramBuilder::Encode(const string& tel_name, string& telegram, const UniData& values)
	{
		ostringstream buffer;

		TelegramDef& fds = Scheme->GetTelgram(tel_name);
		vector<FieldDef>::iterator FieldIterator = fds.FieldDefs.begin();
		while (FieldIterator != fds.FieldDefs.end())
		{
			EncodeItem(buffer, *FieldIterator, values, tel_name);

			FieldIterator++;
		}

		telegram = buffer.str();
	}

	//电文->对象
	void TextTelegramBuilder::Decode(const string& tel_name, const string& telegram, UniData& values)
	{
		TelegramDef& fds = Scheme->GetTelgram(tel_name);
		vector<FieldDef>::iterator FieldIterator = fds.FieldDefs.begin();

		if (!fds.VariableLength && (fds.Length != telegram.size()))
		{
			char s[100];
			sprintf(s, "Telegram size Not Match: %d/%d", fds.Length, telegram.size());
			throw Exception(s, "Telegram Decode");
		}

		unsigned int Position = 0;//位置初始化

		while (FieldIterator != fds.FieldDefs.end())
		{
			DecodeItem(telegram, Position, *FieldIterator, values, tel_name);

			FieldIterator++;
		}

		if (Position != telegram.size())
		{
			char s[100];
			sprintf(s, "Telegram size Error:%d/%d", Position, (int)telegram.size());
			throw Exception(s, "Telegram Decode");
		}
	}

	//
	// Class DelimiterTelegramBuilder
	//

	DelimiterTelegramBuilder::DelimiterTelegramBuilder(string file_name, string delimiter)
		: TextTelegramBuilder(file_name)
	{
		this->Delimiter = delimiter;
	}

	DelimiterTelegramBuilder::~DelimiterTelegramBuilder()
	{
	}

	void DelimiterTelegramBuilder::FormatPart(string& part, const UniData& data, const FieldDef& item)
	{
		if (item.Type != FieldType::String)
		{
			TextTelegramBuilder::FormatPart(part, data, item);
		}
		else
		{
			part.clear();
			part = (string)data;
			part = StringHelper::Trim(part);

			while (part.length() < item.Length)
			{
				part += '~';
			}

			if (part.length() > item.Length) part.resize(item.Length);
		}
	}

	void DelimiterTelegramBuilder::ExtractPart(string& part, const FieldDef& item, UniData& values)
	{
		TextTelegramBuilder::ExtractPart(part, item, values);
	}

	void DelimiterTelegramBuilder::EncodeItem(ostringstream& stream, const FieldDef& item, const UniData& values, const string& tel_name)
	{
		TextTelegramBuilder::EncodeItem(stream, item, values, tel_name);

		if (item.Type != FieldType::Repeat)
		{
			stream << this->Delimiter;
		}
	}

	void DelimiterTelegramBuilder::DecodeItem(const string& telegram, unsigned int& pos, const FieldDef& item, UniData& values, const string& tel_name)
	{
		string part;
		UniData data;
		string FieldName = item.Name;
		unsigned short Length = item.Length;

		if (item.Type != FieldType::Repeat)
		{
			if (telegram.length() >= pos + Length)
			{
				int p = telegram.find(this->Delimiter, pos);
				if (p != string::npos)
				{
					part = telegram.substr(pos, p - pos);

					for (int i = 0; i < part.size(); i++)
					{
						if (part[i] == '~')
						{
							part[i] = ' ';
						}
					}
				}

				ExtractPart(part, item, values);

				pos = p + 1;
			}
			else
			{
				throw TelegramItemSizeError(item.Name, true);
			}
		}
		else
		{
			UniData&& T = UniData::List();

			vector<FieldDef>& fds = Scheme->GetRepatParts(tel_name, item.Name);
			vector<FieldDef>::iterator itr = fds.begin();
			int RepeatTimes = item.Length;
			if (RepeatTimes == 0 && item.RepeatCountField != "")
			{
				RepeatTimes = values.getInt(item.RepeatCountField);
			}

			for (int K = 0; K < RepeatTimes; K++)
			{
				UniData Temp;

				itr = fds.begin();
				while (itr != fds.end())
				{
					DecodeItem(telegram, pos, *itr, Temp, tel_name);
					itr++;
				}

				T.Add(Temp);
			}

			values.set(FieldName, T);
		}
	}

	//
	// Class BinaryTelegramCodec
	//

#pragma region 

	BinaryTelegramCodec::BinaryTelegramCodec(String file_name)
		: BaseTelegramBuilder(file_name)
	{
		IsBig = this->Scheme->IsBig;
	}

	BinaryTelegramCodec::~BinaryTelegramCodec()
	{
	}

	//电文->对象
	void BinaryTelegramCodec::Decode(const short& cmd, const string& telegram, UniData& values)
	{
		char tel_name[20];
		_itoa_s(cmd, tel_name, 10);

        TelegramDef& fds = Scheme->GetTelgram(tel_name);
        vector<FieldDef>::iterator FieldIterator = fds.FieldDefs.begin();

        BinaryDecoder decoder(telegram);
        while (FieldIterator != fds.FieldDefs.end())
        {
            DecodeItem(decoder, *FieldIterator, values, tel_name);

            FieldIterator++;
        }
	}

	//对象-->电文
	void BinaryTelegramCodec::Encode(const short& cmd, string& telegram, const UniData& values)
	{
		char tel_name[20];
		_itoa_s(cmd, tel_name, 10);

        BinaryEncoder buffer;

        TelegramDef& fds = Scheme->GetTelgram(tel_name);
        vector<FieldDef>::iterator FieldIterator = fds.FieldDefs.begin();
        while (FieldIterator != fds.FieldDefs.end())
        {
            EncodeItem(buffer, *FieldIterator, values, tel_name);

            FieldIterator++;
        }

        telegram = buffer.GetData();
	}

	void BinaryTelegramCodec::DecodeItem(BinaryDecoder& decoder, const FieldDef& item, UniData& values, const string& tel_name)
	{
		if (item.Type != FieldType::Repeat)
		{
			if (!IsBig)
			{
				ExtractPart(decoder, item, values);
			}
			else
			{
				ExtractPartReverse(decoder, item, values);
			}
		}
		else
		{
			UniData&& T = UniData::List();

			int repeatLen = 0;
			vector<FieldDef>& fds = Scheme->GetRepatParts(tel_name, item.Name);
			vector<FieldDef>::iterator itr = fds.begin();

			for (int K = 0; K < item.Length; K++)
			{
				UniData Temp = UniData::Object();
				itr = fds.begin();
				while (itr != fds.end())
				{
					DecodeItem(decoder, *itr, Temp, tel_name);
					itr++;
				}

				T.Add(Temp);
			}

			values.set(item.Name, T);
		}
	}

	void BinaryTelegramCodec::ExtractPart(BinaryDecoder& decoder, const FieldDef& item, UniData& values)
	{
		FieldType Type = item.Type;

		switch (Type)
		{
		case FieldType::Int:
			if (item.Length == 2)
			{
				short temp;
				decoder.Read(temp);
				values.set(item.Name, (long)temp);
			}
			else
			{
				int value;
				decoder.Read(value);
				values.set(item.Name, (long)value);
			}
			break;

		case FieldType::Float:
			float temp;
			decoder.Read(temp);
			values.set(item.Name, temp);
			break;

		case FieldType::String:
			string str;

			if (item.Length > 0)
			{
				decoder.Read(str, item.Length);
			}
			else
			{
				decoder.Read(str);
			}
			values.set(item.Name, str);
			break;
		}
	}

	void BinaryTelegramCodec::EncodeItem(BinaryEncoder& buffer, const FieldDef& item, const UniData& values, const string& tel_name)
	{
		unsigned short Length = item.Length;
		string FieldName = item.Name;

		if (item.Type != FieldType::Repeat)
		{
			const UniData& Value = const_cast<UniData&>(values)[FieldName];
			if (Value.isNull()) throw TelegramItemNullError(FieldName, false);
			

			if (!IsBig)
			{
				FormatPart(buffer, Value, item);
			}
			else
			{
				FormatPartReverse(buffer, Value, item);
			}
		}
		else
		{
			auto& fds = Scheme->GetRepatParts(tel_name, item.Name);
			auto itr = fds.begin();
			auto V = const_cast<UniData&>(values)[FieldName].getList();
			
			for(auto &row : *V)
			{
				for (auto &fd : fds)
				{
					EncodeItem(buffer, fd, row, tel_name);
				}
			}
		}
	}

	void BinaryTelegramCodec::FormatPart(BinaryEncoder& buffer, const UniData& data, const FieldDef& item)
	{
		char Temp[1000];
		char format[20];
		int Length = item.Length;
		FieldType Type = item.Type;

		switch (Type)
		{
		case FieldType::Int:
			if (item.Length == 2)
			{
				short t = (long)data;
				buffer.Write(t);
			}
			else
			{
				buffer.Write((int)(long)data);
			}

			break;

		case FieldType::String:
			if (item.Length > 0)
			{
				buffer.Write((string)data, item.Length);
			}
			else
			{
				buffer.Write((string)data);
			}
			break;

		case FieldType::Float:
			buffer.Write((float)(double)data);
			break;
		}
	}

	void BinaryTelegramCodec::FormatPartReverse(BinaryEncoder& buffer, const UniData& data, const FieldDef& item)
	{
		char Temp[1000];
		char format[20];
		int Length = item.Length;
		FieldType Type = item.Type;

		switch (Type)
		{
		case FieldType::Int:
			if (item.Length == 2)
			{
				buffer.WriteReverse((short)(long)data);
			}
			else
			{
				buffer.WriteReverse((int)(long)data);
			}

			break;

		case FieldType::String:
			if (item.Length > 0)
			{
				buffer.Write((string)data, item.Length);
			}
			else
			{
				buffer.Write((string)data);
			}
			break;

		case FieldType::Float:
			buffer.WriteReverse((float)(double)data);
			break;
		}
	}

	void BinaryTelegramCodec::ExtractPartReverse(BinaryDecoder& decoder, const FieldDef& item, UniData& values)
	{
		FieldType Type = item.Type;

		switch (Type)
		{
		case FieldType::Int:
			if (item.Length == 2)
			{
				short value;
				decoder.ReadReverse(value);
				values.set(item.Name, (long)value);
			}
			else
			{
				int value;
				decoder.ReadReverse(value);
				values.set(item.Name, value);
			}

			break;

		case FieldType::Float:
			float temp;
			decoder.ReadReverse(temp);
			values.set(item.Name, temp);
			break;

		case FieldType::String:
			string str;
			if (item.Length > 0)
			{
				decoder.Read(str, item.Length);
			}
			else
			{
				decoder.Read(str);
			}

			values.set(item.Name, str);
			break;
		}
	}

#pragma endregion Class BinaryTelegramCodec
}

#pragma once

#ifndef TELEGRAMBUILDER_H
#define TELEGRAMBUILDER_H

#include "TelegramScheme.hpp"
#include "UniData.hpp"
#include "BaseCodec.hpp"

namespace SoulFab::Link
{
#pragma region Exceptions

	class BaseTelegramItemError : public SoulFab::Base::Exception
	{
	protected:
		std::string ItemName;

	public:
		BaseTelegramItemError(const std::string& item, bool decode);

		BaseTelegramItemError& operator =(const BaseTelegramItemError& ex) noexcept;
	};

	class TelegramItemSizeError : public BaseTelegramItemError
	{
	protected:
		virtual const SoulFab::Base::String GetErrorDescription() const noexcept;

	public:
		TelegramItemSizeError(const std::string& item, bool decode);
	};

	class TelegramItemNullError : public BaseTelegramItemError
	{
	protected:
		virtual const SoulFab::Base::String GetErrorDescription() const noexcept;

	public:
		TelegramItemNullError(const std::string& item, bool decode);
	};

	class TelegramItemFormatError : public BaseTelegramItemError
	{
	protected:
		virtual const SoulFab::Base::String GetErrorDescription() const noexcept;

	public:
		TelegramItemFormatError(const std::string& item, bool decode);
	};

#pragma endregion

	class SHARED_EXPORT BaseTelegramBuilder
	{
	protected:
		static const int MAX_FIELD_LENGTH;

		TelegramScheme* Scheme;
	
	public:
		BaseTelegramBuilder(const SoulFab::Base::String& file_name);
		virtual ~BaseTelegramBuilder();
	};

	class SHARED_EXPORT TextTelegramBuilder : public BaseTelegramBuilder, public IMessageCodec<std::string>
	{
	private:
		bool IsFix;
		char BlankChar;

	protected:
		void EmptyPart(std::string& part, const SoulFab::Data::FieldDef& item);
		bool IsBlank(const std::string& part);

		virtual void EncodeItem(std::ostringstream& stream, const SoulFab::Data::FieldDef& item, const SoulFab::Data::UniData& values, const std::string& tel_name);
		virtual void DecodeItem(const std::string& telegram, unsigned int& pos, const SoulFab::Data::FieldDef& item, SoulFab::Data::UniData& values, const std::string& tel_name);
		virtual void FormatPart(std::string& part, const SoulFab::Data::UniData& data, const SoulFab::Data::FieldDef& item);
		virtual void ExtractPart(std::string& part, const SoulFab::Data::FieldDef& item, SoulFab::Data::UniData& values);

	public:
		TextTelegramBuilder(const SoulFab::Base::String& file_name);
		virtual ~TextTelegramBuilder();

		void Encode(const std::string& cmd, std::string& telegram, const SoulFab::Data::UniData& values) override;
		void Decode(const std::string& cmd, const std::string& telegram, SoulFab::Data::UniData& values) override;
	};

	class SHARED_EXPORT DelimiterTelegramBuilder : public TextTelegramBuilder
	{
	private:
		std::string Delimiter;

	protected:
		virtual void EncodeItem(std::ostringstream& stream, const SoulFab::Data::FieldDef& item, const SoulFab::Data::UniData& values, const std::string& tel_name) override;
		virtual void DecodeItem(const std::string& telegram, unsigned int& pos, const SoulFab::Data::FieldDef& item, SoulFab::Data::UniData& values, const std::string& tel_name) override;
		virtual void FormatPart(std::string& part, const SoulFab::Data::UniData& data, const SoulFab::Data::FieldDef& item) override;
		virtual void ExtractPart(std::string& part, const SoulFab::Data::FieldDef& item, SoulFab::Data::UniData& values) override;

	public:
		DelimiterTelegramBuilder(std::string file_name, std::string delimiter = ";");
		virtual ~DelimiterTelegramBuilder();
	};

	class SHARED_EXPORT BinaryTelegramCodec : public BaseTelegramBuilder, public IMessageCodec<short>
	{
	private:
		bool IsBig;

		void EncodeItem(BinaryEncoder& encoder, const SoulFab::Data::FieldDef& item, const SoulFab::Data::UniData& values, const std::string& tel_name);
		void DecodeItem(BinaryDecoder& decoder, const SoulFab::Data::FieldDef& item, SoulFab::Data::UniData& values, const std::string& tel_name);

	protected:
		virtual void FormatPart(BinaryEncoder& encoder, const SoulFab::Data::UniData& data, const SoulFab::Data::FieldDef& item);
		virtual void ExtractPart(BinaryDecoder& decoder, const SoulFab::Data::FieldDef& item, SoulFab::Data::UniData& values);
		virtual void FormatPartReverse(BinaryEncoder& encoder, const SoulFab::Data::UniData& data, const SoulFab::Data::FieldDef& item);
		virtual void ExtractPartReverse(BinaryDecoder& decoder, const SoulFab::Data::FieldDef& item, SoulFab::Data::UniData& values);

	public:
        BinaryTelegramCodec(SoulFab::Base::String file_name);
		virtual ~BinaryTelegramCodec();

		void Encode(const short& cmd, std::string& telegram, const SoulFab::Data::UniData& values) override;
		void Decode(const short& cmd, const std::string& telegram, SoulFab::Data::UniData& values) override;
	};


}

#endif

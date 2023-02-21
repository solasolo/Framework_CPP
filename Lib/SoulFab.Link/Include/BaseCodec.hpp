#pragma once

#ifndef _SF_BASE_CODEC_H_
#define _SF_BASE_CODEC_H_

#include <vector>

#include "CommonDef.hpp"
#include "UniData.hpp"
#include "StreamBuffer.hpp"

namespace SoulFab::Link
{
    struct FrameDelimiter
    {
        static const char STX = '\x02';
        static const char ETX = '\x03';
    };

    template<typename T>
    class IFrameCodec
    {
    public:
        virtual bool Decode(StreamBuffer& stream, T& cmd, std::string& block) = 0;
        virtual void Encode(std::string& message, T cmd, const std::string& block) = 0;
    };

    template<typename T>
    class IMessageCodec
    {
    public:
        virtual void Decode(const T& cmd, const std::string& message, SoulFab::Data::UniData& data) = 0;
        virtual void Encode(const T& cmd, std::string& message, const SoulFab::Data::UniData& data) = 0;
    };

    class SHARED_EXPORT BinaryDecoder
    {
    protected:
        int Len;
        int Pos;
        const char* Buffer;

    public:
        BinaryDecoder(const std::string& buf);

        bool RawRead(void* data, int length);
        bool Read(char* data, int length);
        bool Read(std::string& data);
        bool Read(std::string& data, int length);
        bool Read(int& data);
        bool Read(short& data);
        bool Read(char& data);
        bool Read(float& data);

        bool ReadReverse(int& data);
        bool ReadReverse(short& data);
        bool ReadReverse(float& data);
    };

    class SHARED_EXPORT BinaryEncoder
    {
    private:
        void AppendZero(std::string& Buffer, int count);
    protected:
        std::string Buffer;

    public:
        BinaryEncoder();

        void Clean();

        void RawWrite(const void* data, int length);
        void Write(const char* data, int length);
        void Write(const std::string& data);
        void Write(const std::string& data, int length);
        void Write(const char* data);
        void Write(char data);
        void Write(int data);
        void Write(short data);
        void Write(float data);

        void WriteReverse(int data);
        void WriteReverse(short data);
        void WriteReverse(float data);

        virtual std::string& GetData();
    };

}

#endif

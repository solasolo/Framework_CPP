#pragma once

#ifndef _SF_SIMPLE_PROTOCOL_H_
#define _SF_SIMPLE_PROTOCOL_H_

#include <vector>

#include "CommonDef.hpp"
#include "BaseCodec.hpp"
#include "BaseEvent.hpp"
#include "BaseCodec.hpp"

namespace SoulFab::Link
{
    class SimpleProtocolEventSource
    {
    private:

    public:
        SimpleProtocolEventSource();

        void DataReady(const std::string& cmd, const std::string& msg);
    };
    
    class SHARED_EXPORT SimpleProtocolDecoder : public BinaryDecoder
    {
    public:
        SimpleProtocolDecoder(const std::string& buf);

        template<typename T> SimpleProtocolDecoder& operator>> (T& data);
    };

    class SHARED_EXPORT SimpleProtocolEncoder : public BinaryEncoder
    {
    private:
        bool FullFrame;

    public:
        SimpleProtocolEncoder(short cmd);
        SimpleProtocolEncoder();

        template<typename T> SimpleProtocolEncoder& operator<< (const T& data);

        virtual std::string& GetData();
    };

    class SHARED_EXPORT SimpleFrameCodec : public IFrameCodec<short>
    {
    private:
        const int CMD_SIZE = 2;
        const int LEN_SIZE = 4;
        const int HEAD_SIZE = CMD_SIZE + LEN_SIZE;

    public:
        bool Decode(StreamBuffer& stream, short& cmd, std::string& block) override;
        void Encode(std::string& message, short cmd, const std::string& block) override;
    };

    template<typename T>
    SimpleProtocolDecoder& SimpleProtocolDecoder::operator>> (T& data)
    {
        this->Read(data);

        return *this;
    }

    template<typename T>
    SimpleProtocolEncoder& SimpleProtocolEncoder::operator<< (const T& data)
    {
        this->Write(data);

        return *this;
    }
}

#endif

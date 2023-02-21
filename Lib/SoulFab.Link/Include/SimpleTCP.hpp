#pragma once

#ifndef _SF_SIMPLE_TCP_H_
#define _SF_SIMPLE_TCP_H_

#include "TCPServer.hpp"
#include "TCPClient.hpp"
#include "SimpleProtocol.hpp"
#include "BaseConfig.hpp"
#include "UniData.hpp"
#include "BaseEvent.hpp"
#include "SimpleProtocol.hpp"
#include "TelegramBuilder.hpp"

namespace SoulFab::Link
{
#pragma region Declarations

    template<typename T>
    class MessageBuilder
    {
    public:
        MessageBuilder();
        ~MessageBuilder();

        std::string BuildTel(const T& cmd, const SoulFab::Data::UniData& data);
        SoulFab::Data::UniData ParsedTel(const T& md, const SoulFab::Base::ByteArray& tel);

        void setCodec(IMessageCodec<T>* codet);

    private:
        IMessageCodec<T>* MessageCodec;
    };

    template<typename T, class F>
    class SHARED_EXPORT MessageTCPServer : public TCPServer<T, F>
    {
    public:
        MessageTCPServer(const std::string ip, unsigned int port);

        void Send(const T& cmd, const SoulFab::Data::UniData& data);

    protected:
        MessageBuilder<T> Builder;
        
        void HandleFrame(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload) override;
        virtual void HandleMessage(IChannel& conn, T cmd, SoulFab::Data::UniData& data) = 0;
    };

    template<typename T, class F>
    void setCodec(IMessageCodec<T>* codet)
    {
        this->MessageCodec = codec;
    }

    template<typename T, class F>
    class SHARED_EXPORT MessageTCPClient : public TCPClient<T, F>
    {
    public:
        MessageTCPClient(const std::string ip, unsigned int port);

        void Send(const T& cmd, const SoulFab::Data::UniData& data);

    protected:
        MessageBuilder<T> Builder;
       
        void HandleFrame(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload) override;
        virtual void HandleMessage(IChannel& conn, const T& cmd, const SoulFab::Data::UniData& data) = 0;
    };

    class SHARED_EXPORT SimpleTCPClient : public MessageTCPClient<short, SimpleFrameCodec>
    {
    public:
        SimpleTCPClient(const std::string ip, unsigned int port, SoulFab::Base::String schema_file);
    };


    class SHARED_EXPORT SimpleTCPServer : public MessageTCPServer<short, SimpleFrameCodec>
    {
    public:
        SimpleTCPServer(const std::string ip, unsigned int port, SoulFab::Base::String schema_file);
    };

#pragma endregion

#pragma region MessageBuilder

    template<typename T>
    MessageBuilder<T>::MessageBuilder()
        : MessageCodec(nullptr)
    {
    }

    template<typename T>
    MessageBuilder<T>::~MessageBuilder()
    {
        SafeDestroy(this->MessageCodec);
    }

    template<typename T>
    std::string MessageBuilder<T>::BuildTel(const T& cmd, const SoulFab::Data::UniData& data)
    {
        string tel;
        string block;

        if (this->MessageCodec)
        {
            this->MessageCodec->Encode(cmd, block, data);
        }
        else
        {
            throw Exception("Null Message Codec");
        }

        return block;
    }

    template<typename T>
    SoulFab::Data::UniData MessageBuilder<T>::ParsedTel(const T& cmd, const SoulFab::Base::ByteArray& tel)
    {
        auto data = SoulFab::Data::UniData::Object();

        if (this->MessageCodec)
        {
            this->MessageCodec->Decode(cmd, tel, data);
        }
        else
        {
            throw Exception("Null Message Codec");
        }

        return data;
    }

    template<typename T>
    void MessageBuilder<T>::setCodec(IMessageCodec<T>* codec)
    {
        this->MessageCodec = codec;
    }

#pragma endregion

#pragma region MessageTCPServer

    template<typename T, class F>
    MessageTCPServer<T, F>::MessageTCPServer(const std::string ip, unsigned int port)
        : TCPServer<T, F>(ip, port)
    {
        
    }

    template<typename T, class F>
    void MessageTCPServer<T, F>::Send(const T& cmd, const SoulFab::Data::UniData& data)
    {
        auto tel = this->Builder.BuildTel(cmd, data);

        this->TCPServer<T, F>::Send(cmd, tel);
    }

    template<typename T, class F>
    void MessageTCPServer<T, F>::HandleFrame(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload)
    {
        auto data = this->Builder.ParsedTel(cmd, payload);

        this->HandleMessage(conn, cmd, data);
    }

#pragma endregion

#pragma region MessageTCPClient

    template<typename T, class F>
    MessageTCPClient<T, F>::MessageTCPClient(const std::string ip, unsigned int port)
        : TCPClient<T, F>(ip, port)
    {

    }

    template<typename T, class F>
    void MessageTCPClient<T, F>::Send(const T& cmd, const SoulFab::Data::UniData& data)
    {
        auto&& tel = this->Builder.BuildTel(cmd, data);

        this->FrameChannel<T, F>::Send(cmd, tel);
    }

    template<typename T, class F>
    void MessageTCPClient<T, F>::HandleFrame(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload)
    {
        const auto& data = this->Builder.ParsedTel(cmd, payload);

        this->HandleMessage(conn, cmd, data);
    }

#pragma endregion

}

#endif

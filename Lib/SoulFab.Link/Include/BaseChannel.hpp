#pragma once

#ifndef _SF_BASE_CHANNEL_H_
#define _SF_BASE_CHANNEL_H_

#include <memory>

#include "Thread.hpp"
#include "StreamBuffer.hpp"
#include "BaseEvent.hpp"
#include "BaseCodec.hpp"
#include "ByteArray.hpp"

namespace SoulFab::Link
{
    class IChannel
    {
    public:
        virtual void Send(const SoulFab::Base::ByteArray& buf) = 0;

    protected:
        virtual int Receive(SoulFab::Base::ByteArray& buf) = 0;
        virtual void Connect() = 0;
        virtual void Reset() = 0;
    };

    class SHARED_EXPORT BaseChannel : public SoulFab::System::Thread, public IChannel
    {
    public:
        static bool _Debug_;

    public:
        BaseChannel();
        BaseChannel(const SoulFab::Base::String name);

        virtual ~BaseChannel();

        void Start() override;
        void Shutdown() override;

        bool IsOnline();
        virtual bool Available() = 0;

        StreamBuffer& GetData();

    protected:
        bool _IsOnline;
        int ConnectTimer;
        int ReceiveTimer;

        SoulFab::System::SimpleThread<BaseChannel>* DataThread;

        virtual void HandleData(BaseChannel& channel) = 0;
        virtual void HandleConnected(IChannel& channel) = 0;

        void Task() override;
        void DataTask();

    private:
        StreamBuffer Buffer;
    };


    template<typename T, class F>
    class FrameChannel : public BaseChannel, public F
    {
    public:
        FrameChannel();
        FrameChannel(const SoulFab::Base::String name);
        ~FrameChannel();

        void Start() override;
        void Shutdown() override;

        virtual void Send(const SoulFab::Base::ByteArray& buf) = 0;
        void Send(const T& cmd, const SoulFab::Base::ByteArray& msg);

        void setCodec(IFrameCodec<T>* codec);

    protected:
        void HandleData(BaseChannel& conn) override;
        virtual void HandleFrame(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload) = 0;

    private:
        IFrameCodec<T>* FrameCodec;

        SoulFab::System::SimpleThread<FrameChannel<T, F>>* WatchdogThread;

        void WatchdogTask();
    };

    template<typename T>
    class IFrameCallBack
    {
    public:
        virtual void OnFrameData(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload) = 0;
        virtual void OnConnected(IChannel& conn) = 0;
    };

    template<typename T>
    class SHARED_EXPORT FrameSource
    {
    private:

    public:
        FrameSource();
        void setFrameHandle(IFrameCallBack<T>* handle);
    };

    //
    //
    //

    template<typename T, class F>
    FrameChannel<T, F>::FrameChannel()
        : BaseChannel(), FrameCodec(nullptr)
    {
        this->WatchdogThread = new SoulFab::System::SimpleThread<FrameChannel<T, F>>("Frame Channel", this, &FrameChannel<T, F>::WatchdogTask);
    }

    template<typename T, class F>
    FrameChannel<T, F>::FrameChannel(const SoulFab::Base::String name)
        : BaseChannel(name)
    {
        this->WatchdogThread = new SoulFab::System::SimpleThread<FrameChannel<T, F>>(name, this, &FrameChannel<T, F>::WatchdogTask);
    }

    template<typename T, class F>
    FrameChannel<T, F>::~FrameChannel()
    {
        this->Shutdown();

        SafeDestroy(this->WatchdogThread);
    }

    template<typename T, class F>
    void FrameChannel<T, F>::Start()
    {
        this->BaseChannel::Start();
        this->WatchdogThread->Start();
    }

    template<typename T, class F>
    void FrameChannel<T, F>::Shutdown()
    {
        this->BaseChannel::Shutdown();

        if (this->WatchdogThread)
        {
            this->WatchdogThread->Shutdown();
        }
    }

    template<typename T, class F>
    void FrameChannel<T, F>::Send(const T& cmd, const SoulFab::Base::ByteArray& data)
    {
        SoulFab::Base::ByteArray msg;

        this->Encode(msg, cmd, data);
        this->Send(msg);
    }

    template<typename T, class F>
    void FrameChannel<T, F>::HandleData(BaseChannel& channel)
    {
        auto& buf = channel.GetData();

        T cmd;
        std::string block;

        while (this->Decode(buf, cmd, block))
        {
            this->HandleFrame(*this, cmd, block);
        }
    }

    template<typename T, class F>
    void FrameChannel<T, F>::WatchdogTask()
    {
    }
}

#endif

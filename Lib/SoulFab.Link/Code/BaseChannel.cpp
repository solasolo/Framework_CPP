#include "BaseChannel.hpp"

using namespace SoulFab::Base;
using namespace SoulFab::System;

namespace SoulFab::Link
{
#pragma region BaseChannel

    bool BaseChannel::_Debug_ = false;


    BaseChannel::BaseChannel()
        : BaseChannel("Channel")
    {
    }

    BaseChannel::BaseChannel(const String name)
        : Thread(name, true), _IsOnline(false), ConnectTimer(3000), ReceiveTimer(10)
    {
        this->DataThread = new SimpleThread<BaseChannel>("Channel Data", this, &BaseChannel::DataTask);
    }

    BaseChannel::~BaseChannel()
    {
        this->Shutdown();
    }

    void BaseChannel::Start()
    {
        this->DataThread->Start();

        this->Thread::Start();
    }
    
    void BaseChannel::Shutdown()
    {
        this->DataThread->Shutdown();

        this->Thread::Shutdown();
    }

    void BaseChannel::Task()
    {
        this->SleepTimer = 10;

        if (!this->IsOnline())
        {
            this->Connect();
        }

        try
        {
            if (this->IsOnline())
            {
                ByteArray data;

                int count = Receive(data);
                if (count > 0)
                {
                    this->Buffer << data;

                    this->SleepTimer = 0;
                }
            }
        }
        catch (ChannelException& ex)
        {
            this->Reset();

            throw ex;
        }
    }

    void BaseChannel::DataTask()
    {
        this->Buffer.Wait();

        this->HandleData(*this);
    }

    bool BaseChannel::IsOnline()
    {
        return this->_IsOnline;
    }

    StreamBuffer& BaseChannel::GetData()
    {
        return this->Buffer;
    }

#pragma endregion

#pragma region FrameChannel


#pragma endregion

}

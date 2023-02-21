#pragma once

#ifndef _SF_TCP_CLIENT_H_
#define _SF_TCP_CLIENT_H_

#include <string>

#include "TCPConnection.hpp"

namespace SoulFab::Link
{
    template<typename T, class F>
    class SHARED_EXPORT TCPClient : public TCPConnection<T, F>
	{
    public:
        TCPClient(const std::string ip, int port);
        ~TCPClient();

        void setConnectTimer(int timer);
    
    protected:
        void Connect() override;
    };

    //
    //
    //

    template<typename T, class F>
    TCPClient<T, F>::TCPClient(const std::string ip, int port)
        : TCPConnection<T, F>()
    {
        this->PeerIP = ip;
        this->PeerPort = port;
    }

    template<typename T, class F>
    TCPClient<T, F>::~TCPClient()
    {
        this->Shutdown();
    }

    template<typename T, class F>
    void TCPClient<T, F>::setConnectTimer(int timer)
    {
        ConnectTimer = timer;
    }

    template<typename T, class F>
    void TCPClient<T, F>::Connect()
    {
        if (!this->IsOnline())
        {
            try
            {
                int WaitTime = this->ConnectTimer + (long(rand()) * this->ConnectTimer / 100000);

                if (TCPConnection<T, F>::_Debug_)
                {
                    this->DebugLog("Try to Connect to %s:%d After %d ms", this->PeerIP.c_str(), this->PeerPort, WaitTime);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(WaitTime));

                SafeDestroy(this->InnerSocket);
                this->InnerSocket = new Socket();

                this->InnerSocket->Connect(this->PeerIP, this->PeerPort);
                this->_IsOnline = true;

                this->HandleConnected(*this);
                this->MessageLog("Connected to %s:%d", this->PeerIP.c_str(), this->PeerPort);

            }
            catch (SocketException& ex)
            {
                this->Log(EventType::Error, ex.GetDescription());
            }
        }
    }
}

#endif

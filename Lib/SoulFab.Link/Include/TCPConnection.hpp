#pragma once

#ifndef _SF_TCP_CONNECTION_H_
#define _SF_TCP_CONNECTION_H_

#include <string>

#include "Thread.hpp"
#include "StreamBuffer.hpp"
#include "BaseEvent.hpp"
#include "Socket.hpp"
#include "BaseChannel.hpp"

namespace SoulFab::Link
{
    template<typename T, class F>
	class SHARED_EXPORT TCPConnection : public FrameChannel<T, F>
	{
    public:
        static bool _Debug_;

    public:
        TCPConnection();
        TCPConnection(class Socket* socket);
        virtual ~TCPConnection();

        int GetLocalPort();
        std::string GetLocalIP();

        bool Available() override;
        void Send(const SoulFab::Base::ByteArray& buf) override;

	protected:
		std::string LocalIP;
		int LocalPort;
		std::string PeerIP;
		int PeerPort;

		Socket* InnerSocket;

		SocketHandle getSocketHandle();

        int Receive(SoulFab::Base::ByteArray& buf) override;
        void Reset() override;
    };

    template<typename T, class F>
    bool TCPConnection<T, F>::_Debug_ = false;

    template<typename T, class F>
    TCPConnection<T, F>::TCPConnection()
        : FrameChannel<T, F>("Connection")
    {
        this->InnerSocket = new Socket();
        this->Start();
    }

    template<typename T, class F>
    TCPConnection<T, F>::TCPConnection(Socket* socket)
        : FrameChannel<T, F>("Connection")
    {
        this->InnerSocket = socket;
        this->Start();
    }

    template<typename T, class F>
    TCPConnection<T, F>::~TCPConnection()
    {
        this->Shutdown();

        SafeDestroy(this->InnerSocket);
    }

    template<typename T, class F>
    void TCPConnection<T, F>::Send(const SoulFab::Base::ByteArray& msg)
    {
        if (IsOnline() && this->InnerSocket != NULL)
        {
            try
            {
                this->InnerSocket->Send(msg);
            }
            catch (...)
            {
                this->Reset();
                throw;
            }
        }
        else
        {
            this->_IsOnline = false;

            throw SocketNoConnection(this->getSocketHandle(), "TCP Send");
        }
    }

    template<typename T, class F>
    std::string TCPConnection<T, F>::GetLocalIP()
    {
        return this->LocalIP;
    }

    template<typename T, class F>
    int TCPConnection<T, F>::GetLocalPort()
    {
        return this->LocalPort;
    }

    template<typename T, class F>
    SocketHandle TCPConnection<T, F>::getSocketHandle()
    {
        return this->InnerSocket ? this->InnerSocket->getHandle() : (SocketHandle)(-1);
    }

    template<typename T, class F>
    bool TCPConnection<T, F>::Available()
    {
        return (this->InnerSocket != NULL && this->_IsOnline);
    }

    template<typename T, class F>
    int TCPConnection<T, F>::Receive(SoulFab::Base::ByteArray& buf)
    {
        int ret = 0;

        if (this->InnerSocket)
        {
            if (this->InnerSocket->CanReceive())
            {
                ret = this->InnerSocket->Receive(buf);
            }
        }

        return ret;
    }

    template<typename T, class F>
    void TCPConnection<T, F>::Reset()
    {
        this->_IsOnline = false;
    }
}

#endif

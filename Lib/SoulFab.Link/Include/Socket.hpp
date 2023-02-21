#pragma once

#ifndef _SF_SOCKET_H_
#define _SF_SOCKET_H_

#include <string>

#include "BaseSocket.hpp"
#include "Thread.hpp"
#include "StreamBuffer.hpp"

namespace SoulFab::Link
{
	class SHARED_EXPORT Socket : public BaseSocket
	{
    private:
        static const int RECEIVE_BUFFER_LEN;

    private:
        char* ReceiveBuffer;
    
    public:
        Socket();
        Socket(SocketHandle s);
        virtual ~Socket();

        Socket* Accept();
        void Send(const SoulFab::Base::ByteArray& buf);
        int Receive(SoulFab::Base::ByteArray& buf);

        void SendTo();
        int ReceiveFrom();
	};

}

#endif

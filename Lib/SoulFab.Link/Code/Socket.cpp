#include "Socket.hpp"

using namespace std;

namespace SoulFab::Link
{
    const int Socket::RECEIVE_BUFFER_LEN = 4096;

    Socket::Socket()
        : BaseSocket(), ReceiveBuffer(nullptr)
    {
        this->ReceiveBuffer = new char[RECEIVE_BUFFER_LEN];

        this->CreateTCP();
    }

    Socket::Socket(SocketHandle s)
        : BaseSocket(s), ReceiveBuffer(nullptr)
    {
        this->ReceiveBuffer = new char[RECEIVE_BUFFER_LEN];
    }

    Socket::~Socket()
    {
        this->Close();

        SafeDestroy(this->ReceiveBuffer);
    }

    Socket* Socket::Accept()
    {
        return new Socket(this->BaseSocket::Accept());
    }

    void Socket::Send(const SoulFab::Base::ByteArray& buf)
    {
        this->BaseSocket::Send(buf.c_str(), buf.length());
    }

    int Socket::Receive(SoulFab::Base::ByteArray& buf)
    {
        int c = 0;

        if (this->CanReceive())
        {
            buf.clear();

            c = this->BaseSocket::Receive(this->ReceiveBuffer, RECEIVE_BUFFER_LEN);
            if (c > 0)
            {
                buf.append(this->ReceiveBuffer, c);
            }
        }

        return c;
    }


}

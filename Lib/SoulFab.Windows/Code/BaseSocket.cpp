#include "BaseSocket.hpp"

#include <WinSock2.h>
#include <ws2tcpip.h>

#include "SysException.hpp"

using namespace std;
using namespace SoulFab::Base; 
using namespace SoulFab::System;

namespace SoulFab::Link
{
#pragma region SocketException

    void SocketException::Check(BaseSocket* socket, const String fun, int ret_code)
    {
        if (ret_code == SOCKET_ERROR)
        {
            throw SocketException(socket->getHandle(), fun);
        }
    }

    SocketException::SocketException(SocketHandle handle, String where)
        : ChannelException(), Handle(handle)
    {
        this->Code = ::WSAGetLastError();
        this->Where = where;
        this->Message = SysException::SysErrorDescription(this->Code);
    }

    SocketException::SocketException(BaseSocket* socket, const SoulFab::Base::String where)
        : ChannelException(), Handle(socket->getHandle())
    {
        this->Code = ::WSAGetLastError();
        this->Where = where;
        this->Message = SysException::SysErrorDescription(this->Code);
    }

    SocketHandle SocketException::getHandle() const
    {
        return this->Handle;
    }

    const String SocketException::GetDescription() const noexcept
    {
        String ErrDescription;
        
        if(this->Handle == INVALID_SOCKET)
        {
            ErrDescription = "Connection NOT Available";
        }
        else
        {
            ErrDescription = String::Format("%s[BaseSocket Handle: %d]", Exception::GetErrorDescription().Pointer(), this->getHandle());
        }

        return ErrDescription;
    }


    SocketNoConnection::SocketNoConnection(const String where)
        : SocketException(INVALID_SOCKET, where)
    {
    }

    SocketNoConnection::SocketNoConnection(SocketHandle handle, const String where)
        : SocketException(handle, where) 
    {}

    const String SocketNoConnection::GetDescription() const noexcept
    {
        String ErrDescription;
        
        if(this->Handle == INVALID_SOCKET)
        {
            ErrDescription = "SOCKET NOT Available";
        }
        else
        {
            ErrDescription = "Connection NOT Available";
        }

        return ErrDescription;
    }

#pragma endregion

#pragma region BaseSocket

    void MakeAddress(string ip, unsigned int port, sockaddr_in& addr)
    {
        memset(&addr, 0, sizeof(addr));

        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
    }

    void ResoAddress(string name, unsigned int port, sockaddr_in& addr)
    {
        if (name.empty() || port == 0)
            throw Exception("Name is not correct!");

        hostent* hostEnt = NULL;
        long lIPAddress = 0;

        hostEnt = gethostbyname(name.c_str());

        if (hostEnt != NULL)
        {
            lIPAddress = ((in_addr*)hostEnt->h_addr)->s_addr;
            addr.sin_addr.s_addr = lIPAddress;
        }
        else
        {
            inet_pton(AF_INET, name.c_str(), &addr.sin_addr);
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
    }

    BaseSocket::BaseSocket()
        : InnerHandle(INVALID_SOCKET)
    {	
    }

    BaseSocket::BaseSocket(SocketHandle s)
    {
        if(s == INVALID_SOCKET)
        {
            throw Exception("Invalid BaseSocket Handle");
        }

        InnerHandle = s; 
    }

    void BaseSocket::Startup()
    {	
        WORD wVersionRequested = MAKEWORD( 2, 2 );
        WSADATA wsaData;

        int err;
        err =  WSAStartup(wVersionRequested, &wsaData);
        if (err != 0) throw SysException(err, "Initiates BaseSocket");
    }

    void BaseSocket::Cleanup()
    {
        WSACleanup();
    }

    SocketHandle BaseSocket::getHandle()
    {
        return InnerHandle;
    }

    string BaseSocket::getLocalAddress()
    {
        return this->IP;
    }

    int BaseSocket::getLocalPort()
    {
        return this->Port;
    }

    void BaseSocket::CreateTCP()
    {
        if ((this->InnerHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        {
            throw SocketException(this, "Create TCP Socket");
        }
    }

    void BaseSocket::CreateUDP(bool broadcast)
    {
        if ((this->InnerHandle = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
        {
            throw SocketException(this, "Create UDP Socket");
        }

        if (broadcast) 
        {
            char op = 1;
            int ret_code = setsockopt(this->InnerHandle, SOL_SOCKET, SO_BROADCAST, &op, sizeof(op));
            SocketException::Check(this, "Set Broadcast", ret_code);
        }
    }

    void BaseSocket::CreateRaw()
    {
    }

    bool BaseSocket::CanReceive()
    {
        TIMEVAL tv = { 0, 0 };
        int ret_code;
        fd_set fdr = { 1,  {InnerHandle} };

        ret_code = select(0, &fdr, NULL, NULL, &tv); //检查可读状态 
        SocketException::Check(this, "Check Readable", ret_code);

        return ret_code > 0 ? true : false;
    }

    void BaseSocket::Bind(string ip, unsigned int port)
    {
        if (ip.empty() || port == 0)
        {
            throw Exception("IP address or port number is not correct!");
        }

        sockaddr_in ipaddr;
        MakeAddress(ip, port, ipaddr);

        int ret_code = bind(InnerHandle, (SOCKADDR*)&ipaddr, sizeof(ipaddr));

        SocketException::Check(this, "BaseSocket Bind", ret_code);

        this->IP = ip;
        this->Port = port;
    }


    void BaseSocket::Connect(string ip, unsigned int port)
    {
        sockaddr_in ipaddr;
        ResoAddress(ip, port, ipaddr);

        int ret_code = connect(InnerHandle, (SOCKADDR*)&ipaddr, sizeof(ipaddr));

        SocketException::Check(this, "Socket Connect", ret_code);
    }

    void BaseSocket::Listen()
    {
        int ret_code = listen(InnerHandle, SOMAXCONN);

        SocketException::Check(this, "Socket Listen", ret_code);
    }

    SocketHandle BaseSocket::Accept()
    {	
        sockaddr_in remote_addr;
        int addr_size = sizeof(remote_addr);
        memset( &remote_addr, 0, sizeof(remote_addr) );

        SOCKET S;
        S = accept(this->InnerHandle, (SOCKADDR*)&remote_addr, &addr_size);

        if(S == INVALID_SOCKET )
        {
            SocketException::Check(this, "BaseSocket Accept", SOCKET_ERROR);
        }

        return S;
    }
    
    void BaseSocket::Send(const char* buf, unsigned int len)
    {
        int ret_code = 0;

        if (buf == NULL)
        {
            throw Exception("Data or data length is not correct!");
        }

        if (len > 0)
        {
            ret_code = send(this->InnerHandle, buf, len, 0);
        }

        SocketException::Check(this, "Socket Send", ret_code);
    }

    int BaseSocket::Receive(char* data, unsigned int len)
    {
        if (data == NULL)
            throw Exception("Data or data length is not correct!");

        int RecvLen = 0;

        if (CanReceive())
        {
            RecvLen = recv(this->InnerHandle, data, len, 0);
            SocketException::Check(this, "Soceket Receive", RecvLen);
        }

        return RecvLen;
    }

    void BaseSocket::Close()
    {
        int ret_code;

        if (InnerHandle != INVALID_SOCKET)
        {
            ret_code = shutdown(InnerHandle, SD_BOTH);
            //		CheckError("Shutdown Socket", ret_code);

            ret_code = closesocket(InnerHandle);
            SocketException::Check(this, "Close Socket", ret_code);
        }
    }

#pragma endregion

}

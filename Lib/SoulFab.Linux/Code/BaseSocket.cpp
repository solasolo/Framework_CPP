#include "BaseSocket.hpp"

#include <sys/socket.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

namespace SoulFab::Link
{
#pragma region SocketException

    SocketException::SocketException(SocketHandle handle, const char* where)
    {
        this->Handle = handle;
    }

    SocketHandle SocketException::getHandle()
    {
        return this->Handle;
    }

    string SocketException::GetDescription() throw()
    {
        string ErrDescription;
        
        if(this->Handle == INVALID_SOCKET)
        {
            ErrDescription = "Connection NOT Available";
        }
        else
        {
            ErrDescription = Exception::GetDescription() + "[BaseSocket Handle:" + to_string(this->getHandle()).c_str() + "]";
        }

        return ErrDescription;
    }


    SocketNoConnection::SocketNoConnection(const char* where)
        : SocketException(INVALID_SOCKET, where)
    {
    }

    SocketNoConnection::SocketNoConnection(SocketHandle handle, const char* where)
        : SocketException(handle, where) 
    {}

    string SocketNoConnection::GetDescription() throw()
    {
        string ErrDescription;
        
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
        if (err != 0) throw WinException(err, "Initiates BaseSocket");
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

    void BaseSocket::CheckError(const char* Function, int ret_code )
    {
        if( ret_code == SOCKET_ERROR )
            throw SocketException(this->getHandle(), Function);
    }

    void BaseSocket::Create()
    {
        if ((InnerHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        {
            CheckError("Create Socket", SOCKET_ERROR);
        }
    }

    bool BaseSocket::CanReceive()
    {
        TIMEVAL tv = { 0, 0 };
        int ret_code;
        fd_set fdr = { 1,  {InnerHandle} };

        ret_code = select(0, &fdr, NULL, NULL, &tv); //检查可读状态 
        CheckError("Check Readable", ret_code);

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

        CheckError("BaseSocket Bind", ret_code);

        this->IP = ip;
        this->Port = port;
    }


    void BaseSocket::Connect(string ip, unsigned int port)
    {
        sockaddr_in ipaddr;
        ResoAddress(ip, port, ipaddr);

        int ret_code = connect(InnerHandle, (SOCKADDR*)&ipaddr, sizeof(ipaddr));

        CheckError("Socket Connect", ret_code);
    }

    void BaseSocket::Listen()
    {
        int ret_code = listen(InnerHandle, SOMAXCONN);

        CheckError("Socket Listen", ret_code);
    }

    SocketHandle BaseSocket::Accept()
    {	
        sockaddr_in r_addr;
        int Len = sizeof(r_addr);

        memset( &r_addr, 0, sizeof(r_addr) );

        SOCKET S;
        S = accept(InnerHandle, (SOCKADDR*)&r_addr, &Len);

        if(S == INVALID_SOCKET )
        {
            CheckError("BaseSocket Accept", SOCKET_ERROR);
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

        if (len > 0)	ret_code = send(InnerHandle, buf, len, 0);

        CheckError("Socket Send", ret_code);
    }

    int BaseSocket::Receive(char* data, unsigned int len)
    {
        if (data == NULL)
            throw Exception("Data or data length is not correct!");

        int RecvLen = 0;

        if (CanReceive())
        {
            RecvLen = recv(this->InnerHandle, data, len, 0);
            CheckError("Soceket Receive", RecvLen);
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
            CheckError("Close Socket", ret_code);
        }
    }

#pragma endregion

}
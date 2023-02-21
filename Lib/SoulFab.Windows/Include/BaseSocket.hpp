#pragma once

#ifndef _SF_BASE_SOCKET_H_
#define _SF_BASE_SOCKET_H_

#include "Exception.hpp"
#include "SysException.hpp"
#include "CommonDef.hpp"

namespace SoulFab::Link
{
    typedef uint64_t SocketHandle;

    class BaseSocket;

    class SHARED_EXPORT SocketException : public SoulFab::Base::ChannelException
    {
    public:
        static void Check(BaseSocket* socket, const SoulFab::Base::String func, int ret_code);

    public:
        SocketException(SocketHandle handle, const SoulFab::Base::String where);
        SocketException(BaseSocket* socket, const SoulFab::Base::String where);

        SocketHandle getHandle() const;
        virtual const SoulFab::Base::String GetDescription() const noexcept;
        
    protected:
        SocketHandle Handle;
    };

    class _declspec(dllexport) SocketNoConnection : public SocketException
    {
    public:
        SocketNoConnection(const SoulFab::Base::String where);
        SocketNoConnection(SocketHandle handle, const SoulFab::Base::String where);

        virtual const SoulFab::Base::String GetDescription() const noexcept;
    };

    class SHARED_EXPORT BaseSocket
    {
    public:
        static void Startup();
        static void Cleanup();

    public:				
        SocketHandle getHandle();

        void Bind(const std::string ip, unsigned int port);
        void Listen();

        void Connect(const std::string ip, unsigned int port);
        bool CanReceive();

        void Close();

    protected:	
        int SocketVersion;					
        SocketHandle InnerHandle;	
	
        int Port;
        std::string IP;

        BaseSocket();
        BaseSocket(SocketHandle s);

        std::string getLocalAddress();
        int getLocalPort();
        std::string getRemoteAddress();
        int getRemotePort();

        SocketHandle Accept();
        int Receive(char* data, unsigned int len); 
        void Send(const char* buf, unsigned int len);

        void CreateTCP();		
        void CreateUDP(bool broadcast = false);
        void CreateRaw();
    };
}

#endif

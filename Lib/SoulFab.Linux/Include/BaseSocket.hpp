#pragma once

#ifndef _SF_BASE_SOCKET_H_
#define _SF_BASE_SOCKET_H_

#include "Exception.hpp"

using namespace std;
using namespace SoulFab::Base; 

namespace SoulFab::Link
{
    typedef int SocketHandle;

    class BaseSocket;

    class _declspec(dllexport) SocketException : public Exception
    {
    public:
        SocketException(SocketHandle handle, const char* where);

        SocketHandle getHandle();
        virtual std::string GetDescription() throw();
        
    protected:
        SocketHandle Handle;
    };

    class _declspec(dllexport) SocketNoConnection : public SocketException
    {
    public:
        SocketNoConnection(const char* where);
        SocketNoConnection(SocketHandle handle, const char* where);

        virtual std::string GetDescription() throw();
    };


    class _declspec(dllexport) BaseSocket
    {
    public:
        static void Startup();
        static void Cleanup();

    public:				
        SocketHandle getHandle();

        void Bind(std::string ip, unsigned int port);
        void Listen();

        void Connect(string ip, unsigned int port);
        bool CanReceive();

    protected:	
        int SocketVersion;					
        SocketHandle InnerHandle;	
	
        int Port;
        string IP;

        BaseSocket();
        BaseSocket(SocketHandle s);

        virtual void CheckError(const char* Function, int ret_code);

        std::string getLocalAddress();
        int getLocalPort();
        std::string getRemoteAddress();
        int getRemotePort();

        SocketHandle Accept();
        int Receive(char* data, unsigned int len); 
        void Send(const char* buf, unsigned int len);
        void Create();				
        void Close();
    };
}

#endif
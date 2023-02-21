#pragma once

#ifndef _SF_TCP_SERVER_H_
#define _SF_TCP_SERVER_H_

#include <list>

#include "TCPConnection.hpp"
#include "Concurrency.hpp"
#include "Socket.hpp"

namespace SoulFab::Link
{
    template<typename T, class F>
    class TCPServer;

    template<typename T, class F>
    class TCPServerConnectioin : public TCPConnection<T, F>
    {
        friend class TCPServer<T, F>;

    public:
        TCPServerConnectioin(TCPServer<T, F>* server, Socket* socket);

    protected:
        void HandleFrame(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload) override;
        void HandleConnected(IChannel& conn) override;

        void Connect() override;
        void Reset() override;

    private:
        TCPServer<T, F>* Server;
    };

    template<typename T, class F>
    class SHARED_EXPORT TCPServer : public SoulFab::System::Thread
    {
        friend class TCPServerConnectioin<T, F>;

    public:
        TCPServer(const std::string ip, unsigned int port, bool Single = false);
        ~TCPServer();

        int GetClientsCount();

        void Send(const T& cmd, const SoulFab::Base::ByteArray& msg);

        void Start() override;
        void Shutdown() override;

    protected:
        virtual void Task();

        virtual void HandleConnected(IChannel& conn) = 0;
        virtual void HandleFrame(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload) = 0;

        void SendAll(const std::string& msg);

    private:
        bool Quiting;
        bool Single;

        std::string Address;
        unsigned int Port;

        Socket* Listener;
        SoulFab::System::ConcurrencyList<TCPServerConnectioin<T, F>*> Connections;

        void ClearConn();
    };

#pragma region TCPServerConnectioin

    template<typename T, class F>
    TCPServerConnectioin<T, F>::TCPServerConnectioin(TCPServer<T, F>* server, Socket* socket)
        : TCPConnection<T, F>(socket), Server(server)
    {
        this->_IsOnline = true;
    }

    template<typename T, class F>
    void TCPServerConnectioin<T, F>::HandleFrame(IChannel& conn, const T& cmd, const SoulFab::Base::ByteArray& payload)
    {
        this->Server->HandleFrame(conn, cmd, payload);
    }

    template<typename T, class F>
    void TCPServerConnectioin<T, F>::HandleConnected(IChannel& conn)
    {
        this->Server->HandleConnected(conn);
    }

    template<typename T, class F>
    void TCPServerConnectioin<T, F>::Connect()
    {
    }

    template<typename T, class F>
    void TCPServerConnectioin<T, F>::Reset()
    {
        TCPConnection<T, F>::Reset();
    }

#pragma endregion


#pragma region TCPServer

    template<typename T, class F>
    TCPServer<T, F>::TCPServer(const std::string ip, unsigned int port, bool Single)
        : SoulFab::System::Thread("TCP Server", true)
    {
        Quiting = false;
        this->Listener = NULL;
        this->Single = Single;

        Address = ip;
        Port = port;

        this->Listener = new Socket();
        this->Listener->Bind(ip, port);
    }

    template<typename T, class F>
    TCPServer<T, F>::~TCPServer()
    {
        this->Shutdown();

        SafeDestroy(Listener);
    }

    template<typename T, class F>
    void TCPServer<T, F>::Start()
    {
        this->Listener->Listen();

        this->Thread::Start();
    }

    template<typename T, class F>
    void TCPServer<T, F>::Shutdown()
    {
        // Stop Listen
        if (this->Listener)
        {
            this->Listener->Close();
        }

        this->ClearConn();

        this->Thread::Shutdown();
    }

    template<typename T, class F>
    void TCPServer<T, F>::Send(const T& cmd, const SoulFab::Base::ByteArray& msg)
    {
        this->Connections.Foreach([this, &cmd, &msg](auto& c)
            {
                bool ret = true;

                try
                {
                    c->FrameChannel<T, F>::Send(cmd, msg);
                }
                catch (SocketException& ex)
                {
                    c->Shutdown();
                    SafeDestroy(c);

                    ret = false;

                    this->ErrorLog(ex);
                }

                return ret;
            }
        );
    }

    template<typename T, class F>
    void TCPServer<T, F>::SendAll(const std::string& msg)
    {
        this->Connections.Foreach(
            [this, &msg](auto& c)
            {
                bool ret = true;

                try
                {
                    c->Send(msg);
                }
                catch (SocketException& ex)
                {
                    c->Shutdown();
                    SafeDestroy(c);

                    ret = false;

                    this->ErrorLog(ex);
                }

                return ret;
            }
        );
    }

    template<typename T, class F>
    int TCPServer<T, F>::GetClientsCount()
    {
        return (int)Connections.Count();
    }

    template<typename T, class F>
    void TCPServer<T, F>::Task()
    {
        Socket* socket = this->Listener->Accept();
        auto conn = new TCPServerConnectioin(this, socket);
        conn->setLogger(this->InnerLogger);
        conn->Start();

        this->Log(EventType::Message, "Client Linked %s:%d", this->Address.c_str(), this->Port);

        if (this->Single)
        {
            ClearConn();
        }
        this->Connections.Add(conn);

        this->HandleConnected(*conn);
    }

    template<typename T, class F>
    void TCPServer<T, F>::ClearConn()
    {
        this->Connections.Foreach(
            [this](auto& c)
            {
                try
                {
                    c->Shutdown();
                    SafeDestroy(c);
                }
                catch (Exception& ex)
                {
                    this->ErrorLog(ex);
                }

                return true;
            }
        );

        this->Connections.Clear();
    }

#pragma endregion

}

#endif

#include <iostream>
#include <string>
#include <ctime>
#include <conio.h>

#include "TCPServer.hpp"
#include "TCPClient.hpp"
#include "SimpleProtocol.hpp"
#include "ConsoleLogger.hpp"

using namespace std;

using namespace SoulFab::Link;
using namespace SoulFab::Base;
using namespace SoulFab::Logger;

class Tester : public TCPServer<short, SimpleFrameCodec>
{
public:
    Tester(string ip, int port) : TCPServer<short, SimpleFrameCodec>(ip, port) {}

protected:
    virtual void HandleConnected(IChannel& conn) override
    {
        cout << "Connected" << endl;
    }

    void HandleFrame(IChannel& conn, const short& cmd, const ByteArray& msg) override
    {
        cout << "Server Got: " << msg << endl;
    }
};

int main(int argc, char* argv[])
{
	char buf[20];

	string ip = "127.0.0.1";
	int port = 4444;

	if(argc >=3)
	{
		ip = argv[1];
		port = atoi(argv[2]);
	}

    Tester Server (ip, port);
	cout << "server Socket has created!" << endl;

	ConsoleLogger logger;

	Server.setLogger(&logger);

	while(!_kbhit())
	{
		try
		{
			//Server.SendAll(string("ab123fghi"));
			string data = string("afrom server ") + _strtime(buf);
			Server.Send(1, data);
			Sleep(1000);
		}
		catch(Exception e)
		{
			cout << e.GetDescription().Pointer() << endl;
		}

	};

	return 0;
}

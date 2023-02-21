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

class Tester : public TCPClient<short, SimpleFrameCodec>
{
public:
    Tester(string ip, int port) : TCPClient<short, SimpleFrameCodec>(ip, port) {}

protected:
    void HandleConnected(IChannel& conn) override
    {
        cout << "Connection" << endl;
    }

    void HandleFrame(IChannel& conn, const short& cmd, const ByteArray& payload) override
    {
        cout << "Client Got: " << payload << endl;
    }
};

int main(int argc, char* argv[])
{
	string ip = "127.0.0.1";
	int port = 4444;

	if(argc >=3)
	{
		ip = argv[1];
		port = atoi(argv[2]);
	}

	char buf[20];
    Tester Client(ip, port);
    Client.Start();
	cout << "client Socket has created!" << endl;
	
	//while(!Client.IsOnline())
	//{
	//	try
	//	{
	//		Client.Connect((char*)IP.c_str(), 4444);
	//	}
	//	catch(WinSocketException e)
	//	{
	//		wcout << e.where() << L":" << e.why() << L":" << e.what() << endl;
	//	}
	//	catch(WinException e)
	//	{
	//		wcout << e.where() << L":" << e.why() << L":" << e.what() << endl;
	//	}
	//}
	

	while(!kbhit())
	{
		try
		{
			while(Client.IsOnline())
			{
				string data = string("from client ") + buf;

				Client.Send(data);
				Sleep(1000);
			}

			Sleep(100);
		}
		catch(Exception e)
		{
			cout << e.GetDescription().Pointer() << endl;
		}
	};

	return 0;
}


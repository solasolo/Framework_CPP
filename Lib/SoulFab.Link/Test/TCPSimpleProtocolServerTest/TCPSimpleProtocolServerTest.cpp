
#include "stdafx.h"

#include "WinTCP.h"
#include "SimpleProtocol.h"

#include <iostream>
#include <string>
#include <ctime>
#include <conio.h>

using namespace std;

class TestServer : public TCPServerSimpleProtocol
{
protected:
	virtual void ProcessCommand(TCPConnection& conn, short cmd, BinaryDecoder& decoder);
	virtual void ProcessConnect(TCPConnection& conn);
	virtual void ProcessNotice(EventType type, wstring& msg);

public:
	TestServer(string ip, int port);

};

TestServer::TestServer(string ip, int port)
	: TCPServerSimpleProtocol()
{
	this->Start(ip, port);
}

void TestServer::ProcessCommand(TCPConnection& conn, short cmd, BinaryDecoder& decoder)
{
	int num;
	string name ;
	
	decoder.Read(num);
	decoder.Read(name);

	cout << "Server Got: [" << cmd  << "]" << num << " " << name << endl;
}

void TestServer::ProcessConnect(TCPConnection& conn)
{
}

void TestServer::ProcessNotice(EventType type, wstring& msg)
{
	wcout << msg << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char buf[20];
	TestServer  Server ("127.0.0.1", 4444);
	cout << "server Socket has created!" << endl;

	short cmd = 100;
	SimpleProtocolEncoder* encoder = new SimpleProtocolEncoder(cmd);

	int n = 0;
	while(!kbhit())
	{
		try
		{
			string msg = string("from server ") + _strtime(buf);
			
			// ±àÂëºóÔÙ·¢ËÍ
			short i = 1;
			(*encoder) << i;
			(*encoder) << msg;
			msg = encoder->GetData();

			Server.Send(1000, msg);
			Sleep(1000);
			n++;
		}
		catch(WinSocketException e)
		{
			wcout << e.where() << L":" << e.why() << L":" << e.what() << endl;
		}
		catch(WinException e)
		{
			wcout << e.where() << L":" << e.why() << L":" << e.what() << endl;
		}

	};

	wcout << L"over";
	system("pause");

	return 0;
}

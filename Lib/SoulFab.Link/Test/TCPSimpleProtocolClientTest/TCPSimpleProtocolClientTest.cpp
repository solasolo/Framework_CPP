#include "stdafx.h"

#include "WinTCP.h"
#include "SimpleProtocol.h"
#include <iostream>
#include <string>
#include <ctime>
#include <conio.h>

using namespace std;


class TestClinet : public TCPClientSimpleProtocol
{
protected:
	virtual void ProcessCommand(TCPConnection& conn, short cmd, BinaryDecoder& decoder);
	virtual void ProcessConnect(TCPConnection& conn);
	virtual void ProcessNotice(EventType type, wstring& msg);

public:
	TestClinet(string ip, int port);

};

TestClinet::TestClinet(string ip, int port)
	: TCPClientSimpleProtocol(ip, port)
{

}

void TestClinet::ProcessCommand(TCPConnection& conn, short cmd, BinaryDecoder& decoder)
{
	int num;
	string name ;
	
	decoder.Read(num);
	decoder.Read(name);

	cout << "Server Got: [" << cmd  << "]" << num << " " << name << endl;
}

void TestClinet::ProcessConnect(TCPConnection& conn)
{
}

void TestClinet::ProcessNotice(EventType type, wstring& msg)
{
	wcout<<msg<<endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char buf[20];
	TestClinet Client("127.0.0.1", 4444);
	cout << "client Socket has created!" << endl;

	while(!kbhit())
	{
		try
		{
			while(Client.IsOnline())
			{
				Client.Send(1, string("Hello Server"));
				Sleep(1000);
			}

			Sleep(100);
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

	return 0;
}


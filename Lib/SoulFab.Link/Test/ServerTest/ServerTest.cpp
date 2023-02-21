// ServerTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "Winsocket.h"

#include <windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		WinSocket Server;
		cout << "Socket has created!" << endl;
		Server.Bind("192.168.1.16", 8888);
		cout << "Open port!" << endl;
		
		cout << "Listening!" << endl;
		Server.Listen();

		cout << "Waiting connect!" << endl;
		
		SOCKET a;
		while(!Server.CanReceive());
		a = Server.Accept();

		WinSocket s(a);
        cout << "Connected!" << endl;

		Sleep(1000);

		s.Send("Hello",5);

	}
	catch(WinSocketException e)
	{
		wcout << e.where() << L":" << e.why() << L":" << e.what() << endl;

	}
	catch(WinException e)
	{
		wcout << e.where() << L":" << e.why() << L":" << e.what() << endl;
	}

	cin.get();

	return 0;
}


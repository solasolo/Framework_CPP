// ClientTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Winsocket.h"

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		char buf[100];
		WinSocket Client;
		cout << "Socket has created!" << endl;
		
		Client.Connect("192.168.1.41", 8888);
		cout << "Connecting!" << endl;
	
		int l = 0;
		while(l == 0)
			l = Client.Receive(buf, 100);
		buf[l] = 0;
		cout << buf << endl;

	}
	catch(WinSocketException e)
	{
		wcout << e.where() << ":" << e.why() << ":" << e.what() << endl;
	}
	catch(WinException e)
	{
		wcout << e.where() << ":" << e.why() << ":" << e.what() << endl;
	}

	cin.get();

	return 0;
}


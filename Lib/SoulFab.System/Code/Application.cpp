#include "Application.hpp"

#include "Thread.hpp"
#include "OS.hpp"

#include <conio.h>

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::System;

namespace SoulFab::System
{
	int BaseApplication::WaitKey(int key)
	{
		int ret = 0;

		while (true)
		{
			while (!_kbhit())
			{
				Sleep(100);
			}

			ret = _getch();
			if (key == 0 || key == ret) break;
		}

		return ret;
	}


	void BaseApplication::Initialize()
	{
		// OSException::Install();
		// BaseSocket::Startup();
	}

	void BaseApplication::Uninitialize()
	{
		// BaseSocket::Cleanup();
	}

	BaseApplication::BaseApplication()
		: LogSource("Application")
	{
	}

	ILogger* BaseApplication::CreateLogger()
	{
		return nullptr;
	}

	void BaseApplication::Main(int argc, char* args[])
	{
		this->RootPath = OS::GetApplicationRootPath();

		ILogger* SysLogger = this->CreateLogger();
		setLogger(SysLogger);

		this->MessageLog(u8"系统启动 Starting from: %s", this->RootPath.Pointer());

        Thread::setThreadContext(this);
        Thread::ExceptionWrap(*this, this, &BaseApplication::MainProcess);

		SafeDestroy(SysLogger);
	}

    void BaseApplication::MainProcess()
    {

        this->Initialize();

        try
        {
            this->Create();
            this->Run();
        }
        catch (...)
        {
            try
            {
                this->Destroy();
            }
            catch (...)
            {

            }

            throw;
        }
        this->Destroy();

        this->Uninitialize();
    }

}

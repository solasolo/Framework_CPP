#include "Thread.hpp"

#include <chrono>
#include <system_error>

#include "CommonDef.hpp"
#include "Exception.hpp"

using namespace std;
using namespace std::chrono;
using namespace SoulFab::Base;

namespace SoulFab::System
{
	IThreadContext* Thread::ThreadContext = nullptr;

	void Thread::setThreadContext(IThreadContext* handle)
	{
		ThreadContext = handle;
	}

	void Thread::Initialize()
	{
		if (ThreadContext)
		{
			ThreadContext->Initialize();
		}
	}

	void Thread::Uninitialize()
	{
		if (ThreadContext)
		{
			ThreadContext->Uninitialize();
		}
	}

    Thread::Thread(const String name, bool suspend)
        : LogSource(name), IsRuning(false), IsShutdown(false), _Thread(nullptr), SleepTimer(100)
    {
		if(!suspend)
		{
			this->Start();
		}
	}

	Thread::~Thread()
	{
		this->IsRuning = false;
	}

	void Thread::Start()
	{
        if (!IsRuning)
        {
            this->IsRuning = true;

            this->_Thread = new thread(Thread::ThreadProcess, this);
            this->_Thread->detach();
        }
	}

	void Thread::Shutdown()
	{
		if (this->IsRuning)
		{
			this->IsRuning = false;
			WaitForShutdown();

			SafeDestroy(this->_Thread);
		}
	}

	void Thread::WaitForShutdown()
	{
		while (!this->IsShutdown)
		{
			if (this->QuitEvent.Wait(300)) break;
		}
	}

	void Thread::ThreadLoop()
	{
		this->Log(EventType::Debug, "Thread [" + this->getLogName() + "] Started");

		this->Begin();

		while (this->IsRuning)
		{
            this->DoTask();

			if (this->SleepTimer > 0)
			{
				this_thread::sleep_for(this->SleepTimer * 1ms);
			}
		}

		this->End();

		this->QuitEvent.Set();
		this->IsShutdown = true;

		this->Log(EventType::Debug, String("Thread [") + this->getLogName() + "] Quit");
	}

    void Thread::DoTask()
    {
        Thread::ExceptionWrap(*this, this, &Thread::Task);
    }

	void Thread::ThreadProcess(Thread* me)
	{
		Initialize();

		Thread::ExceptionWrap(*me, me, &Thread::ThreadLoop);

		Uninitialize();
	}
}

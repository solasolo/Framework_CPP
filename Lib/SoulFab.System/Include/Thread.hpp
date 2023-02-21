#pragma once

#ifndef _SF_THREAD_H_
#define _SF_THREAD_H_

#include <string>
#include <thread>
#include <chrono>
#include <functional>

#include "BaseEvent.hpp"
#include "CommonDef.hpp"
#include "Synchronization.hpp"
#include "String.hpp"

namespace SoulFab::System
{
	class IThreadContext
	{
	public:
		virtual void Initialize() = 0;
		virtual void Uninitialize() = 0;
	};

	class SHARED_EXPORT Thread : public SoulFab::Base::LogSource
	{
	public:
        template<class T>
        using ThreadTask = void(T::*)();

		static void setThreadContext(IThreadContext* handle);

        template<class T> static void ExceptionWrap(SoulFab::Base::LogSource& logger, T* obj, ThreadTask<T> fn);

	public:
		Thread(const SoulFab::Base::String name, bool suspend = true);
        ~Thread();

		virtual void Start();
		virtual void Shutdown();

		void WaitForShutdown();

	private:
		static IThreadContext* ThreadContext;

		static void ThreadProcess(Thread* param);
		static void Initialize();
		static void Uninitialize();

	private:
		bool IsRuning;
		bool IsShutdown;
		std::thread* _Thread;
		AutoEvent QuitEvent;

		void ThreadLoop();

	protected:
		int SleepTimer;

        virtual void DoTask();

		virtual void Begin() {}
		virtual void End() {}
        virtual void Task() {};
	};

    template<class T>
	class SimpleThread : public Thread
	{
	public:
        SimpleThread(const SoulFab::Base::String name, T* obj, ThreadTask<T> task, int sleep = 10);

    private:
        T* ExternalObject;
        ThreadTask<T> ExternalTask;

        void DoTask() override;
    };

    template<typename T>
    void Thread::ExceptionWrap(SoulFab::Base::LogSource& logger, T* obj, void(T::*fn)())
    {
        try
        {
            (obj->*fn)();
        }
        catch (SoulFab::Base::Exception& ex)
        {
            logger.ErrorLog(ex, "Thread [" + logger.getLogName() + "]");
        }
        catch (std::system_error& ex)
        {
            logger.ErrorLog(ex, "Thread [" + logger.getLogName() + "]");
        }
        catch (std::exception& ex)
        {
            logger.ErrorLog(ex, "Thread [" + logger.getLogName() + "]");
        }
        catch (...)
        {
            logger.Log(EventType::Error, "Thread [" + logger.getLogName() + "] Unknown Exception");
        }
    }


    template<class T>
    SimpleThread<T>::SimpleThread(const SoulFab::Base::String name, T* obj, ThreadTask<T> task, int sleep)
        : ExternalObject(obj), ExternalTask(task), Thread(name, true)
    {
        this->SleepTimer = sleep;
    }

    template<class T>
    void SimpleThread<T>::DoTask()
    {
        Thread::ExceptionWrap(*this->ExternalObject, this->ExternalObject, this->ExternalTask);
    }
}

#endif

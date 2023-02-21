#include "ThreadLogger.hpp"

#include "CommonDef.hpp"
#include "Synchronization.hpp"

#include <atomic>

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::System;

namespace SoulFab::Logger
{
    ThreadLogger::ThreadLogger()
		: BaseLogger(), Thread("ThreadLogger"),  MessageQueue(nullptr)
	{
		this->MessageQueue = new list<LogMessage>();
		this->SleepTimer = 0;

        this->Start();
	}

	ThreadLogger::~ThreadLogger()
	{
		if(this->MessageQueue)
		{
			this->WriteAll(*this->MessageQueue);
		}

		this->Shutdown();

		SafeDestroy(this->MessageQueue);
	}

	void ThreadLogger::Clean(const DateTime& last_time)
	{
		for(auto logger : this->LoggerList)
		{
			logger->Clean(last_time);
		}
	}

	void ThreadLogger::WriteAll(const list<LogMessage>& msg_list)
	{
		for(auto &logger : this->LoggerList)
		{
			logger->BatchLog(msg_list);
		}
	}

	void ThreadLogger::Task()
	{
		list<LogMessage>* OldBuffer;

		this->LogEvent.Wait(300);
		// if(this->LogEvent.Wait(300))
		{
			LockerGuard LG(this->BufferGuard);

			OldBuffer = this->MessageQueue;
			this->MessageQueue = new list<LogMessage>();

			LG.Close();

			if(OldBuffer)
			{
				LogMessage Msg;

				if(OldBuffer->size() > 0)
				{
					this->WriteAll(*OldBuffer);
				}

				delete OldBuffer;
			}
		}
	}

	void ThreadLogger::AddLogger(ILogWriter* obj)
	{
		LoggerList.push_back(obj);
	}

	void ThreadLogger::CleanLoggerList()
	{
		this->LoggerList.clear();
	}

	void ThreadLogger::Write(const DateTime& time, EventType type, const String& message)
	{
		LockerGuard LG(this->BufferGuard);

		if(this->MessageQueue)
		{
			LogMessage Msg;

			Msg.Time = time;
			Msg.Type = type;
			Msg.Message = message;

			this->MessageQueue->push_back(Msg);

			this->LogEvent.Set();
		}
	}
}

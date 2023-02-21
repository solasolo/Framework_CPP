#pragma once

#ifndef _SF_THREAD_LOGGER_H_
#define _SF_THREAD_LOGGER_H_

#include <list>
#include <vector>

#include "BaseLogger.hpp"
#include "Thread.hpp"
#include "Synchronization.hpp"

namespace SoulFab::Logger
{
    class SHARED_EXPORT ThreadLogger : public BaseLogger, public SoulFab::System::Thread
	{
	public:
		ThreadLogger();
		virtual ~ThreadLogger();

		void AddLogger(ILogWriter* obj);
		void CleanLoggerList();

		virtual void Clean(const SoulFab::Base::DateTime& last_time) override;

	private:
		std::vector<ILogWriter*> LoggerList;
		std::list<LogMessage>* MessageQueue;
		
		SoulFab::System::CriticalSection BufferGuard;
		SoulFab::System::AutoEvent LogEvent;

		void WriteAll(const std::list<LogMessage>& msg_list);

	protected:
		void Task() override;
		void Write(const SoulFab::Base::DateTime& time, SoulFab::Base::EventType Type, const SoulFab::Base::String& Message) override;
	};
}

#endif
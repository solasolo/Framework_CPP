#pragma once

#ifndef _SF_BASE_LOGGER_H_
#define _SF_BASE_LOGGER_H_

#include <chrono>
#include <string>
#include <list>

#include "BaseEvent.hpp"
#include "DateTime.hpp"
#include "Synchronization.hpp"

namespace SoulFab::Logger
{
	const int EVENT_LOGGER_BUFFER_SIZE = 4095;

	struct LogMessage
	{
		SoulFab::Base::DateTime Time;
		SoulFab::Base::EventType Type;
		SoulFab::Base::String Message;
	};

	class ILogWriter : public SoulFab::Base::ILogger
	{
	public:
		virtual void Clean(const SoulFab::Base::DateTime& last_time) = 0;
		virtual void BatchLog(const std::list<LogMessage>& msgs) noexcept = 0;
	};

    class BaseLogger : public ILogWriter
    {
	public: 
		thread_local static char EventLoggerBuffer[EVENT_LOGGER_BUFFER_SIZE + 1];

	public:
		static std::string FormatDate(const SoulFab::Base::DateTime& t);
		static std::string FormatTime(const SoulFab::Base::DateTime& t);
		static std::string& getTypeString(SoulFab::Base::EventType Type);

		virtual ~BaseLogger() {};

		void Log(SoulFab::Base::EventType Type, const char* fmt, ...) noexcept;

		virtual void Log(SoulFab::Base::EventType Type, const SoulFab::Base::String& Message) noexcept override;
		virtual void BatchLog(const std::list<LogMessage>& msgs) noexcept override;
	
	private:
		static std::string EventTypeString[];

    protected:
		struct TimeStamp
		{
			time_t T;
			int MT;
		};

		SoulFab::System::CriticalSection Gate;

		BaseLogger();

		virtual void Write(const SoulFab::Base::DateTime& time, SoulFab::Base::EventType Type, const SoulFab::Base::String& Message) = 0;
    };

}

#endif
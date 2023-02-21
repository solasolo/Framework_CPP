#include "BaseLogger.hpp"

#include <stdarg.h>

using namespace std;
using namespace SoulFab::Base;

namespace SoulFab::Logger
{
	thread_local char BaseLogger::EventLoggerBuffer[EVENT_LOGGER_BUFFER_SIZE + 1];

	string BaseLogger::EventTypeString[] =
	{
		string("-WDG"),
		string("-RCD"),
		string("-MSG"),
		string("-WRN"),
		string("-ERR"),
		string("-DBG"),
		string("-NTF"),
		string("-TRC")
	};

    BaseLogger::BaseLogger()
	{
	}

	string BaseLogger::FormatDate(const DateTime& t)
	{
		string ret;

		DateStruct ds = t.getDateStruct();

		char buf[20];
        buf[19] = '\x0';

		sprintf_s(buf, 19, "%04d-%02d-%02d", ds.Year, ds.Month, ds.Day);
        ret.append(buf);

		return ret;
	}

	string BaseLogger::FormatTime(const DateTime& t)
	{
		string ret;

		TimeStruct ts = t.getTimeStruct();

		char buf[20];
        buf[19] = '\x0';

		sprintf_s(buf, 19, "%02d:%02d:%02d.%03d", ts.Hour, ts.Minute, ts.Second, ts.Millisecond);
        ret.append(buf);

		return ret;
	}

	string& BaseLogger::getTypeString(EventType Type)
	{	
		if(Type >=  EventType::COUNT)
		{
            Type = EventType::Debug;
		}

        return EventTypeString[(int)Type];
	}

	void BaseLogger::Log(EventType Type, const String& Message) noexcept
	{
		DateTime time = DateTime::Now();

		try
		{
			this->Write(time, Type, Message);
		}
		catch(...)
		{
		}
	}

	inline void BaseLogger::Log(EventType Type, const char* fmt, ...) noexcept
	{
		char* buf = EventLoggerBuffer;

		va_list args;
		va_start(args, fmt);

		vsnprintf(buf, EVENT_LOGGER_BUFFER_SIZE, fmt, args);

		String Message = buf;
		this->Log(Type, Message);
	}

	void BaseLogger::BatchLog(const list<LogMessage>& msg_list) noexcept
	{
		try
		{
			for(auto msg : msg_list)
			{
				this->Write(msg.Time, msg.Type, msg.Message);
			}
		}
		catch(...)
		{
		}
	}
}
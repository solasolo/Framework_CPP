
#pragma once

#ifndef _SF__BASE_EVENT_H_
#define _SF__BASE_EVENT_H_

#include <string>
#include <exception>

#include "CommonDef.hpp"
#include "Exception.hpp"
#include "String.hpp"

namespace SoulFab::Base
{
	enum class EventType
	{
		Watchdog = 0,
		Record,
		Message,
		Warning,
		Error,
		Debug,
		Notify,
		Trace,

		COUNT
	};


	class ILogger
	{
	public:
		virtual void Log(EventType Type, const String& Message) noexcept = 0;

		virtual ~ILogger() {}
	};

	//
	//
	//

	class SHARED_EXPORT Logger
	{
	public:
		Logger();
		Logger(const SoulFab::Base::ILogger* logger);

		virtual void setLogger(const SoulFab::Base::ILogger* callback);

        template<typename ...A>	void DebugLog(const char* fmt, A ...args) noexcept;
		template<typename ...A>	void InfoLog(const char* fmt, A ...args) noexcept;
		template<typename ...A>	void ErrorLog(const char* fmt, A ...args) noexcept;
		template<typename ...A>	void WarningLog(const char* fmt, A ...args) noexcept;
		template<typename ...A>	void MessageLog(const char* fmt, A ...args) noexcept;
        template<typename ...A>	void NotifyLog(const char* fmt, A ...args) noexcept;

        template<typename ...A>	void DebugLog(const char8_t* fmt, A ...args) noexcept;
		template<typename ...A>	void InfoLog(const char8_t* fmt, A ...args) noexcept;
		template<typename ...A>	void ErrorLog(const char8_t* fmt, A ...args) noexcept;
		template<typename ...A>	void WarningLog(const char8_t* fmt, A ...args) noexcept;
		template<typename ...A>	void MessageLog(const char8_t* fmt, A ...args) noexcept;
        template<typename ...A>	void NotifyLog(const char8_t* fmt, A ...args) noexcept;

        void RecordLog(const SoulFab::Base::String& str) noexcept;
        template<class T> void ErrorLog(const T& ex, const SoulFab::Base::String& place = "") noexcept;

		void Log(EventType Type, const String& Message) noexcept;
		template<typename ...A> void Log(EventType Type, const char* fmt, A... args) noexcept;

	protected:
		SoulFab::Base::ILogger* InnerLogger;
	};


    template<typename ...A>	void Logger::NotifyLog(const char* fmt, A ...args) noexcept
    {
        this->Log(EventType::Notify, fmt, args...);
    }

	template<typename ...A>
	inline void  Logger::MessageLog(const char* fmt, A... args) noexcept
	{
		this->Log(EventType::Message, fmt, args...);
	}

	template<typename ...A>
	inline void  Logger::WarningLog(const char* fmt, A... args) noexcept
	{
		this->Log(EventType::Warning, fmt, args...);
	}

	template<typename ...A>
	inline void  Logger::DebugLog(const char* fmt, A... args) noexcept
	{
		this->Log(EventType::Debug, fmt, args...);
	}

	template<typename ...A>
	inline void  Logger::ErrorLog(const char* fmt, A... args) noexcept
	{
		this->Log(EventType::Error, fmt, args...);
	}


    template<typename ...A>	void Logger::NotifyLog(const char8_t* fmt, A ...args) noexcept
    {
        this->Log(EventType::Notify, fmt, args...);
    }

	template<typename ...A>
	inline void  Logger::MessageLog(const char8_t* fmt, A... args) noexcept
	{
		this->Log(EventType::Message, reinterpret_cast<const char*>(fmt), args...);
	}

	template<typename ...A>
	inline void  Logger::WarningLog(const char8_t* fmt, A... args) noexcept
	{
		this->Log(EventType::Warning, reinterpret_cast<const char*>(fmt), args...);
	}

	template<typename ...A>
	inline void  Logger::DebugLog(const char8_t* fmt, A... args) noexcept
	{
		this->Log(EventType::Debug, reinterpret_cast<const char*>(fmt), args...);
	}

	template<typename ...A>
	inline void  Logger::ErrorLog(const char8_t* fmt, A... args) noexcept
	{
		this->Log(EventType::Error, reinterpret_cast<const char*>(fmt), args...);
	}

	template<class T>
	inline void  Logger::ErrorLog(const T& ex, const SoulFab::Base::String& place) noexcept
	{
		if (this->InnerLogger)
		{
			this->InnerLogger->Log(EventType::Error, Exception::Format(place, ex));
		}
	}

	template<typename ...A>
	inline void Logger::Log(EventType Type, const char* fmt, A... args) noexcept
	{
        this->InnerLogger->Log(Type, String::Format(fmt, args...));
	}

    inline void Logger::RecordLog(const SoulFab::Base::String& str) noexcept
    {
        this->InnerLogger->Log(EventType::Record, str);
    }

    inline void Logger::Log(EventType Type, const String& Message) noexcept
    {
        if (this->InnerLogger)
        {
            this->InnerLogger->Log(Type, Message);
        }
    }

	//
	//
	//

	class SHARED_EXPORT LogSource : public Logger
	{
	public:
		LogSource(const SoulFab::Base::String& name);
        String getLogName();

	protected:
        SoulFab::Base::String LogName;
	};


}

#endif

#pragma once

#ifndef _SF_CONSOLE_LOGGER_H_
#define _SF_CONSOLE_LOGGER_H_

#include "BaseLogger.hpp"

namespace SoulFab::Logger
{
	class SHARED_EXPORT ConsoleLogger : public BaseLogger
	{
		enum class ConsoleColor
		{
			Default = 0,
			Black,
			Red,
			Green,
			Yellow,
			Blue,
			Magenta,
			Cyan,
            White,
            Gray,
			BrightRed,
			BrightGreen,
			BrightYellow,
			BrightBlue,
			BrightMagenta,
			BrightCyan,
			BrightWhite,
		};

	public:
		static bool UTF8;

	public:
		ConsoleLogger();
		virtual ~ConsoleLogger();

        virtual void Clean(const SoulFab::Base::DateTime& last_time) override {}

    private:
        static const char* ForeColorCommand[]; 

    private:
        void SetColor(ConsoleColor color);

	protected:
		virtual void Write(const SoulFab::Base::DateTime& time, SoulFab::Base::EventType type, const SoulFab::Base::String& message) override;
	};
}

#endif

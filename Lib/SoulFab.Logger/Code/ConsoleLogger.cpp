#include "ConsoleLogger.hpp"

#include <iostream>

#include "StringHelper.hpp"

using namespace std;
using namespace SoulFab::Base;

namespace SoulFab::Logger
{
    const char* ConsoleLogger::ForeColorCommand[] = 
    {
		"\x1b[39m",
		"\x1b[30m",
		"\x1b[31m",
		"\x1b[32m",
		"\x1b[33m",
		"\x1b[34m",
		"\x1b[35m",
		"\x1b[36m",
		"\x1b[37m",
		"\x1b[90m",
		"\x1b[91m",
		"\x1b[92m",
		"\x1b[93m",
		"\x1b[94m",
		"\x1b[95m",
		"\x1b[96m",
		"\x1b[97m",
	};

	bool ConsoleLogger::UTF8 = false;

    ConsoleLogger::ConsoleLogger(void)
		: BaseLogger()
	{
	}


	ConsoleLogger::~ConsoleLogger(void)
	{
	}

	void ConsoleLogger::Write(const DateTime& time, EventType type, const String& message)
	{
		string sTime;
		sTime = BaseLogger::FormatTime(time);
		ConsoleColor color;

		string sType = this->getTypeString(type);
		switch(type)
		{
		case EventType::Watchdog:
			color = ConsoleColor::Magenta;
			break;

		case EventType::Record:
			color = ConsoleColor::Cyan;
			break;

		case EventType::Message:
			color = ConsoleColor::Gray;
			break;

		case EventType::Warning:
			color = ConsoleColor::Yellow;
			break;

		case EventType::Error:
			color = ConsoleColor::Red;
			break;

		case EventType::Debug:
			color = ConsoleColor::Green;
			break;

		case EventType::Trace:
			color = ConsoleColor::BrightGreen;
			break;

        case EventType::Notify:
            color = ConsoleColor::White;
            break;

        default:
			color = ConsoleColor::Default;
		}
		

        if (type != EventType::Record)
        {
            this->SetColor(ConsoleColor::Gray);
            cout << sTime;
            this->SetColor(color);
            cout << " " << sType << "   ";
        }
        else
        {
            this->SetColor(color);
        }

		if (UTF8)
		{
            cout << message.Pointer() << endl;
		}
		else
		{
			string GBKString = StringHelper::UTF82GBK(message, BaseLogger::EventLoggerBuffer);

            cout << GBKString << endl;
		}
		
		this->SetColor(ConsoleColor::Default);
	}

    void ConsoleLogger::SetColor(ConsoleColor color)
    {
        cout << ForeColorCommand[(int)color];
    }

}

#pragma once

#ifndef _SF_FILE_LOGGER_H_
#define _SF_FILE_LOGGER_H_

#include "BaseLogger.hpp"

namespace SoulFab::Logger
{
	class BaseFileLogger: public BaseLogger
	{
	public:
		static void CleanLogger(const SoulFab::Base::String path, const std::string prefix, time_t last_time);

	public:
		BaseFileLogger() {};
		BaseFileLogger(const SoulFab::Base::String path, const std::string prefix);
		~BaseFileLogger();


		virtual void Clean(const SoulFab::Base::DateTime& last_time) override;

	protected:
		std::ofstream* LogFile;//原先私有
		std::string LastFileName;//原先私有

		SoulFab::Base::String Path;
		SoulFab::Base::String Prefix;

		std::string MakeFileName(const SoulFab::Base::String& prefix, const SoulFab::Base::String& DateStr);

		void OpenLog(const std::string file);
		void OutputLog(const SoulFab::Base::DateTime& time, SoulFab::Base::EventType Type, const SoulFab::Base::String& Message);
		void CloseLog();

		virtual void Write(const SoulFab::Base::DateTime& time, SoulFab::Base::EventType Type, const SoulFab::Base::String& Message) override;
	};

	class SHARED_EXPORT FileLogger : public BaseFileLogger
	{
	public:
		FileLogger(const std::string path, const std::string prefix);
		virtual ~FileLogger();

		virtual void BatchLog(const std::list<LogMessage>& msg_list) noexcept override;
	};
}

#endif
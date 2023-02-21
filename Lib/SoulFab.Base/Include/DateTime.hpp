#pragma once

#ifndef _SF_DATE_TIME_H_
#define _SF_DATE_TIME_H_

#include <chrono>

#include "CommonDef.hpp"

namespace SoulFab::Base
{
	struct SHARED_EXPORT TimeStruct
	{
		int Hour;
		int Minute;
		int Second;
		int Millisecond;
	};

	struct SHARED_EXPORT DateStruct
	{
		int Year;
		int Month;
		int Day;
	};

	class SHARED_EXPORT DateTime
	{
	public: 
		DateTime();
		DateTime(double value);

		operator double();

		std::string CTS() const;
		std::string ISOString() const;
		std::string LocalCTS() const;

		DateStruct getDateStruct() const;
		TimeStruct getTimeStruct() const;
	
	private:
		std::chrono::system_clock::time_point TimeValue;

	public:
		static DateTime Now();
	};

}

#endif 
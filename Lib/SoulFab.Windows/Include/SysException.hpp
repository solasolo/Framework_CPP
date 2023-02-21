#pragma once

#ifndef _SF_SYS_EXCEPTION_H_
#define _SF_SYS_EXCEPTION_H_

#include <string>
#include "Exception.hpp"

namespace SoulFab::System
{
	class _declspec(dllexport) OSException : public SoulFab::Base::Exception
	{
	public:	
		static void Install();
		
		OSException(unsigned long code, const SoulFab::Base::String message);
	};

	class _declspec(dllexport) SysException : public SoulFab::Base::Exception
	{
	public:
		static void Check(const SoulFab::Base::String place);
		static void Check(unsigned long hr, const SoulFab::Base::String place);
		static void COMCheck(long hr, const SoulFab::Base::String place);

        static SoulFab::Base::String SysErrorDescription(int Code);

		SysException(const SoulFab::Base::String place);
		SysException(unsigned long code, const SoulFab::Base::String place);
		SysException(const SysException& ex) noexcept;

		SysException& operator =(const SysException& ex) noexcept;

	protected:
		void SetErrorCode(unsigned long code);

		const SoulFab::Base::String GetErrorDescription() const noexcept override;
	};
}

#endif

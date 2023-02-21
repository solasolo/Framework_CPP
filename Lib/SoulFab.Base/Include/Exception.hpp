#pragma once

#ifndef _SF_EXCEPTION_H_
#define _SF_EXCEPTION_H_

#include <string>
#include <exception>
#include <system_error>

#include "CommonDef.hpp"
#include "String.hpp"

namespace SoulFab::Base
{
	class SHARED_EXPORT Exception
	{
	public:
		Exception();
		Exception(const String& msg, const String place = String(""), int code = 0);
		Exception(const Exception& ex);
				
		virtual const String GetDescription() const noexcept;

	public:
		static String Format(const String& context, const Exception& ex);
		static String Format(const String& context, const std::exception& ex);
		static String Format(const String& context, const std::system_error& ex);

	protected:
		int Code;
		String Where;
		String Message;

		virtual const String GetErrorDescription() const;

	};

    class ChannelException : public Exception
    {

    };

}

#endif // _EXCEPTION_H_

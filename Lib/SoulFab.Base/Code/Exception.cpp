#include "Exception.hpp"

using namespace std;

namespace SoulFab::Base
{
    Exception::Exception()
        :  Code(0), Where(""), Message("")
    {
    }

	Exception::Exception(const String& msg, const String place, int code)
        : Code(code), Where(place), Message(msg)
    {
    }

    Exception::Exception(const Exception& ex)
    {
        this->Message = ex.Message;
        this->Where = ex.Where;
        this->Code = ex.Code;
    }

    const String Exception::GetErrorDescription() const
    {
        return this->Message;
    }

    const String Exception::GetDescription() const noexcept
    {
        String ErrDescription;

        ErrDescription = this->Where.Empty() ? GetErrorDescription() : Exception::Format(this->Where, *this);

        return ErrDescription;
    }


    String Exception::Format(const String& context, const Exception& ex)
    {
        return String::Format("[%s]%s", context.Pointer(), ex.GetErrorDescription().Pointer());
    }

    String Exception::Format(const String& context, const exception& ex)
    {
        return String::Format("[%s]%s", context.Pointer(), ex.what());
    }

    String Exception::Format(const String& context, const system_error& ex)
    {
		return String::Format("[%s]%s", context.Pointer(), ex.what());
    }

}
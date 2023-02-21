#include "SysException.hpp"

#include <windows.h>

using namespace std;
using namespace SoulFab::Base;

namespace SoulFab::System
{
	string SEMessage(unsigned int Code, EXCEPTION_POINTERS* se)
	{
		void* ExecuteAddress = se->ExceptionRecord->ExceptionAddress;
		void* DataAddress = NULL;

		Code = se->ExceptionRecord->ExceptionCode;
		string TypeString;

		switch(Code)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			TypeString = "EXCEPTION_ACCESS_VIOLATION";
			DataAddress = (void*)se->ExceptionRecord->ExceptionInformation[1];
			break;

		case EXCEPTION_DATATYPE_MISALIGNMENT:     
			TypeString = "EXCEPTION_DATATYPE_MISALIGNMENT";
			break;

		case EXCEPTION_BREAKPOINT:                
			TypeString = "EXCEPTION_BREAKPOINT";
			break;

		case EXCEPTION_SINGLE_STEP:               
			TypeString = "EXCEPTION_SINGLE_STEP";
			break;

		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:     
			TypeString = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
			break;

		case EXCEPTION_FLT_DENORMAL_OPERAND:      
			TypeString = "EXCEPTION_FLT_DENORMAL_OPERAND";
			break;

		case EXCEPTION_FLT_DIVIDE_BY_ZERO:        
			TypeString = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
			break;

		case EXCEPTION_FLT_INEXACT_RESULT:       
			TypeString = "EXCEPTION_FLT_INEXACT_RESULT";
			break;

		case EXCEPTION_FLT_INVALID_OPERATION:     
			TypeString = "EXCEPTION_FLT_INVALID_OPERATION";
			break;

		case EXCEPTION_FLT_OVERFLOW:              
			TypeString = "EXCEPTION_FLT_OVERFLOW";
			break;

		case EXCEPTION_FLT_STACK_CHECK:           
			TypeString = "EXCEPTION_FLT_STACK_CHECK";
			break;

		case EXCEPTION_FLT_UNDERFLOW:             
			TypeString = "EXCEPTION_FLT_UNDERFLOW";
			break;

		case EXCEPTION_INT_DIVIDE_BY_ZERO:        
			TypeString = "EXCEPTION_INT_DIVIDE_BY_ZERO";
			break;

		case EXCEPTION_INT_OVERFLOW:              
			TypeString = "EXCEPTION_INT_OVERFLOW";
			break;

		case EXCEPTION_PRIV_INSTRUCTION:          
			TypeString = "EXCEPTION_PRIV_INSTRUCTION";
			break;

		case EXCEPTION_IN_PAGE_ERROR:             
			TypeString = "EXCEPTION_IN_PAGE_ERROR";
			break;

		case EXCEPTION_ILLEGAL_INSTRUCTION:       
			TypeString = "EXCEPTION_ILLEGAL_INSTRUCTION";
			break;

		case EXCEPTION_NONCONTINUABLE_EXCEPTION:  
			TypeString = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
			break;

		case EXCEPTION_STACK_OVERFLOW:            
			TypeString = "EXCEPTION_STACK_OVERFLOW";
			break;

		case EXCEPTION_INVALID_DISPOSITION:       
			TypeString = "EXCEPTION_INVALID_DISPOSITION";
			break;

		case EXCEPTION_GUARD_PAGE:                
			TypeString = "EXCEPTION_GUARD_PAGE";
			break;

		case EXCEPTION_INVALID_HANDLE:            
			TypeString = "EXCEPTION_INVALID_HANDLE";
			break;

		case CONTROL_C_EXIT:                      
			TypeString = "CONTROL_C_EXIT";
			break;

		default:
			TypeString = "EXCEPTION_UNKNOWN";
		}

		char buf[100];
		buf[99] = L'\x0';

		if(DataAddress)
		{
			sprintf_s(buf, 99, "%S @ 0x%p R/W: 0x%p", TypeString.c_str(), ExecuteAddress, DataAddress);
		}
		else
		{
			sprintf_s(buf, 99, "%S @ 0x%p", TypeString.c_str(), ExecuteAddress);
		}
		
		return buf;
	}

	void SEHandle(unsigned int Code, EXCEPTION_POINTERS* SE)
	{
		string message = SEMessage(Code, SE);
		throw OSException(Code, message);
	}

	void OSException::Install()
	{
		_set_se_translator(SEHandle);
	}

	OSException::OSException(unsigned long code, const String message)
	{
		Code = code;
		Message = message;
	}

    //
    //
    //

    String SysException::SysErrorDescription(int code)
    {
        String ErrString;
        LPVOID lpMsgBuf = NULL;

        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

        if (lpMsgBuf)
        {
            ErrString = (char*)lpMsgBuf;
            LocalFree(lpMsgBuf);
        }
        else
        {
            ErrString = "No Error Description!";
        }

        return ErrString;
    }

	SysException::SysException(const String place)
	{
		Where = place;
		SetErrorCode(GetLastError());
	}

	SysException::SysException(unsigned long code, const String place)
	{
		Where = place;
		SetErrorCode(code);
	}

	SysException::SysException(const SysException& ex) throw()
		: Exception(ex)
	{
	}

	SysException& SysException::operator =(const SysException& ex) throw()
	{
		this->Code = ex.Code;
		this->Where = ex.Where;
		this->Message = ex.Message;

		return *this;
	}

	void SysException::SetErrorCode(unsigned long code)
	{
		this->Code = code;
		this->Message = this->GetErrorDescription();
	}

	const String SysException::GetErrorDescription() const noexcept
	{
        return SysErrorDescription(this->Code);
	}

	void SysException::Check(const String place)
	{
		DWORD ECode;

		ECode = GetLastError();
		if (ECode != 0)
		{
			throw SysException(ECode, place);
		}
	}

	void SysException::Check(unsigned long hr, const String place)
	{
		DWORD ECode;

		ECode = hr;
		if (ECode != 0)
		{
			throw SysException(ECode, place);
		}
	}

	void SysException::COMCheck(HRESULT hr, const String place)
	{
		if (hr < 0)
		{
			throw SysException(hr, place);
		}
	}
}

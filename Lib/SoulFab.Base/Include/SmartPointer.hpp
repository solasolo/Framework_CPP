#pragma once

#ifndef _SF_SMART_POINTER_H_
#define _SF_SMART_POINTER_H_

#include <atomic>
#include <cassert>

#include "CommonDef.hpp"
#include "Exception.hpp"

namespace SoulFab::Base 
{
	template<class T>
	class SmartPointer
	{
	public:
		SmartPointer();	
		SmartPointer(T* ptr);	
		SmartPointer(const SmartPointer<T>& sp);

		~SmartPointer();	

		T* get();

		SmartPointer<T>& operator =(const std::nullptr_t nil);
		SmartPointer<T>& operator =(const SmartPointer<T>& sp);
		T* operator ->();
		operator bool();
		bool operator !();	
		T& operator *();
		T* operator &();

	private:
		T* ObjectRef;
		int* Counter;

		void Check();

		void _AddRef();
		void _Release();
	};

	class SHARED_EXPORT VariantPointer
	{
	public:
		VariantPointer();
		template<class T> VariantPointer(const T* ptr);
		VariantPointer(const VariantPointer& sp);

		~VariantPointer();

		operator bool();
		VariantPointer& operator =(const VariantPointer& sp);
		template<class T> VariantPointer& operator =(const T* ptr);
		template<class T> VariantPointer& Assign(const VariantPointer& sp);
		template<class T> T* get();

	private:
		void* ObjectRef;
		int* Counter;

		void _AddRef();
		template<class T> inline void _Release();

		void Check();
	};

#pragma region SmartPointer

	template<class T>
	inline void SmartPointer<T>::_AddRef()
	{
		if(this->Counter)
		{
			(*this->Counter)++;
		}
	}

	// InnerRef meybe invalid after release, shuld be asign a new value
	template<class T>
	inline void SmartPointer<T>::_Release()
	{
		if(this->Counter)
		{
			(*this->Counter)--;
			if (*this->Counter == 0)
			{
				delete this->Counter;
				if (this->ObjectRef) delete this->ObjectRef;
			}
		}

		this->Counter = nullptr;
		this->ObjectRef = nullptr;
	}

	template<class T>
	SmartPointer<T>::SmartPointer()
		: ObjectRef(nullptr), Counter(nullptr)
	{
	}

	template<class T>
	SmartPointer<T>::SmartPointer(T* ptr)
		: ObjectRef(nullptr), Counter(nullptr)
	{
		if(ptr)
		{
			this->Counter = new int(1);
			this->ObjectRef = ptr;
		}
	}

	template<class T>
	SmartPointer<T>::SmartPointer(const SmartPointer<T>& sp)
	{
		this->ObjectRef = sp.ObjectRef;
		this->Counter = sp.Counter;

		this->_AddRef();
	}

	template<class T>
	SmartPointer<T>::~SmartPointer()
	{
		this->_Release();
	}

	template<class T>
	inline SmartPointer<T>& SmartPointer<T>::operator =(const std::nullptr_t nil)
	{
		this->_Release();

		return *this;
	}

	template<class T>
	inline SmartPointer<T>& SmartPointer<T>::operator =(const SmartPointer<T>& sp)
	{
		if (sp.ObjectRef != this->ObjectRef)
		{
			this->_Release();

			this->Counter = sp.Counter;
			this->ObjectRef = sp.ObjectRef;
			this->_AddRef();
		}

		return *this;
	}

	template<class T>
	inline T* SmartPointer<T>::get()
	{
		return &(*this);
	}

	template<class T>
	inline T* SmartPointer<T>::operator ->()
	{
		this->Check();
		
		return this- ObjectRef;
	}

	template<class T>
	inline SmartPointer<T>::operator bool()
	{
		return (this->ObjectRef && this->Counter);
	}

	template<class T>
	inline bool SmartPointer<T>::operator !()
	{
		return !(*this);
	}

	template<class T>
	inline T* SmartPointer<T>::operator &()
	{
		T* ret = nullptr;

		if(*this)
		{
			ret = this->ObjectRef;
		}

		return ret;
	}

	template<class T>
	inline T& SmartPointer<T>::operator *()
	{
		this->Check();

		return *this->ObjectRef;
	}

	template<class T>
	inline void SmartPointer<T>::Check()
	{
		if (!this->Counter)
		{
			throw Exception("Uninitialized Smart Poiner");
		}

		if (!this->ObjectRef)
		{
			throw Exception("Null Smart Poiner");
		}
	}

#pragma endregion


#pragma region VariantPointer

	template<class T> 
	inline void VariantPointer::_Release()
	{
		if (this->Counter)
		{
			(*this->Counter)--;
			if (*this->Counter == 0)
			{
				delete this->Counter;
				if(this->ObjectRef) delete (T*)this->ObjectRef;
			}
		}

		this->Counter = nullptr;
		this->ObjectRef = nullptr;
	}

	inline void VariantPointer::_AddRef()
	{
		if (this->Counter)
		{
			(*this->Counter)++;
		}
	}

	template<class T>
	inline VariantPointer::VariantPointer(const T* ptr)
		: ObjectRef(nullptr), Counter(nullptr)
	{
		if (ptr)
		{
			this->Counter = new int(1);
			this->ObjectRef = ptr;
		}
	}

	template<class T> 
	inline VariantPointer& VariantPointer::operator =(const T* ptr)
	{
		this->_Release<T>();

		if (ptr)
		{
			this->Counter = new int(1);
			this->ObjectRef = (void*)ptr;
		}

		return *this;
	}

	inline VariantPointer& VariantPointer::operator =(const VariantPointer& sp)
	{
		throw Exception("Must Using Assign<>");
	}

	template<class T> 
	VariantPointer& VariantPointer::Assign(const VariantPointer& sp)
	{
		if (sp.ObjectRef != this->ObjectRef)
		{
			this->_Release<T>();

			this->Counter = sp.Counter;
			this->ObjectRef = sp.ObjectRef;
			this->_AddRef();
		}

		return *this;
	}

	template<class T> 
	inline T* VariantPointer::get()
	{
		T* ret = nullptr;

		if (*this)
		{
			ret = (T*)this->ObjectRef;
		}

		return ret;
	}

	inline VariantPointer::operator bool()
	{
		return (this->ObjectRef && this->Counter);
	}

#pragma endregion

}

#endif
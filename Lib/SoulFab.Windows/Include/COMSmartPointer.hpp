#pragma once

#ifndef _SF_COM_SMART_POINTER_H_
#define _SF_COM_SMART_POINTER_H_

#include <Unknwn.h>
#include <atlbase.h>

namespace SoulFab::Base
{
    inline void SafeRelease(IUnknown* pInterface);
    
    template<class T>
	class COMSmartPointer 
	{
	private: 
		void Assign(T* pi);

	protected:
		T* pInterface;

		inline void _AddRef();
		inline void _Release();

	public:
		COMSmartPointer();
		COMSmartPointer(T* pi);	
		COMSmartPointer(const COMSmartPointer<T>& sp);	

		~COMSmartPointer();

		template<class P> COMSmartPointer<P> Query(GUID id);

		inline COMSmartPointer<T>& operator <<(T* pi);
		inline COMSmartPointer<T>& operator =(const COMSmartPointer<T>& sp);
		inline COMSmartPointer<T>& operator =(T* pi);

		inline T* operator ->();
		inline operator bool();
		inline bool operator !();	

		inline T& operator *();
		inline T* operator &();
	};

	template<class T>
	class AutoSmartPointer : public COMSmartPointer<T>
	{
	private:
		static GUID ID;

		template<class P> void Assign(P* pi);

	public:
		AutoSmartPointer();
		AutoSmartPointer(T* pi);	
		AutoSmartPointer(const AutoSmartPointer<T>& sp);	
	};

#pragma region COMSmartPointer

	template<class T>
	inline void COMSmartPointer<T>::_AddRef()
	{
		if(pInterface)
		{
			pInterface->AddRef();
		}
	}

	template<class T>
	inline void COMSmartPointer<T>::_Release()
	{
		if(this->pInterface)
		{
			this->pInterface->Release();
			this->pInterface = NULL;
		}
	}

	template<class T>
	void COMSmartPointer<T>::Assign(T* pi)
	{
		if(this->pInterface != pi) 
		{
			this->_Release();

			this->pInterface = pi;

			this->_AddRef();
		}
	}

	template<class T>
	COMSmartPointer<T>::COMSmartPointer()
	{
		this->pInterface = NULL;
	}

	template<class T>
	COMSmartPointer<T>::COMSmartPointer(T* pi)
	{
		this->pInterface = NULL;

		this->Assign(pi);
	}

	template<class T>
	COMSmartPointer<T>::COMSmartPointer(const COMSmartPointer<T>& sp)
	{
		this->pInterface = NULL;

		this->Assign(sp.pInterface);
	}

	template<class T>
	COMSmartPointer<T>::~COMSmartPointer()  
	{
		this->_Release();
	}

	template<class T> 
	template<class P>
	COMSmartPointer<P> COMSmartPointer<T>::Query(GUID id)
	{
		P* pIntf;
		COMSmartPointer<P> Result;

		HRESULT hr = (*this)->QueryInterface(id, (void**)&pIntf);
		DXException::Check(hr, "Query Interface");		

		Result << pIntf;

		return Result;
	}

	template<class T> 
	inline COMSmartPointer<T>& COMSmartPointer<T>::operator <<(T* pi)
	{
		this->_Release();

		if(pi)
		{
			this->pInterface = pi;
		}

		return *this;
	}

	template<class T>
	inline COMSmartPointer<T>& COMSmartPointer<T>::operator =(const COMSmartPointer<T>& sp)
	{
		this->Assign(sp.pInterface);

		return *this;
	}

	template<class T>
	inline COMSmartPointer<T>& COMSmartPointer<T>::operator =(T* pi)
	{	
		this->Assign(pi);

		return *this;
	}

	template<class T>
	inline T* COMSmartPointer<T>::operator ->()
	{
		return pInterface;	
	}

	template<class T>
	inline COMSmartPointer<T>::operator bool()
	{
		return (this->pInterface != NULL);	
	}

	template<class T>
	inline bool COMSmartPointer<T>::operator !()
	{
		return (this->pInterface == NULL);	
	}

	template<class T>
	inline T* COMSmartPointer<T>::operator &()
	{
		return this->pInterface;	
	}

#pragma endregion

#pragma region AutoSmartPointer

	template<class T>
	GUID AutoSmartPointer<T>::ID = __uuidof(T);


	template<class T>
	template<class P>
	void AutoSmartPointer<T>::Assign(P* pi)
	{
		T* pNew;

		if(pi && this->pInterface != pi) 
		{
			this->_Release();

			HRESULT hr = pi->QueryInterface(id, (void**)&pNew);
			DXException::Check(hr, "Query Interface");		

			this->pInterface = pNew;
		}
	}

	template<class T>
	AutoSmartPointer<T>::AutoSmartPointer()
	{
		this->pInterface = NULL;
	}

	template<class T>
	AutoSmartPointer<T>::AutoSmartPointer(T* pi)
	{
		this->pInterface = NULL;

		this->Assign(pi);
	}

	template<class T>
	AutoSmartPointer<T>::AutoSmartPointer(const AutoSmartPointer<T>& sp)
	{
		this->pInterface = NULL;

		this->Assign(sp.pInterface);
	}

#pragma endregion


	inline void SafeRelease(IUnknown* pInterface)
	{
		if(pInterface)
		{
			pInterface->Release();
			pInterface = NULL;
		}
	}
}

#endif
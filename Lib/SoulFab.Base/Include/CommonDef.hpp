#pragma once

#ifndef _SF_COMMON_DEF_H_
#define _SF_COMMON_DEF_H_

#include <string>
#include <thread>
#include <chrono>

using uint64 = unsigned long long;

#ifdef WIN32
#define SHARED_EXPORT __declspec(dllexport)
#else
#define SHARED_EXPORT 
#endif

template<class T>
inline void SafeDestroyArray(T* &obj)
{
	delete[] obj;          
	obj = nullptr;   
}

template<class T>
inline void SafeDestroy(T* &obj)
{
	delete obj;          
	obj = nullptr;   
}

template<class T>
inline void SafeDestroy(void* &obj)
{
	delete (T*)obj;          
	obj = nullptr;   
}

inline void Sleep(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#endif

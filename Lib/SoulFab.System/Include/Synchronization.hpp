#pragma once

#ifndef _SF_SYNCHRONIZATION_H_
#define _SF_SYNCHRONIZATION_H_

#include <string>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "CommonDef.hpp"


namespace SoulFab::System
{
	class ILocker
	{
	public:	
		virtual void Lock() = 0;
		virtual void Unlock() = 0;
		virtual bool TryLock() = 0;
	};

	class ManualEvent
	{
	public: 		
		ManualEvent();
		ManualEvent(bool state);
		ManualEvent(bool state, const std::string name);

		void Set();
		void Reset();
	};

	class SHARED_EXPORT AutoEvent
	{
	private:
		std::mutex Locker;
		std::condition_variable Handle;

	public: 		
		AutoEvent();

		void Set();
		void Wait();
		bool Wait(int ms);
	};

	class SHARED_EXPORT CriticalSection : public ILocker
	{
	private:
		std::mutex Handler;

	public:
		CriticalSection();
		~CriticalSection();

		void Lock();
		void Unlock();
		bool TryLock();
	};

	class SHARED_EXPORT LockerGuard
	{
	private:	
		bool InGuard;
		ILocker* Locker;	

	public:	
		LockerGuard(CriticalSection& cs, bool test = false);
		~LockerGuard();

		bool IsLocking();
		void Close();
	};

	class Mutex
	{
	private:	
		std::mutex Handler;

	public:	
		Mutex();
		~Mutex();

		bool Open(std::string name);
	};

	class LightLocker
	{
	private:	
		std::atomic<int>* Locker;

	public:	
		LightLocker();
		~LightLocker();

		bool Lock();
		void SpanLock();
		void WaitingLock();
		void Unlock();
	};
}

#endif

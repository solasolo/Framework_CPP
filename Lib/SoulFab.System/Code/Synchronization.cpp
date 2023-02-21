#include "Synchronization.hpp"

#include <atomic>
#include <thread>

#include "CommonDef.hpp"

using namespace std;

namespace SoulFab::System
{
    #pragma region ManualEvent

    #pragma endregion

    #pragma region AutoEvent

    AutoEvent::AutoEvent()
    {
    }

    void AutoEvent::Set()
    {
        this->Handle.notify_all();
    }
    
    void AutoEvent::Wait()
    {
        unique_lock lock(this->Locker);

        this->Handle.wait(lock);
    }

    bool AutoEvent::Wait(int ms)
    {
        unique_lock lock(this->Locker);

        cv_status s = this->Handle.wait_for(lock, ms * 1ms);

        return s == cv_status::no_timeout;
    }

    #pragma endregion


    #pragma region CriticalSection

    CriticalSection::CriticalSection()
    {
    }

    CriticalSection::~CriticalSection()
    {

    }

    void CriticalSection::Lock()
    {
        this->Handler.lock();
    }

    void CriticalSection::Unlock()
    {
        this->Handler.unlock();
    }

    bool CriticalSection::TryLock()
    {
        return this->Handler.try_lock();
    }

    #pragma endregion

    #pragma region LockerGuard

    LockerGuard::LockerGuard(CriticalSection &cs, bool test)
    {
        this->InGuard = false;
        this->Locker = &cs;

        if (!test)
        {
            this->Locker->Lock();
            this->InGuard = true;
        }
        else
        {
            this->InGuard = this->Locker->TryLock();
        }
    }

    LockerGuard::~LockerGuard()
    {
        Close();
    }

    bool LockerGuard::IsLocking()
    {
        return this->InGuard;
    }

    void LockerGuard::Close()
    {
        if (InGuard)
        {
            this->Locker->Unlock();
            this->InGuard = false;
        }
    }

    #pragma endregion

    #pragma region Mutex


    #pragma endregion

    #pragma region LightLocker

	LightLocker::LightLocker()
	{
		this->Locker = nullptr;
		this->Locker = new atomic<int>(0);
	}

	LightLocker::~LightLocker()
	{
		this->Unlock();

        SafeDestroy(this->Locker);
	}

	bool LightLocker::Lock()
	{
		bool ret = false;
		int value = -1;

		if(this->Locker)
		{
			ret = this->Locker->compare_exchange_strong(value, 0);
		}

		return ret;
	}

	void LightLocker::SpanLock()
	{
		while(!Lock())
		{
		}
	}

	void LightLocker::WaitingLock()
	{
		static auto time = chrono::milliseconds(1);

		while(!Lock())
		{
			this_thread::sleep_for(time);
		}
	}

	void LightLocker::Unlock()
	{
		int value = 0;

		this->Locker->compare_exchange_strong(value, -1);
	}

    #pragma endregion
} 
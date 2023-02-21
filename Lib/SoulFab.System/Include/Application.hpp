#pragma once

#ifndef _SF_APPLICATION_H_
#define _SF_APPLICATION_H_

#include <string>

#include "Thread.hpp"
#include "String.hpp"

namespace SoulFab::System
{
	class SHARED_EXPORT BaseApplication : public SoulFab::Base::LogSource , public IThreadContext
	{
	public:
		static int WaitKey(int key = 0);

	public:
		BaseApplication();

		virtual void Main(int argc, char* args[]);

		virtual void Initialize();
		virtual void Uninitialize();

	protected:
		SoulFab::Base::String RootPath;
		
        void MainProcess();

		virtual SoulFab::Base::ILogger* CreateLogger();

		virtual void Create() = 0;
		virtual void Run() = 0;
		virtual void Destroy() = 0;
	};
}

#endif

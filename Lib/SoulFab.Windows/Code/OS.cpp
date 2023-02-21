#include "OS.hpp"

#include <windows.h>

#include "StringHelper.hpp"
#include "SysException.hpp"

using namespace std;
using namespace SoulFab::Base;

namespace SoulFab::System
{
	String OS::GetApplicationRootPath()
	{
		static thread_local char ExecPath[512];

		if (!GetModuleFileNameA(NULL, ExecPath, 512))
		{
			SysException::Check("Get Startup Path");
		}

		string FullPath = ExecPath;
		String path = StringHelper::GBK2UTF8(FullPath);

		String ret = StringHelper::ExtractFilePath(path) + "\\..\\";

		return ret;
	}

	string OS::OSPath(const String path)
	{
		return StringHelper::UTF82GBK(path);
	}

}

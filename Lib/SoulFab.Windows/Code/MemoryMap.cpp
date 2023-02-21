#include "MemoryMap.hpp"

#include <windows.h>

#include "SysException.hpp"
#include "OS.hpp"

using namespace std;
using namespace SoulFab::Base;

namespace SoulFab::System
{
    MemoryMap::MemoryMap()
        : Address(nullptr), ReadOnly(false)
    {

    }

    MemoryMap::MemoryMap(string name, String file, unsigned long size)
        : Address(nullptr), ReadOnly(false)
    {
        this->Create(name, file, size);
    }

    MemoryMap::~MemoryMap()
    {
        this->Close();
    }

    void MemoryMap::Create(string name, String file, unsigned long size)
    {
        using uint64 = unsigned long long;
        const uint64 PageAlign = 0x00000000000003FF;

        uint64 LongSize = (uint64)size;

        // align to page size
        LongSize = ( (LongSize + PageAlign) & ( ~PageAlign) );


        DWORD dwSizeLow = static_cast<DWORD>(LongSize & 0xFFFFFFFF);
        DWORD dwSizeHigh = static_cast< DWORD >((LongSize >> 32 ) & 0xFFFFFFFF);


        HANDLE hFile = this->CreateMapFile(file);

        this->Handle = CreateFileMappingA(hFile , NULL , PAGE_READWRITE , dwSizeHigh , dwSizeLow ,name.c_str());
        if(!this->Handle)
        {
            throw SysException("Create File Map");
        }

        this->OpenView();
    }

    void MemoryMap::Open(char* name)
    {
        DWORD dwAccess = this->ReadOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS;

        this->Handle = OpenFileMapping(dwAccess , FALSE , name);
        if(!this->Handle)
        { 
            SysException::Check("Open File Map");
        }

        this->OpenView();
    }

    void MemoryMap::Close()
    {
        if (this->Address)
        {
            UnmapViewOfFile(this->Address);
        }

        this->Address = nullptr;
        
        if (this->Handle)
        {
            CloseHandle(this->Handle);
        }

        this->Handle = INVALID_HANDLE_VALUE;
    }

    FileHandle MemoryMap::CreateMapFile(String file)
    {
        HANDLE ret = INVALID_HANDLE_VALUE;

        string path = OS::OSPath(file);

        ret = ::CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (ret == INVALID_HANDLE_VALUE)
        {
            SysException::Check("Create/Open Mapping File");
        }

        return ret;
    }

    void MemoryMap::OpenView()
    {
        unsigned long size = 0;
        DWORD dwAccess = this->ReadOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS;

        try
        {
            this->Address = MapViewOfFileEx(this->Handle, dwAccess, 0, 0, size, 0);
            if (!this->Address)
            {

                SysException::Check("Open View of File Map");
            }
        }
        catch (...)
        {
            this->Close();

            throw;
        }
    }

    void* MemoryMap::Get()
    {
        return this->Address;
    }

    void MemoryMap::Delete()
    {

    }
}
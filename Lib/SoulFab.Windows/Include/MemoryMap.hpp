#pragma once

#ifndef _SF_MEMORY_MAP_H_
#define _SF_MEMORY_MAP_H_

#include "CommonDef.hpp"
#include "String.hpp"

#include <string>

namespace SoulFab::System
{
    typedef void* MapHandle;
    typedef void* FileHandle;

    class SHARED_EXPORT MemoryMap
    {
    public:
        MemoryMap();
        MemoryMap(std::string name, SoulFab::Base::String file, unsigned long size);

        ~MemoryMap();

        void Create(std::string name, SoulFab::Base::String file, unsigned long size);
        void Open(char* name);
        void Close();

        void* Get();

    private:
        unsigned long Size;
        void* Address;
        bool ReadOnly;

        MapHandle Handle;

        FileHandle CreateMapFile(SoulFab::Base::String file);
        void OpenView();

        void Delete();
    };
}

#endif
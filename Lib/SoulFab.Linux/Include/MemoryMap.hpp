#pragma once

#ifndef _SF_MEMORY_MAP_H_
#define _SF_MEMORY_MAP_H_

#include <string>

using namespace std;

namespace SoulFab::Link
{
    typedef void* MapHandle;
    typedef void* FileHandle;

    class MemoryMap
    {
    public:
        MemoryMap();
        ~MemoryMap();

        void Create(unsigned long size, char* name, char* file);
        void Open(char* name);
        void Close();

        void* Get();

    private:
        unsigned long Size;
        void* Address;
        bool ReadOnly;

        MapHandle Handle;

        FileHandle CreateMapFiel(char* file);

        void Delete();
    };
}

#endif
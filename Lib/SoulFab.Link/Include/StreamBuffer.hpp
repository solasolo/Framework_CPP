#pragma once

#ifndef _SF_STREAM_BUFFER_H_
#define _SF_STREAM_BUFFER_H_

#include <list>

#include "CommonDef.hpp"
#include "ByteArray.hpp"
#include "Synchronization.hpp"

namespace SoulFab::Link
{
    class SHARED_EXPORT StreamBuffer
    {
        typedef std::list<SoulFab::Base::ByteArray> BufferList;

    public:
        ~StreamBuffer();

        StreamBuffer& operator << (SoulFab::Base::ByteArray data);

        bool Wait();
        void Clear();
        unsigned int getSize();

        bool ReadData(SoulFab::Base::ByteArray& data, unsigned int len = 0);
        bool ReadData(void* data, unsigned int len);
        bool PickData(SoulFab::Base::ByteArray& data, unsigned int len = 0);
        bool PickData(unsigned int len);

        int ScanData(char ch);

        char ReadByte(unsigned int pos);
        short ReadShort(unsigned int pos);
        int ReadInt(unsigned int pos);

    private:
        struct Cursor
        {
            BufferList::iterator it;
            int pos;
        };

        SoulFab::System::CriticalSection BufferGate;
        SoulFab::System::AutoEvent BufferEvent;
        BufferList Buffer;

        unsigned int InnerGetSize();
        bool ReadData(void* data, unsigned int pos, unsigned int len);
        bool Locate(unsigned int pos, Cursor& cur);
    };
}

#endif

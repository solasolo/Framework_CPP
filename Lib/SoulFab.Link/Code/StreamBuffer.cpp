#include "StreamBuffer.hpp"
#include "ByteArray.hpp"
#include "Exception.hpp"

using namespace std;
using namespace SoulFab::System;
using namespace SoulFab::Base;

namespace SoulFab::Link
{
    StreamBuffer::~StreamBuffer()
    {
        this->Clear();
    }

    StreamBuffer& StreamBuffer::operator << (ByteArray data)
    {
        LockerGuard CG(BufferGate);
        
        Buffer.push_back(data);
        this->BufferEvent.Set();

        CG.Close();

        return *this;
    }

    bool StreamBuffer::Wait()
    {
        return this->BufferEvent.Wait(100);
    }

    void StreamBuffer::Clear()
    {
        LockerGuard CG(BufferGate);

        Buffer.clear();

        CG.Close();
    }

    unsigned int StreamBuffer::InnerGetSize()
    {
        unsigned int Result = 0;

        BufferList::iterator it = Buffer.begin();
        while(it != Buffer.end())
        {
            Result += it->size();
            it++;
        }

        return Result;
    }

    unsigned int StreamBuffer::getSize()
    {
        unsigned int Result = 0;

        LockerGuard CG(BufferGate);

        Result = this->InnerGetSize();

        CG.Close();

        return Result;
    }

    bool StreamBuffer::Locate(unsigned int pos, Cursor& cur)
    {
        cur.it = Buffer.begin();
        cur.pos = 0;
        while(cur.it != Buffer.end() && pos > 0)
        {
            int n = cur.it->size();
            if(pos >= n)
            {
                pos -= n;
            }
            else
            {
                cur.pos = pos;
                pos = 0;
                break;
            }

            cur.it++;
        }

        return (pos == 0);
    }

    bool StreamBuffer::ReadData(void* data, unsigned int pos, unsigned int len)
    {
        bool Result = false;

        LockerGuard CG(BufferGate);

        Cursor cur;
        Result = Locate(pos, cur);

        if(Result)
        {
            int c = 0;

            while(cur.it != Buffer.end() && c < len)
            {
                int n = cur.it->size();
                n -= cur.pos;			
                if(len - c < n) n = len - c;

                memcpy((char*)data + c, cur.it->c_str() + cur.pos, n);

                cur.it++;
                c += n;
                cur.pos = 0;
            }
        
            if(len != c) Result = false;	
        }
        
        CG.Close();

        return Result;
    }

    bool StreamBuffer::ReadData(ByteArray& data, unsigned int len)
    {
        int c = 0;
        bool Result = false;

        data.clear();

        LockerGuard CG(BufferGate);
        
        if(InnerGetSize() >= len)
        {
            BufferList::iterator it = Buffer.begin();
            while(it != Buffer.end() && c < len)
            {
                int n = it->size();
                if(len - c < n) n = len - c;
                data += it->substr(0, n);

                it++;
                c += n;
            }

            Result = true;
        }

        CG.Close();

        return Result;
    }

    bool StreamBuffer::PickData(ByteArray& data, unsigned int len)
    {
        bool Result = ReadData(data, len);

        if(Result) PickData(len);

        return Result;
    }

    bool StreamBuffer::PickData(unsigned int len)
    {
        bool Result = false;

        LockerGuard CG(BufferGate);

        if(InnerGetSize() >= len)
        {
            int c = 0;
            BufferList::iterator it = Buffer.begin();
            while(it != Buffer.end() && c < len)
            {
                int n = it->size();
                if(len - c < n) 
                {
                    n = len - c;
                    it->erase(0, n);

                    break;
                }
                else
                {
                    it = Buffer.erase(it); 
                }

                c += n;
            }

            Result = true;
        }

        CG.Close();

        return Result;
    }

    bool StreamBuffer::ReadData(void* data, unsigned int len)
    {
        return this->ReadData(data, 0, len);
    }

    char StreamBuffer::ReadByte(unsigned int pos)
    {
        char Result;

        LockerGuard CG(BufferGate);

        Cursor cur;
        if(Locate(pos, cur))
        {
            Result = (*cur.it)[cur.pos];
        }
        else
        {
            throw Exception("Execced Index", "Read Stream Byte");
        }

        CG.Close();


        return Result;
    }

    short StreamBuffer::ReadShort(unsigned int pos)
    {
        short Result = 0;
        
        if(!ReadData(&Result, pos, 2))
        {
            throw Exception("Execced Index", "Read Stream Short");
        }
            
        return Result;
    }

    int StreamBuffer::ReadInt(unsigned int pos)
    {
        int Result = 0;
        
        if(!ReadData(&Result, pos, 4))
        {
            throw Exception("Execced Index", "Read Stream Int");
        }
            
        return Result;
    }

    int StreamBuffer::ScanData(char ch)
    {
        int Result = -1;

        LockerGuard CG(BufferGate);

        unsigned int pos = 0;
        for (auto& item : Buffer)
        {
            int n = item.find(ch);
            if(n > -1)
            {
                Result = pos + n; 	
                break;
            }
            else
            {
                pos += item.size();
            }
        }

        CG.Close();

        return Result;
    }
}

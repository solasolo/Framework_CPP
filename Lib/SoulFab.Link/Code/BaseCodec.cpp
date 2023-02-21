#include "BaseCodec.hpp"

using namespace std;

namespace SoulFab::Link
{

#pragma region BinaryDecoder
    BinaryDecoder::BinaryDecoder(const string& buf)
    {
        this->Buffer = buf.c_str();
        this->Len = buf.size();
        this->Pos = 0;
    }

    bool BinaryDecoder::RawRead(void* data, int length)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - length)
        {
            const char* p = this->Buffer + this->Pos;
        
            memcpy(data, p, length);
            this->Pos += length;

            Result = true;
        }

        return Result;
    }

    bool BinaryDecoder::Read(char* data, int length)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 2)
        {
            const char* p = this->Buffer + this->Pos;

            int len;
            len = *((short*)p);
            this->Pos += 2;

            if(this->Pos <= this->Len - len)
            {
                memcpy(data, p + 2, len);
                data[len] = 0;
                this->Pos += len;

                Result = true;
            }
        }

        return Result;
    }

    bool BinaryDecoder::Read(string& data)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 2)
        {
            const char* p = this->Buffer + this->Pos;

            int len;
            len = *((short*)p);
            this->Pos += 2;

            if(this->Pos <= this->Len - len)
            {
                data.clear();
                data.append(p + 2, len);
                this->Pos += len;

                Result = true;
            }
        }

        return Result;
    }

    bool BinaryDecoder::Read(string& data, int length)
    {
        bool Result = false;
        if(this->Pos + length <= this->Len)
        {
            const char* p = this->Buffer + this->Pos;
            data.clear();
            data.append(p, length);
            data.resize(strlen(data.c_str()));//去除末尾的\x0
            this->Pos += length;
            Result = true;
        }
        return Result;
    }

    bool BinaryDecoder::Read(int& data)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 4)
        {
            const char* p = this->Buffer + this->Pos;

            data = *((int*)p);
            this->Pos += 4;

            Result = true;
        }

        return Result;
    }

    bool BinaryDecoder::Read(short& data)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 2)
        {
            const char* p = this->Buffer + this->Pos;

            data = *((short*)p);
            this->Pos += 2;

            Result = true;
        }

        return Result;
    }

    bool BinaryDecoder::Read(float& data)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 4)
        {
            const char* p = this->Buffer + this->Pos;

            data = *((float*)p);
            this->Pos += 4;

            Result = true;
        }

        return Result;
    }

    bool BinaryDecoder::Read(char& data)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 1)
        {
            const char* p = this->Buffer + this->Pos;

            data = *((char*)p);
            this->Pos += 1;

            Result = true;
        }

        return Result;
    }

    bool BinaryDecoder::ReadReverse(int& data)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 4)
        {
            const char* p = this->Buffer + this->Pos;
            
            char* content = (char*)&data;
            for(int i = 3; i >= 0; i--)
            {
                content[i] = *p;
                p += 1;
            }

            this->Pos += 4;

            Result = true;
        }

        return Result;
    }

    bool BinaryDecoder::ReadReverse(short& data)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 2)
        {
            const char* p = this->Buffer + this->Pos;
            
            char* content = (char*)&data;
            for(int i = 1; i >= 0; i--)
            {
                content[i] = *p;
                p += 1;
            }

            this->Pos += 2;

            Result = true;
        }

        return Result;
    }

    bool BinaryDecoder::ReadReverse(float& data)
    {
        bool Result = false;
        
        if(this->Pos <= this->Len - 4)
        {
            const char* p = this->Buffer + this->Pos;
            
            char* content = (char*)&data;
            for(int i = 3; i >= 0; i--)
            {
                content[i] = *p;
                p += 1;
            }

            this->Pos += 4;

            Result = true;
        }

        return Result;
    }

#pragma endregion

#pragma region BinaryEncoder

    BinaryEncoder::BinaryEncoder()
    {
        this->Clean();
    }

    void BinaryEncoder::Clean()
    {
        this->Buffer.clear();
    }

    void BinaryEncoder::AppendZero(string& Buffer, int count)
    {
        for(int i = 0; i < count; i++)
        {
            Buffer.append("\x0", 1);
        }
    }

    void BinaryEncoder::RawWrite(const void* data, int length)
    {
        Buffer.append((char*)data, length);
    }

    void BinaryEncoder::Write(const char* data, int length)
    {
        int actLen = strlen(data);
        
        if(actLen > length)
        {
            actLen = length;
        }

        Buffer.append((char*)&actLen, 2);
        Buffer.append(data, actLen);

        if(actLen < length)
        {
            AppendZero(Buffer,length - actLen);
        }
    }

    void BinaryEncoder::Write(char data)
    {
        Buffer.append(&data, 1);
    }

    void BinaryEncoder::Write(int data)
    {
        Buffer.append((char*)&data, 4);
    }

    void BinaryEncoder::Write(short data)
    {
        Buffer.append((char*)&data, 2);
    }

    void BinaryEncoder::Write(float data)
    {
        Buffer.append((char*)&data, 4);
    }

    void BinaryEncoder::Write(const string& data)
    {
        int len = data.length();	// 字符串型数据在数据前增加2位长度
        Buffer.append((char*)&len, 2);
        Buffer.append(data.c_str(), len);
    }

    void BinaryEncoder::Write(const string& data,int length)
    {
        int actLen = data.length();
        
        if(actLen > length)
        {
            actLen = length;
        }

        Buffer.append(data.c_str(), actLen);

        if(actLen < length)
        {
            AppendZero(Buffer,length - actLen);
        }
    }

    void BinaryEncoder::Write(const char* data)
    {
        int len = strlen(data);	// 字符串型数据在数据前增加2位长度
        Buffer.append((char*)&len, 2);
        Buffer.append(data, len);
    }

    void BinaryEncoder::WriteReverse(int data)
    {
        for(int i = 3; i >= 0; i--)
        {
            Buffer.append((char*)&data + i, 1);
        }
    }

    void BinaryEncoder::WriteReverse(short data)
    {
        for(int i = 1; i >= 0; i--)
        {
            Buffer.append((char*)&data + i, 1);
        }
    }

    void BinaryEncoder::WriteReverse(float data)
    {
        for(int i = 3; i >= 0; i--)
        {
            Buffer.append((char*)&data + i, 1);
        }
    }

    string& BinaryEncoder::GetData()
    {
        return Buffer;
    }


#pragma endregion

}
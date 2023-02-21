#include "SimpleProtocol.hpp"

using namespace std;

namespace SoulFab::Link
{
    SimpleProtocolDecoder::SimpleProtocolDecoder(const string& buf)
        : BinaryDecoder(buf)
    {
    }

    //
    // Class SimpleProtocolEncoder
    //

    SimpleProtocolEncoder::SimpleProtocolEncoder()
        : BinaryEncoder()
    {
        this->FullFrame = false;
    }

    SimpleProtocolEncoder::SimpleProtocolEncoder(short cmd)
        : BinaryEncoder()
    {
        this->FullFrame = true;

        this->Write(FrameDelimiter::STX);
        this->Write((short)0);
        this->Write(cmd);
    }

    string& SimpleProtocolEncoder::GetData()
    {
        if (this->FullFrame)
        {
            int l = Buffer.length() - 1;

            this->Write(FrameDelimiter::ETX);
            Buffer[1] = *((char*)&l);
            Buffer[2] = *((char*)&l + 1);
        }

        return Buffer;
    }

    bool SimpleFrameCodec::Decode(StreamBuffer& stream, short& cmd, string& blcok)
    {
        bool Result = false;
        bool Remain = false;

        do
        {
            int start_pos = stream.ScanData(FrameDelimiter::STX);
            if (start_pos > 0)
            {
                stream.PickData(start_pos);
            }
            else if (start_pos < 0)
            {
                stream.Clear();
            }

            int BufLen = stream.getSize();
            if (BufLen >= HEAD_SIZE + 2)
            {
                int PayloadSize = stream.ReadInt(1);

                if (BufLen >= PayloadSize + HEAD_SIZE + 2)
                {
                    if (stream.ReadByte(PayloadSize + HEAD_SIZE + 1) == FrameDelimiter::ETX)
                    {
                        cmd = (short)(stream.ReadShort(LEN_SIZE + 1)); // Read Command

                        stream.PickData(HEAD_SIZE + 1);		// 删除cmd及之前的字节 1 + 2 + 2

                        if (PayloadSize > 0)
                        {
                            stream.PickData(blcok, PayloadSize); // Read Data
                        }
                        else
                        {
                            blcok = "";
                        }

                        stream.PickData(1); // Remove ETX
                        Result = true;
                    }
                    else
                    {
                        stream.PickData(1); // Remove STX
                        Remain = true;
                    }
                }
            }
        } while (!Result && Remain);
        
        return Result;
    }


    void SimpleFrameCodec::Encode(string& message, short cmd, const string& blcok)
    {
        int l = blcok.length();

        message.clear();
        message.append(&FrameDelimiter::STX, 1);
        message.append((char*)&l, 2);
        message.append((char*)&cmd, 2);
        message += blcok;
        message.append(&FrameDelimiter::ETX, 1);
    }
}

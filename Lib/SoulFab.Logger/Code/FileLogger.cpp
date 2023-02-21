#include "FileLogger.hpp"

#include <fstream>
#include <vector>
#include "OS.hpp"

using namespace std;
using namespace SoulFab::Base;
using namespace SoulFab::System;

namespace SoulFab::Logger
{
#pragma region BaseFileLogger

    string BaseFileLogger::MakeFileName(const String& prefix, const String& DateStr)
    {
        String Result;

        Result = this->Path + prefix + u8"_" + DateStr + u8".log";

        return OS::OSPath(Result);
    }

    BaseFileLogger::BaseFileLogger(const String path, const string prefix)
    {
        this->Path = path;
        this->Prefix = prefix;

        this->LogFile = NULL;
        this->LastFileName.clear();
    }

    BaseFileLogger::~BaseFileLogger()
    {
    }

    void BaseFileLogger::OpenLog(string file)
    {
        if (this->LogFile)
        {
            if (this->LastFileName != file)
            {
                this->CloseLog();
            }
        }
        else
        {
            this->CloseLog();
        }

        if (!this->LogFile)
        {
            this->LogFile = new ofstream(file.c_str(), ios::out | ios::app | ios::binary);
        }

        this->LastFileName = file;
    }

    void BaseFileLogger::OutputLog(const DateTime& time, EventType Type, const String& Message)
    {
        string sTime = FormatTime(time);
        string sType = getTypeString((EventType)Type);

        String LogFileName = MakeFileName(Prefix, FormatDate(time));
        this->OpenLog(LogFileName);

        if (this->LogFile)
        {
            if (Type != EventType::Record)
            {
                (*this->LogFile) << sTime << "\t" << sType << "\t";
            }

            (*this->LogFile) << Message.Pointer() << endl;
        }
    }

    void BaseFileLogger::Write(const DateTime& time, EventType type, const String& message)
    {
        this->OutputLog(time, type, message);
        if (this->LogFile) this->LogFile->flush();
    }

    void BaseFileLogger::CloseLog()
    {
        if (this->LogFile)
        {
            this->LogFile->flush();
            this->LogFile->close();

            delete this->LogFile;
            this->LogFile = NULL;
        }

        this->LastFileName.clear();
    }

    void BaseFileLogger::Clean(const DateTime& last_time)
    {
        /*
        vector<string> FileList;
        FileList.clear();

        T_finddata_t FindData;
        intptr_t FindHandle;
        string FileName = path + prefix + "*.log";

        string LastFile = path + MakeFileName(prefix, FormatDate(last_time));

        FindHandle	= T_findfirst((TChar*)FileName.c_str(), &FindData);
        int rt = FindHandle;
        while(rt != -1)
        {
            string FindFile;
            FindFile = FindData.name;
            if(FindFile < LastFile)
            {
                FileList.push_back(FindFile);
            }
            rt = T_findnext(FindHandle, &FindData);
        }
        _findclose(FindHandle);

        vector<string>::iterator it;
        for(it = FileList.begin(); it != FileList.end(); it++)
        {
            _wremove((path + *it).c_str());
        }
        */
    }

#pragma endregion


#pragma region FileLogger

    FileLogger::FileLogger(string path, string prefix)
        : BaseFileLogger(path, prefix)
    {
        Log(EventType::Message, u8"************************************日志服务启动(Log Started)*************************************");
    }

    FileLogger::~FileLogger()
    {
        Log(EventType::Message, u8"************************************日志服务中止(Log End)*************************************");
    }

    void FileLogger::BatchLog(const list<LogMessage>& msg_list) throw()
    {
        try
        {
            if (this->LogFile)
            {
                for (auto msg : msg_list)
                {
                    LockerGuard LG(Gate);

                    this->OutputLog(msg.Time, msg.Type, msg.Message);

                    LG.Close();
                }

                if (this->LogFile) this->LogFile->flush();
            }
        }
        catch (...)
        {
        }

    }

#pragma endregion 

}



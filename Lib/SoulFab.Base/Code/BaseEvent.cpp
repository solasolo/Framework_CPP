#include "BaseEvent.hpp"

#include <stdarg.h>

using namespace std;

namespace SoulFab::Base
{
    Logger::Logger()
        : InnerLogger(nullptr)
    {
    }

    Logger::Logger(const ILogger* callback)
    {
        this->setLogger(callback);

    }

    void Logger::setLogger(const ILogger* callback)
    {
        this->InnerLogger = const_cast<ILogger*>(callback);
    }

    //
    //
    //

    LogSource::LogSource(const String& name)
    {
        this->LogName = name;
    }

    String LogSource::getLogName()
    {
        return this->LogName;
    }
}

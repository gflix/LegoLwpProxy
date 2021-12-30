#ifndef UTILS_LOG_HPP_
#define UTILS_LOG_HPP_

#include <sstream>
#include <string>
#include <models/LogLevel.hpp>

namespace Lego
{

class Log
{
public:
    Log();
    explicit Log(LogLevel logLevel);
    virtual ~Log() = default;

    void error(std::string logMessage) const;
    void warning(std::string logMessage) const;
    void notice(std::string logMessage) const;
    void info(std::string logMessage) const;
    void debug(std::string logMessage) const;

    void setLogLevel(LogLevel logLevel);
    LogLevel getLogLevel(void) const;

    static Log instance;

private:
    bool toTty;
    LogLevel logLevel;

    std::string getTimestamp(void) const;
    void determineOutputMode(void);
};

#define LOG_ERROR(message) { \
    std::stringstream tempLogStream; tempLogStream << message; Lego::Log::instance.error(tempLogStream.str()); \
}
#define LOG_WARNING(message) { \
    std::stringstream tempLogStream; tempLogStream << message; Lego::Log::instance.warning(tempLogStream.str()); \
}
#define LOG_NOTICE(message) { \
    std::stringstream tempLogStream; tempLogStream << message; Lego::Log::instance.notice(tempLogStream.str()); \
}
#define LOG_INFO(message) { \
    std::stringstream tempLogStream; tempLogStream << message; Lego::Log::instance.info(tempLogStream.str()); \
}
#define LOG_DEBUG(message) { \
    std::stringstream tempLogStream; tempLogStream << message; Lego::Log::instance.debug(tempLogStream.str()); \
}

} /* namespace Lego */

#endif /* UTILS_LOG_HPP_ */

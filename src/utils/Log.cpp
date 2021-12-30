#include <stdio.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <ctime>
#include <iostream>
#include <utils/Log.hpp>

namespace Lego
{

Log Log::instance = {};

Log::Log():
    Log(LogLevel::DEBUG)
{
}

Log::Log(LogLevel logLevel):
    toTty(true),
    logLevel(logLevel)
{
    determineOutputMode();
}

void Log::error(std::string logMessage) const
{
    if (LogLevel::ERROR <= logLevel) {
        if (toTty) {
            std::cerr << "\033[31;1m" << getTimestamp() << " [E] " << logMessage << "\033[0m" << std::endl;
        } else {
            std::cerr << SD_ERR << logMessage << std::endl;
        }
    }
}

void Log::warning(std::string logMessage) const
{
    if (LogLevel::WARNING <= logLevel) {
        if (toTty) {
            std::cerr << "\033[33;1m" << getTimestamp() << " [W] " << logMessage << "\033[0m" << std::endl;
        } else {
            std::cerr << SD_WARNING << logMessage << std::endl;
        }
    }
}

void Log::notice(std::string logMessage) const
{
    if (LogLevel::NOTICE <= logLevel) {
        if (toTty) {
            std::cout << "\033[1m" << getTimestamp() << " [N] " << logMessage << "\033[0m" << std::endl;
        } else {
            std::cerr << SD_NOTICE << logMessage << std::endl;
        }
    }
}

void Log::info(std::string logMessage) const
{
    if (LogLevel::INFO <= logLevel) {
        if (toTty) {
            std::cout << getTimestamp() << " [I] " << logMessage << std::endl;
        } else {
            std::cerr << SD_INFO << logMessage << std::endl;
        }
    }
}

void Log::debug(std::string logMessage) const
{
    if (LogLevel::DEBUG <= logLevel) {
        if (toTty) {
            std::cout << "\033[30;1m" << getTimestamp() << " [D] " << logMessage << "\033[0m" << std::endl;
        } else {
            std::cerr << SD_DEBUG << logMessage << std::endl;
        }
    }
}

void Log::setLogLevel(LogLevel logLevel)
{
    this->logLevel = logLevel;
}

LogLevel Log::getLogLevel(void) const
{
    return logLevel;
}

std::string Log::getTimestamp(void) const
{
    char buffer[128];
    time_t currentTimestamp = time(nullptr);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&currentTimestamp));

    return buffer;
}

void Log::determineOutputMode(void)
{
    toTty = isatty(fileno(stdout)) && isatty(fileno(stderr));
}

} /* namespace Lego */

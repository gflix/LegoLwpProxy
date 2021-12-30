#include <models/LogLevel.hpp>

namespace Lego
{

std::ostream& operator<<(std::ostream& stream, const LogLevel& item)
{
    switch (item)
    {
    case LogLevel::ERROR:
        stream << "ERROR";
        break;
    case LogLevel::WARNING:
        stream << "WARNING";
        break;
    case LogLevel::NOTICE:
        stream << "NOTICE";
        break;
    case LogLevel::INFO:
        stream << "INFO";
        break;
    case LogLevel::DEBUG:
        stream << "DEBUG";
        break;
    default:
        stream << "UNKNOWN";
        break;
    }

    return stream;
}

} /* namespace Lego */

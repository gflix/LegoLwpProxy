#ifndef MODELS_LOGLEVEL_HPP_
#define MODELS_LOGLEVEL_HPP_

#include <ostream>

namespace Lego
{

enum class LogLevel
{
    ERROR = 0,
    WARNING = 1,
    NOTICE = 2,
    INFO = 3,
    DEBUG = 4
};

std::ostream& operator<<(std::ostream& stream, const LogLevel& item);

} /* namespace Lego */

#endif /* MODELS_LOGLEVEL_HPP_ */

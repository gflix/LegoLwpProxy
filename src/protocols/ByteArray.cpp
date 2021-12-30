#include <sstream>
#include <protocols/ByteArray.hpp>

namespace Lego::Protocol
{

uint8_t ByteArray::decodeHexDigit(char digit)
{
    if (digit >= '0' && digit <= '9')
    {
        return digit - '0';
    }
    else if (digit >= 'a' && digit <= 'f')
    {
        return digit + 10 - 'a';
    }
    else if (digit >= 'A' && digit <= 'F')
    {
        return digit + 10 - 'A';
    }
    throw std::invalid_argument("invalid hex digit");
}

Lego::ByteArray ByteArray::fromHexString(const std::string& input)
{
    if (input.size() & 1)
    {
        throw std::invalid_argument("require even number of characters when decoding hex string to ByteArray");
    }

    auto it = input.cbegin();
    Lego::ByteArray byteArray;
    while (it != input.cend())
    {
        uint8_t value = decodeHexDigit(*it) << 4;
        ++it;
        value |= decodeHexDigit(*it);
        ++it;

        byteArray.push_back(value);
    }

    return byteArray;
}

Lego::ByteArray ByteArray::fromVector(const std::vector<uint8_t>& input)
{
    return { input.cbegin(), input.cend() };
}

std::string ByteArray::toHexString(const Lego::ByteArray& byteArray, bool withWhitespace)
{
    std::stringstream stream;
    bool firstElement = true;
    char buffer[4];

    for (auto element: byteArray)
    {
        if (firstElement)
        {
            firstElement = false;
        }
        else
        {
            if (withWhitespace)
            {
                stream << " ";
            }
        }

        snprintf(buffer, sizeof(buffer) - 1, "%02X", element);
        stream << buffer;
    }

    return stream.str();
}

std::vector<uint8_t> ByteArray::toVector(const Lego::ByteArray& byteArray)
{
    return { byteArray.cbegin(), byteArray.cend() };
}

} /* namespace Lego::Protocol */

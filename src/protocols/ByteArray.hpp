#ifndef PROTOCOLS_BYTEARRAY_HPP_
#define PROTOCOLS_BYTEARRAY_HPP_

#include <vector>
#include <models/ByteArray.hpp>

namespace Lego::Protocol
{

class ByteArray
{
public:

    static uint8_t decodeHexDigit(char digit);
    static Lego::ByteArray fromHexString(const Lego::ByteArray& input);
    static Lego::ByteArray fromHexString(const std::string& input);
    static Lego::ByteArray fromVector(const std::vector<uint8_t>& input);
    static std::string toHexString(const Lego::ByteArray& byteArray, bool withWhitespace = false);
    static std::vector<uint8_t> toVector(const Lego::ByteArray& byteArray);
};

} /* namespace Lego::Protocol */

#endif /* PROTOCOLS_BYTEARRAY_HPP_ */

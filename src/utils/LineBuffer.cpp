#include <stdexcept>
#include <utils/LineBuffer.hpp>

namespace Lego
{

const ByteArray LineBuffer::lineBreak { 0x0d, 0x0a }; // CR LF

LineBuffer::LineBuffer():
    m_lineEndPosition(ByteArray::npos)
{
}

LineBuffer::~LineBuffer()
{
}

void LineBuffer::putDataIntoBuffer(const ByteArray& data)
{
    m_buffer += data;
}

bool LineBuffer::hasLine(void)
{
    m_lineEndPosition = m_buffer.find(lineBreak);

    return m_lineEndPosition != ByteArray::npos;
}

ByteArray LineBuffer::getLine(void)
{
    if (m_lineEndPosition == ByteArray::npos)
    {
        throw std::runtime_error("no complete line in buffer");
    }

    auto line = m_buffer.substr(0, m_lineEndPosition);
    m_buffer.erase(0, m_lineEndPosition + lineBreak.size());

    return line;
}

} /* namespace Lego */

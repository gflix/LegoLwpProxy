#ifndef UTILS_LINEBUFFER_HPP_
#define UTILS_LINEBUFFER_HPP_

#include <models/ByteArray.hpp>

namespace Lego
{

class LineBuffer
{
public:
    LineBuffer();
    virtual ~LineBuffer();

    void putDataIntoBuffer(const ByteArray& data);
    bool hasLine(void);
    ByteArray getLine(void);

    static const ByteArray lineBreak;

protected:

    ByteArray m_buffer;
    ByteArray::size_type m_lineEndPosition;

private:

};

} /* namespace Lego */

#endif /* UTILS_LINEBUFFER_HPP_ */

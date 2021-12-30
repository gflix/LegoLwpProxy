#include <unistd.h>
#include <controllers/GenericDescriptor.hpp>

namespace Lego
{

GenericDescriptor::GenericDescriptor()
{
    resetDescriptor();
}

GenericDescriptor::~GenericDescriptor()
{
    close();
}

void GenericDescriptor::close(void)
{
    if (!isConnected())
    {
        return;
    }

    ::close(m_descriptor);
    resetDescriptor();
}

int GenericDescriptor::descriptor(void) const
{
    return m_descriptor;
}

bool GenericDescriptor::isConnected(void) const
{
    return m_descriptor >= 0;
}

void GenericDescriptor::resetDescriptor(void)
{
    m_descriptor = -1;
}

} /* namespace Lego */

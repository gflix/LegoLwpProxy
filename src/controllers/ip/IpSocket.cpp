#include <arpa/inet.h>
#include <cstring>
#include <controllers/ip/IpSocket.hpp>

namespace Lego
{

IpSocket::IpSocket(void):
    m_protocol(Protocol::INVALID)
{
}

IpSocket::~IpSocket()
{
}

IpSocket::Protocol IpSocket::protocol(void) const
{
    return m_protocol;
}

void IpSocket::close(void)
{
    m_protocol = Protocol::INVALID;
    GenericDescriptor::close();
}

std::string IpSocket::ipv6SocketAddressToString(const sockaddr_in6& address)
{
    char addressBuffer[INET6_ADDRSTRLEN];

    bzero(addressBuffer, sizeof(addressBuffer));
    inet_ntop(AF_INET6, &address.sin6_addr, addressBuffer, sizeof(addressBuffer));

    return std::string(addressBuffer) + ":" + std::to_string(ntohs(address.sin6_port));
}

bool IpSocket::withinRange(int value, int minInclusive, int maxInclusive)
{
    return value >= minInclusive && value <= maxInclusive;
}

} /* namespace Lego */

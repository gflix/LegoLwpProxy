#ifndef CONTROLLERS_IP_IPSOCKET_HPP_
#define CONTROLLERS_IP_IPSOCKET_HPP_

#include <netinet/in.h>
#include <string>
#include <controllers/GenericDescriptor.hpp>

#define IP_SOCKET_PORT_MIN (1024)
#define IP_SOCKET_PORT_MAX (65535)

namespace Lego
{

class IpSocket: public GenericDescriptor
{
public:
    enum class Protocol
    {
        IPV4,
        IPV6,
        INVALID,
    };

    IpSocket(void);
    virtual ~IpSocket();

    Protocol protocol(void) const;
    void close(void) override;

    static std::string ipv6SocketAddressToString(const sockaddr_in6& address);
    static bool withinRange(int value, int minInclusive, int maxInclusive);

protected:
    Protocol m_protocol;
};

} /* namespace Lego */

#endif /* CONTROLLERS_IP_IPSOCKET_HPP_ */

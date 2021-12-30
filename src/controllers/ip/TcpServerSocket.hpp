#ifndef CONTROLLERS_IP_TCPSERVERSOCKET_HPP_
#define CONTROLLERS_IP_TCPSERVERSOCKET_HPP_

#include <controllers/ip/TcpSocket.hpp>

namespace Lego
{

class TcpServerSocket: public IpSocket
{
public:
    TcpServerSocket(void);
    virtual ~TcpServerSocket();

    void open(int port, IpSocket::Protocol protocol = IpSocket::Protocol::IPV4);
};

} /* namespace Lego */

#endif /* CONTROLLERS_IP_TCPSERVERSOCKET_HPP_ */

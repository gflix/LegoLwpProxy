#ifndef CONTROLLERS_IP_TCPSOCKET_HPP_
#define CONTROLLERS_IP_TCPSOCKET_HPP_

#include <controllers/ip/IpSocket.hpp>
#include <models/ByteArray.hpp>

namespace Lego
{

class TcpSocket: public IpSocket
{
public:
    TcpSocket(void);
    virtual ~TcpSocket();

    void setDescriptor(int descriptor, IpSocket::Protocol protocol);
    void send(const ByteArray& data);
    void receive(ByteArray& data, int& timeoutMilliseconds);
};

} /* namespace Lego */

#endif /* CONTROLLERS_IP_TCPSOCKET_HPP_ */

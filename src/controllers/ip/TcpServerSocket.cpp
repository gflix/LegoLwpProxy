#include <netinet/in.h>
#include <cstring>
#include <stdexcept>
#include <controllers/ip/TcpServerSocket.hpp>

namespace Lego
{

TcpServerSocket::TcpServerSocket(void)
{
}

TcpServerSocket::~TcpServerSocket()
{
}

void TcpServerSocket::open(int port, IpSocket::Protocol protocol)
{
    close();

    if (!withinRange(port, IP_SOCKET_PORT_MIN, IP_SOCKET_PORT_MAX))
    {
        throw std::invalid_argument(
            "invalid port number (got " + std::to_string(port) + ", "
            "expected " + std::to_string(IP_SOCKET_PORT_MIN) + "-" + std::to_string(IP_SOCKET_PORT_MAX) + ")");
    }

    int socketNamespace = AF_UNSPEC;
    switch (protocol)
    {
    case IpSocket::Protocol::IPV4:
        socketNamespace = AF_INET;
        break;
    case IpSocket::Protocol::IPV6:
        socketNamespace = AF_INET6;
        break;
    default:
        throw std::invalid_argument("invalid protocol");
    }
    m_protocol = protocol;

    m_descriptor = socket(socketNamespace, SOCK_STREAM, 0);
    if (!isConnected())
    {
        throw std::runtime_error("could not create TCP socket");
    }

    int enableReuseAddress = 1;
    setsockopt(m_descriptor, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddress, sizeof(enableReuseAddress));

    if (protocol == Protocol::IPV4)
    {
        sockaddr_in serverAddress;
        bzero(&serverAddress, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(port);
        if (bind(m_descriptor, (sockaddr*) &serverAddress, sizeof(serverAddress)) != 0)
        {
            close();
            throw std::runtime_error("could not bind to socket");
        }
    }
    else
    {
        sockaddr_in6 serverAddress;
        bzero(&serverAddress, sizeof(serverAddress));
        serverAddress.sin6_family = AF_INET6;
        serverAddress.sin6_addr = in6addr_any;
        serverAddress.sin6_port = htons(port);
        if (bind(m_descriptor, (sockaddr*) &serverAddress, sizeof(serverAddress)) != 0)
        {
            close();
            throw std::runtime_error("could not bind to socket");
        }
    }

    if (listen(m_descriptor, 5) != 0)
    {
        close();
        throw std::runtime_error("could not listen on socket");
    }
}

} /* namespace Lego */

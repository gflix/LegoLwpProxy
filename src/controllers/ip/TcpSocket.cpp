#include <unistd.h>
#include <stdexcept>
#include <controllers/ip/TcpSocket.hpp>

#define TCP_SOCKET_RECEIVE_BUFFER (256)

namespace Lego
{

TcpSocket::TcpSocket()
{
}

TcpSocket::~TcpSocket()
{
}

void TcpSocket::setDescriptor(int descriptor, IpSocket::Protocol protocol)
{
    m_descriptor = descriptor;
    m_protocol = protocol;
}

void TcpSocket::send(const ByteArray& data)
{
    if (!isConnected())
    {
        throw std::runtime_error("TCP socket is not connected");
    }

    fd_set fileDescriptors;
    timeval timeout;

    FD_ZERO(&fileDescriptors);
    FD_SET(m_descriptor, &fileDescriptors);
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    if (::select(m_descriptor + 1, nullptr, &fileDescriptors, nullptr, &timeout) <= 0)
    {
        throw std::runtime_error("timed out waiting to transmit the data");
    }

    auto writtenBytes = ::write(m_descriptor, data.c_str(), data.size());
    if (writtenBytes < (ssize_t) sizeof(data.size()))
    {
        throw std::runtime_error("could not send the data");
    }
}

void TcpSocket::receive(ByteArray& data, int& timeoutMilliseconds)
{
    if (!isConnected())
    {
        throw std::runtime_error("TCP socket is not connected");
    }

    fd_set fileDescriptors;
    timeval timeout;

    FD_ZERO(&fileDescriptors);
    FD_SET(m_descriptor, &fileDescriptors);
    timeout.tv_sec = timeoutMilliseconds / 1000;
    timeout.tv_usec = (timeoutMilliseconds % 1000) * 1000;

    if (::select(m_descriptor + 1, &fileDescriptors, NULL, NULL, &timeout) <= 0) {
        timeoutMilliseconds = 0;
        throw std::runtime_error("timed out waiting for data from the TCP socket");
    }
    timeoutMilliseconds = (timeout.tv_sec * 1000) + (timeout.tv_usec / 1000);

    uint8_t buffer[TCP_SOCKET_RECEIVE_BUFFER];
    auto receivedBytes = ::read(m_descriptor, buffer, TCP_SOCKET_RECEIVE_BUFFER);
    if (receivedBytes < 0)
    {
        throw std::runtime_error("could not receive data from the TCP socket");
    }

    data = ByteArray(buffer, receivedBytes);
}

} /* namespace Lego */

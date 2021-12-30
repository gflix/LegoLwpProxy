#include <unistd.h>
#include <controllers/threads/ServerConnectionThread.hpp>
#include <utils/Log.hpp>

namespace Lego
{

ServerConnectionThread::ServerConnectionThread(
    BluetoothGattCharacteristicController& legoLwpCharacteristic,
    int clientDescriptor,
    IpSocket::Protocol protocol,
    const std::string& peer):
    GenericThread("ServerConnectionThread(" + peer + ")"),
    m_legoLwpCharacteristic(legoLwpCharacteristic)
{
    m_socket.setDescriptor(clientDescriptor, protocol);
}

ServerConnectionThread::~ServerConnectionThread()
{
}

void ServerConnectionThread::send(const ByteArray& data)
{
    if (!m_socket.isConnected())
    {
        throw std::runtime_error("TCP client socket is not connected");
    }

    m_socket.send(data);
}

void ServerConnectionThread::setup(void)
{
    if (!m_socket.isConnected())
    {
        throw std::runtime_error("TCP client socket is not connected");
    }
    if (m_socket.protocol() != IpSocket::Protocol::IPV6)
    {
        throw std::runtime_error("TCP client socket is not an IPv6 socket");
    }
}

void ServerConnectionThread::task(fd_set& descriptors)
{
    if (FD_ISSET(m_socket.descriptor(), &descriptors))
    {
        ByteArray receivedData;
        int timeoutMilliseconds = 50;
        m_socket.receive(receivedData, timeoutMilliseconds);

        if (receivedData.empty())
        {
            m_socket.close();
            m_threadFinished = true;
        }
        else
        {
            LOG_INFO("Received " << receivedData.size() << " bytes of data");
        }
    }
}

void ServerConnectionThread::shutdown(void)
{
    m_socket.close();
}

void ServerConnectionThread::updateDescriptors(int& maxDescriptor, fd_set& descriptors, timeval& timeout)
{
    maxDescriptor = max(maxDescriptor, m_socket.descriptor());
    FD_SET(m_socket.descriptor(), &descriptors);
}

} /* namespace Lego */

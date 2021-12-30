#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <controllers/threads/ServerConnectionThread.hpp>
#include <controllers/threads/ServerThread.hpp>
#include <utils/Log.hpp>

namespace Lego
{

ServerThread::ServerThread(
    TcpServerSocket& serverSocket,
    BluetoothGattCharacteristicController& legoLwpCharacteristic):
    GenericThread("ServerThread"),
    m_serverSocket(serverSocket),
    m_legoLwpCharacteristic(legoLwpCharacteristic)
{
    m_legoLwpCharacteristic.setServerThread(this);
}

ServerThread::~ServerThread()
{
}

void ServerThread::broadcast(const ByteArray& data, ServerConnectionThread* except)
{
    if (data.empty())
    {
        return;
    }

    std::lock_guard<std::mutex> lock(m_connectionThreadsMutex);

    for (auto it = m_connectionThreads.begin(); it != m_connectionThreads.end(); ++it)
    {
        auto connectionThread = it->get();
        if (connectionThread == except)
        {
            continue;
        }

        if (!connectionThread->threadHasFinished())
        {
            connectionThread->send(data);
        }
    }
}

void ServerThread::broadcast(const std::string& data, ServerConnectionThread* except)
{
    broadcast(ByteArray((uint8_t*) data.c_str(), data.size()), except);
}

void ServerThread::setup(void)
{
    if (!m_serverSocket.isConnected())
    {
        throw std::runtime_error("TCP server socket is not connected");
    }
    if (m_serverSocket.protocol() != IpSocket::Protocol::IPV6)
    {
        throw std::runtime_error("TCP server socket is not an IPv6 socket");
    }
}

void ServerThread::task(fd_set& descriptors)
{
    std::lock_guard<std::mutex> lock(m_connectionThreadsMutex);

    if (FD_ISSET(m_serverSocket.descriptor(), &descriptors))
    {
        sockaddr_in6 clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        auto clientSocket = ::accept(m_serverSocket.descriptor(), (sockaddr*) &clientAddress, &clientAddressSize);
        if (clientSocket >= 0)
        {
            auto peer = IpSocket::ipv6SocketAddressToString(clientAddress);

            auto connectionThread = new ServerConnectionThread(
                m_legoLwpCharacteristic, clientSocket, IpSocket::Protocol::IPV6, peer);
            try
            {
                connectionThread->start();
                m_legoLwpCharacteristic.restartNotifications();
                m_connectionThreads.push_back(std::unique_ptr<ServerConnectionThread>(connectionThread));
            }
            catch(const std::exception& e)
            {
                delete connectionThread;
                LOG_WARNING("Could not start the connection thread (" << e.what() << ")! Closing the connection.");
                ::close(clientSocket);
            }
        }
        else
        {
            LOG_WARNING("Could not accept incoming TCP connection! Unknown error! Skipping.");
        }
    }

    cleanUpFinishedConnectionThreads();
}

void ServerThread::shutdown(void)
{
    std::lock_guard<std::mutex> lock(m_connectionThreadsMutex);

    for (auto it = m_connectionThreads.begin(); it != m_connectionThreads.end(); ++it)
    {
        auto connectionThread = it->get();
        if (!connectionThread->threadHasFinished())
        {
            LOG_INFO("Stopping thread " + connectionThread->threadIdentifier() + " ...");
            connectionThread->stop();
        }
    }
    cleanUpFinishedConnectionThreads();
}

void ServerThread::updateDescriptors(int& maxDescriptor, fd_set& descriptors, timeval& timeout)
{
    maxDescriptor = max(maxDescriptor, m_serverSocket.descriptor());
    FD_SET(m_serverSocket.descriptor(), &descriptors);
}

void ServerThread::cleanUpFinishedConnectionThreads(void)
{
    if (!m_connectionThreads.empty())
    {
        auto it = m_connectionThreads.begin();
        while (it != m_connectionThreads.end())
        {
            auto connectionThread = it->get();
            if (connectionThread->threadHasFinished())
            {
                it = m_connectionThreads.erase(it);
            }
            else
            {
                ++it;
            }
        }

        if (m_connectionThreads.empty())
        {
            LOG_INFO("No client connections left, stopping notifications.");
            m_legoLwpCharacteristic.stopNotifications();
        }
    }
}

} /* namespace Lego */

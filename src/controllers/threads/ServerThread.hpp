#ifndef CONTROLLERS_THREADS_SERVERTHREAD_HPP_
#define CONTROLLERS_THREADS_SERVERTHREAD_HPP_

#include <memory>
#include <mutex>
#include <vector>
#include <controllers/dbus/BluetoothGattCharacteristicController.hpp>
#include <controllers/ip/TcpServerSocket.hpp>
#include <controllers/threads/GenericThread.hpp>

namespace Lego
{

class ServerConnectionThread;

class ServerThread: public GenericThread
{
public:
    typedef std::vector<std::unique_ptr<ServerConnectionThread>> ConnectionThreads;

    ServerThread(
        TcpServerSocket& serverSocket,
        BluetoothGattCharacteristicController& legoLwpCharacteristic);
    virtual ~ServerThread();

    void broadcast(const ByteArray& data, ServerConnectionThread* except = nullptr);
    void broadcast(const std::string& data, ServerConnectionThread* except = nullptr);

protected:
    TcpServerSocket& m_serverSocket;
    BluetoothGattCharacteristicController& m_legoLwpCharacteristic;
    ConnectionThreads m_connectionThreads;

    void setup(void) override;
    void task(fd_set& descriptors) override;
    void shutdown(void) override;
    void updateDescriptors(int& maxDescriptor, fd_set& descriptors, timeval& timeout) override;

    void cleanUpFinishedConnectionThreads(void);

private:
    std::mutex m_connectionThreadsMutex;
};

} /* namespace Lego */

#endif /* CONTROLLERS_THREADS_SERVERTHREAD_HPP_ */

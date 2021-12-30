#ifndef CONTROLLERS_THREADS_SERVERCONNECTIONTHREAD_HPP_
#define CONTROLLERS_THREADS_SERVERCONNECTIONTHREAD_HPP_

#include <controllers/dbus/BluetoothGattCharacteristicController.hpp>
#include <controllers/ip/TcpSocket.hpp>
#include <controllers/threads/GenericThread.hpp>

namespace Lego
{

class ServerConnectionThread: public GenericThread
{
public:
    ServerConnectionThread(
        BluetoothGattCharacteristicController& legoLwpCharacteristic,
        int clientDescriptor,
        IpSocket::Protocol protocol,
        const std::string& peer);
    virtual ~ServerConnectionThread();

    void send(const ByteArray& data);

protected:
    BluetoothGattCharacteristicController& m_legoLwpCharacteristic;
    TcpSocket m_socket;

    void setup(void) override;
    void task(fd_set& descriptors) override;
    void shutdown(void) override;
    void updateDescriptors(int& maxDescriptor, fd_set& descriptors, timeval& timeout) override;

private:

};

} /* namespace Lego */

#endif /* CONTROLLERS_THREADS_SERVERCONNECTIONTHREAD_HPP_ */

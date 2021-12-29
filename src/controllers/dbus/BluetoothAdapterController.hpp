#ifndef CONTROLLERS_DBUS_BLUETOOTHADAPTERCONTROLLER_HPP_
#define CONTROLLERS_DBUS_BLUETOOTHADAPTERCONTROLLER_HPP_

#include <controllers/dbus/GenericBluetoothController.hpp>

namespace Lego
{

class BluetoothAdapterController: public GenericBluetoothController
{
public:
    BluetoothAdapterController(const std::string& bluetoothAdapterPath);
    virtual ~BluetoothAdapterController();

    void startDiscovery(void);
    void stopDiscovery(void);

    static ManagedObjects getAdapters(void);
    static ManagedObjects getAdapters(const ManagedObjects& input);

    static const std::string bluetoothAdapterInterfaceName;
    static const std::string startDiscoveryMethodName;
    static const std::string stopDiscoveryMethodName;

protected:

private:

};

} /* namespace Lego */

#endif /* CONTROLLERS_DBUS_BLUETOOTHADAPTERCONTROLLER_HPP_ */

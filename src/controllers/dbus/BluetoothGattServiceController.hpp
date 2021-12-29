#ifndef CONTROLLERS_DBUS_BLUETOOTHGATTSERVICECONTROLLER_HPP_
#define CONTROLLERS_DBUS_BLUETOOTHGATTSERVICECONTROLLER_HPP_

#include <controllers/dbus/GenericBluetoothController.hpp>

namespace Lego
{

class BluetoothGattServiceController: public GenericBluetoothController
{
public:
    BluetoothGattServiceController(const std::string& bluetoothServicePath);
    virtual ~BluetoothGattServiceController();

    static ManagedObjects getGattServices(void);
    static ManagedObjects getGattServices(const ManagedObjects& input);
    static ManagedObjects filterGattServicesByDevice(const ManagedObjects& input, const std::string& devicePath);
    static ManagedObjects filterGattServicesByService(const ManagedObjects& input, const std::string& serviceUuid);

    static const std::string bluetoothGattServiceInterfaceName;

protected:

private:

};

} /* namespace Lego */

#endif /* CONTROLLERS_DBUS_BLUETOOTHGATTSERVICECONTROLLER_HPP_ */

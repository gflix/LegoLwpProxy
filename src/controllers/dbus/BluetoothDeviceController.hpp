#ifndef CONTROLLERS_DBUS_BLUETOOTHDEVICECONTROLLER_HPP_
#define CONTROLLERS_DBUS_BLUETOOTHDEVICECONTROLLER_HPP_

#include <string>
#include <vector>
#include <controllers/dbus/GenericBluetoothController.hpp>

namespace Lego
{

class BluetoothDeviceController: public GenericBluetoothController
{
public:
    typedef std::vector<std::string> StringList;

    BluetoothDeviceController(const std::string& bluetoothAdapterPath);
    virtual ~BluetoothDeviceController();

    static ManagedObjects getDevices(void);
    static ManagedObjects getDevices(const ManagedObjects& input);
    static ManagedObjects filterDevicesByAdapter(const ManagedObjects& input, const std::string& adapterPath);
    static ManagedObjects filterDevicesByService(const ManagedObjects& input, const std::string& serviceUuid);

    static const std::string bluetoothDeviceInterfaceName;
    static const std::string adapterPropertyName;
    static const std::string uuidsPropertyName;

protected:

private:

};

} /* namespace Lego */

#endif /* CONTROLLERS_DBUS_BLUETOOTHDEVICECONTROLLER_HPP_ */

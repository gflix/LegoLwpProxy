#include <algorithm>
#include <controllers/dbus/BluetoothDeviceController.hpp>

namespace Lego
{

const std::string BluetoothDeviceController::bluetoothDeviceInterfaceName { "org.bluez.Device1" };
const std::string BluetoothDeviceController::adapterPropertyName { "Adapter" };
const std::string BluetoothDeviceController::uuidsPropertyName { "UUIDs" };

BluetoothDeviceController::BluetoothDeviceController(const std::string& bluetoothDevicePath):
    GenericBluetoothController(bluetoothDevicePath)
{
}

BluetoothDeviceController::~BluetoothDeviceController()
{
}

BluetoothDeviceController::ManagedObjects BluetoothDeviceController::getDevices(void)
{
    return getDevices(getManagedObjects());
}

BluetoothDeviceController::ManagedObjects BluetoothDeviceController::getDevices(
    const ManagedObjects& input)
{
    return filterManagedObjectsByImplementedInterface(input, bluetoothDeviceInterfaceName);
}

BluetoothDeviceController::ManagedObjects BluetoothDeviceController::filterDevicesByAdapter(
    const ManagedObjects& input,
    const std::string& adapterPath)
{
    ManagedObjects result;

    std::copy_if(input.cbegin(), input.cend(), std::inserter(result, result.begin()), [&](auto& path) {
        try
        {
            const sdbus::Variant& property =
                getProperty(path.second, bluetoothDeviceInterfaceName, adapterPropertyName);
            if (!property.containsValueOfType<sdbus::ObjectPath>())
            {
                return false;
            }
            return property.get<sdbus::ObjectPath>() == adapterPath;
        }
        catch(const std::exception&)
        {
            return false;
        }
    });

    return result;
}

BluetoothDeviceController::ManagedObjects BluetoothDeviceController::filterDevicesByService(
    const ManagedObjects& input,
    const std::string& serviceUuid)
{
    ManagedObjects result;

    std::copy_if(input.cbegin(), input.cend(), std::inserter(result, result.begin()), [&](auto& path) {
        try
        {
            const sdbus::Variant& property =
                getProperty(path.second, bluetoothDeviceInterfaceName, uuidsPropertyName);
            if (!property.containsValueOfType<StringList>())
            {
                return false;
            }

            auto uuids = property.get<StringList>();
            auto it = std::find_if(uuids.cbegin(), uuids.cend(), [&](auto& item) {
                return item == serviceUuid;
            });
            return it != uuids.cend();
        }
        catch(const std::exception&)
        {
            return false;
        }
    });

    return result;
}

} /* namespace Lego */

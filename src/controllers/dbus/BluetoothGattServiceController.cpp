#include <algorithm>
#include <controllers/dbus/BluetoothGattServiceController.hpp>

namespace Lego
{

const std::string BluetoothGattServiceController::bluetoothGattServiceInterfaceName { "org.bluez.GattService1" };

BluetoothGattServiceController::BluetoothGattServiceController(const std::string& bluetoothServicePath):
    GenericBluetoothController(bluetoothServicePath)
{
}

BluetoothGattServiceController::~BluetoothGattServiceController()
{
}

BluetoothGattServiceController::ManagedObjects BluetoothGattServiceController::getGattServices(void)
{
    return getGattServices(getManagedObjects());
}

BluetoothGattServiceController::ManagedObjects BluetoothGattServiceController::getGattServices(
    const ManagedObjects& input)
{
    return filterManagedObjectsByImplementedInterface(input, bluetoothGattServiceInterfaceName);
}

BluetoothGattServiceController::ManagedObjects BluetoothGattServiceController::filterGattServicesByDevice(
    const ManagedObjects& input,
    const std::string& devicePath)
{
    ManagedObjects result;

    std::copy_if(input.cbegin(), input.cend(), std::inserter(result, result.begin()), [&](auto& path) {
        try
        {
            const sdbus::Variant& property =
                getProperty(path.second, bluetoothGattServiceInterfaceName, devicePropertyName);
            if (!property.containsValueOfType<sdbus::ObjectPath>())
            {
                return false;
            }
            return property.get<sdbus::ObjectPath>() == devicePath;
        }
        catch(const std::exception&)
        {
            return false;
        }
    });

    return result;
}

BluetoothGattServiceController::ManagedObjects BluetoothGattServiceController::filterGattServicesByService(
    const ManagedObjects& input,
    const std::string& serviceUuid)
{
    ManagedObjects result;

    std::copy_if(input.cbegin(), input.cend(), std::inserter(result, result.begin()), [&](auto& path) {
        try
        {
            const sdbus::Variant& property =
                getProperty(path.second, bluetoothGattServiceInterfaceName, uuidPropertyName);
            if (!property.containsValueOfType<std::string>())
            {
                return false;
            }
            return property.get<std::string>() == serviceUuid;
        }
        catch(const std::exception&)
        {
            return false;
        }
    });

    return result;
}

} /* namespace Lego */

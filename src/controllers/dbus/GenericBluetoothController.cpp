#include <algorithm>
#include <stdexcept>
#include <controllers/dbus/GenericBluetoothController.hpp>

namespace Lego
{

const std::string GenericBluetoothController::bluetoothBusName { "org.bluez" };
const std::string GenericBluetoothController::objectManagerInterfaceName { "org.freedesktop.DBus.ObjectManager" };
const std::string GenericBluetoothController::getManagedObjectsMethodName { "GetManagedObjects" };

GenericBluetoothController::GenericBluetoothController(const std::string& bluetoothPath):
    m_path(bluetoothPath),
    m_bluetoothProxy(sdbus::createProxy(bluetoothBusName, bluetoothPath))
{
}

GenericBluetoothController::~GenericBluetoothController()
{
}

const std::string& GenericBluetoothController::path(void) const
{
    return m_path;
}

GenericBluetoothController::ManagedObjects GenericBluetoothController::getManagedObjects(void)
{
    ManagedObjects result;

    auto proxy = sdbus::createProxy(bluetoothBusName, "/");
    proxy->callMethod(getManagedObjectsMethodName).onInterface(objectManagerInterfaceName).storeResultsTo(result);

    return result;
}

GenericBluetoothController::ManagedObjects GenericBluetoothController::filterManagedObjectsByImplementedInterface(
    const GenericBluetoothController::ManagedObjects& input,
    const std::string& implementedInterface)
{
    ManagedObjects result;

    std::copy_if(input.cbegin(), input.cend(), std::inserter(result, result.begin()), [&](auto& path) {
        return path.second.find(implementedInterface) != path.second.cend();
    });

    return result;
}

const GenericBluetoothController::Property& GenericBluetoothController::getProperty(
    const ManagedObject& managedObject,
    const std::string& interface,
    const std::string& property)
{
    auto it = managedObject.find(interface);
    if (it == managedObject.cend())
    {
        throw std::out_of_range("requested interface \"" + property + "\" not found");
    }

    return getProperty(it->second, property);
}

const GenericBluetoothController::Property& GenericBluetoothController::getProperty(
    const Properties& properties,
    const std::string& property)
{
    auto it = properties.find(property);
    if (it == properties.cend())
    {
        throw std::out_of_range("requested property \"" + property + "\" not found");
    }

    return it->second;
}


} /* namespace Lego */

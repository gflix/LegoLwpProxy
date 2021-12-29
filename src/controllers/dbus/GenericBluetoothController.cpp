#include <algorithm>
#include <stdexcept>
#include <controllers/dbus/GenericBluetoothController.hpp>

namespace Lego
{

const std::string GenericBluetoothController::bluetoothBusName { "org.bluez" };
const std::string GenericBluetoothController::objectManagerInterfaceName { "org.freedesktop.DBus.ObjectManager" };
const std::string GenericBluetoothController::propertiesInterfaceName { "org.freedesktop.DBus.Properties" };
const std::string GenericBluetoothController::getAllMethodName { "GetAll" };
const std::string GenericBluetoothController::getManagedObjectsMethodName { "GetManagedObjects" };
const std::string GenericBluetoothController::adapterPropertyName { "Adapter" };
const std::string GenericBluetoothController::connectedPropertyName { "Connected" };
const std::string GenericBluetoothController::devicePropertyName { "Device" };
const std::string GenericBluetoothController::namePropertyName { "Name" };
const std::string GenericBluetoothController::notifyingPropertyName { "Notifying" };
const std::string GenericBluetoothController::servicePropertyName { "Service" };
const std::string GenericBluetoothController::servicesResolvedPropertyName { "ServicesResolved" };
const std::string GenericBluetoothController::uuidPropertyName { "UUID" };
const std::string GenericBluetoothController::uuidsPropertyName { "UUIDs" };
const std::string GenericBluetoothController::valuePropertyName { "Value" };
const std::string GenericBluetoothController::propertiesChangedSignalName { "PropertiesChanged" };

GenericBluetoothController::GenericBluetoothController(const std::string& bluetoothPath):
    m_path(bluetoothPath),
    m_bluetoothProxy(sdbus::createProxy(bluetoothBusName, bluetoothPath))
{
    m_bluetoothProxy->uponSignal(propertiesChangedSignalName).onInterface(propertiesInterfaceName).
        call([this](const std::string& interface, const Properties& changedProperties, const StringList& invalidatedProperties) {
            this->onPropertiesChanged(interface, changedProperties, invalidatedProperties);
        });
    m_bluetoothProxy->finishRegistration();  // TODO this prevents sub classing
}

GenericBluetoothController::~GenericBluetoothController()
{
}

const std::string& GenericBluetoothController::path(void) const
{
    return m_path;
}

GenericBluetoothController::Properties GenericBluetoothController::properties(const std::string& interface)
{
    Properties result;

    m_bluetoothProxy->callMethod(getAllMethodName).onInterface(propertiesInterfaceName).withArguments(interface).storeResultsTo(result);

    return result;
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

void GenericBluetoothController::onPropertiesChanged(
    const std::string& interface,
    const Properties& changedProperties,
    const StringList& invalidatedProperties)
{
}

} /* namespace Lego */

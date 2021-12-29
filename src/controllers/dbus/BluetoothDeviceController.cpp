#include <algorithm>
#include <iostream>
#include <controllers/dbus/BluetoothDeviceController.hpp>
#include <controllers/dbus/BluetoothGattServiceController.hpp>

namespace Lego
{

const std::string BluetoothDeviceController::bluetoothDeviceInterfaceName { "org.bluez.Device1" };
const std::string BluetoothDeviceController::connectMethodName { "Connect" };
const std::string BluetoothDeviceController::disconnectMethodName { "Disconnect" };
const std::string BluetoothDeviceController::legoLwpServiceUuid { "00001623-1212-efde-1623-785feabcd123" };

BluetoothDeviceController::BluetoothDeviceController(const std::string& bluetoothDevicePath):
    GenericBluetoothController(bluetoothDevicePath),
    m_connected(false),
    m_servicesResolved(false)
{
    refreshDeviceProperties();
}

BluetoothDeviceController::~BluetoothDeviceController()
{
}

const BluetoothDeviceController::Properties& BluetoothDeviceController::deviceProperties(void) const
{
    return m_deviceProperties;
}

bool BluetoothDeviceController::connected(void) const
{
    return m_connected;
}

bool BluetoothDeviceController::servicesResolved(void) const
{
    return m_servicesResolved;
}

const BluetoothDeviceController::ManagedObjects& BluetoothDeviceController::services(void) const
{
    return m_services;
}

void BluetoothDeviceController::refreshDeviceProperties(void)
{
    m_deviceProperties = properties(bluetoothDeviceInterfaceName);
}

void BluetoothDeviceController::connect(void)
{
    m_bluetoothProxy->callMethod(connectMethodName).onInterface(bluetoothDeviceInterfaceName).storeResultsTo();
}

void BluetoothDeviceController::disconnect(void)
{
    m_bluetoothProxy->callMethod(disconnectMethodName).onInterface(bluetoothDeviceInterfaceName).storeResultsTo();
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

std::string BluetoothDeviceController::getName(const Properties& properties)
{
    auto& property = getProperty(properties, namePropertyName);
    if (!property.containsValueOfType<std::string>())
    {
        return "[unknown]";
    }
    return property.get<std::string>();
}

bool BluetoothDeviceController::hasLegoLwpService(const Properties& properties)
{
    auto& property = getProperty(properties, uuidsPropertyName);
    if (!property.containsValueOfType<StringList>())
    {
        return false;
    }

    auto uuids = property.get<StringList>();
    auto it = std::find_if(uuids.cbegin(), uuids.cend(), [&](auto& item) {
        return item == legoLwpServiceUuid;
    });
    return it != uuids.cend();
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

void BluetoothDeviceController::onPropertiesChanged(
    const std::string& interface,
    const Properties& changedProperties,
    const StringList& invalidatedProperties)
{
    if (interface == bluetoothDeviceInterfaceName)
    {
        onDevicePropertiesChanged(changedProperties, invalidatedProperties);
    }
}

void BluetoothDeviceController::onDevicePropertiesChanged(
    const Properties& changedProperties,
    const StringList& invalidatedProperties)
{
    for (auto& invalidatedProperty: invalidatedProperties)
    {
        onDevicePropertyRemoved(invalidatedProperty);
        m_deviceProperties.erase(invalidatedProperty);
    }

    for (auto& changedProperty: changedProperties)
    {
        onDevicePropertyChanged(changedProperty.first, changedProperty.second);
        m_deviceProperties[changedProperty.first] = changedProperty.second;
    }
}

void BluetoothDeviceController::onDevicePropertyRemoved(const std::string& invalidatedProperty)
{
    std::cout << "BluetoothDeviceController::onDevicePropertyRemoved(" << invalidatedProperty << ")" << std::endl;
}

void BluetoothDeviceController::onDevicePropertyChanged(const std::string& key, const Property& value)
{
    if (key == connectedPropertyName)
    {
        if (value.containsValueOfType<bool>())
        {
            m_connected = value.get<bool>();
        }
    }
    else if (key == servicesResolvedPropertyName)
    {
        if (value.containsValueOfType<bool>())
        {
            m_servicesResolved = value.get<bool>();
            onDeviceServicesResolved(m_servicesResolved);
        }
    }
}

void BluetoothDeviceController::onDeviceServicesResolved(bool state)
{
    if (state)
    {
        m_services = BluetoothGattServiceController::filterGattServicesByDevice(
            BluetoothGattServiceController::getGattServices(),
            m_path);
    }
    else
    {
        m_services.clear();
    }
}

} /* namespace Lego */

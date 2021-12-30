#include <algorithm>
#include <iostream>
#include <controllers/dbus/BluetoothGattCharacteristicController.hpp>
#include <controllers/threads/ServerThread.hpp>
#include <protocols/ByteArray.hpp>
#include <utils/Log.hpp>

namespace Lego
{

const std::string BluetoothGattCharacteristicController::bluetoothGattCharacteristicInterfaceName { "org.bluez.GattCharacteristic1" };
const std::string BluetoothGattCharacteristicController::startNotifyMethodName { "StartNotify" };
const std::string BluetoothGattCharacteristicController::stopNotifyMethodName { "StopNotify" };
const std::string BluetoothGattCharacteristicController::legoLwpCharacteristicUuid { "00001624-1212-efde-1623-785feabcd123" };

BluetoothGattCharacteristicController::BluetoothGattCharacteristicController(const std::string& bluetoothServicePath):
    GenericBluetoothController(bluetoothServicePath),
    m_serverThread(nullptr)
{
    refreshGattCharacteristicProperties();
}

BluetoothGattCharacteristicController::~BluetoothGattCharacteristicController()
{
    stopNotifications();
}

const BluetoothGattCharacteristicController::Properties& BluetoothGattCharacteristicController::gattCharacteristicProperties(void) const
{
    return m_gattCharacteristicProperties;
}

const BluetoothGattCharacteristicController::Bytes& BluetoothGattCharacteristicController::data(void) const
{
    return m_data;
}

void BluetoothGattCharacteristicController::refreshGattCharacteristicProperties(void)
{
    m_gattCharacteristicProperties = properties(bluetoothGattCharacteristicInterfaceName);
}

void BluetoothGattCharacteristicController::restartNotifications(void)
{
    stopNotifications();
    m_bluetoothProxy->callMethod("StartNotify").onInterface(bluetoothGattCharacteristicInterfaceName);
}

void BluetoothGattCharacteristicController::setServerThread(ServerThread* serverThread)
{
    m_serverThread = serverThread;
}

void BluetoothGattCharacteristicController::stopNotifications(void)
{
    try
    {
        m_bluetoothProxy->callMethod("StopNotify").onInterface(bluetoothGattCharacteristicInterfaceName);
    }
    catch(const std::exception&)
    {
        // silently skip
    }
}

BluetoothGattCharacteristicController::ManagedObjects BluetoothGattCharacteristicController::getGattCharacteristics(void)
{
    return getGattCharacteristics(getManagedObjects());
}

BluetoothGattCharacteristicController::ManagedObjects BluetoothGattCharacteristicController::getGattCharacteristics(
    const ManagedObjects& input)
{
    return filterManagedObjectsByImplementedInterface(input, bluetoothGattCharacteristicInterfaceName);
}

BluetoothGattCharacteristicController::ManagedObjects BluetoothGattCharacteristicController::filterGattCharacteristicsByService(
    const ManagedObjects& input,
    const std::string& serviceUuid)
{
    ManagedObjects result;

    std::copy_if(input.cbegin(), input.cend(), std::inserter(result, result.begin()), [&](auto& path) {
        try
        {
            const sdbus::Variant& property =
                getProperty(path.second, bluetoothGattCharacteristicInterfaceName, servicePropertyName);
            if (!property.containsValueOfType<sdbus::ObjectPath>())
            {
                return false;
            }
            return property.get<sdbus::ObjectPath>() == serviceUuid;
        }
        catch(const std::exception&)
        {
            return false;
        }
    });

    return result;
}

BluetoothGattCharacteristicController::ManagedObjects BluetoothGattCharacteristicController::filterGattCharacteristicsByCharacteristic(
    const ManagedObjects& input,
    const std::string& characteristicUuid)
{
    ManagedObjects result;

    std::copy_if(input.cbegin(), input.cend(), std::inserter(result, result.begin()), [&](auto& path) {
        try
        {
            const sdbus::Variant& property =
                getProperty(path.second, bluetoothGattCharacteristicInterfaceName, uuidPropertyName);
            if (!property.containsValueOfType<std::string>())
            {
                return false;
            }
            return property.get<std::string>() == characteristicUuid;
        }
        catch(const std::exception&)
        {
            return false;
        }
    });

    return result;
}

void BluetoothGattCharacteristicController::onPropertiesChanged(
    const std::string& interface,
    const Properties& changedProperties,
    const StringList& invalidatedProperties)
{
    if (interface == bluetoothGattCharacteristicInterfaceName)
    {
        onGattCharacteristicPropertiesChanged(changedProperties, invalidatedProperties);
    }
}

void BluetoothGattCharacteristicController::onGattCharacteristicPropertiesChanged(
    const Properties& changedProperties,
    const StringList& invalidatedProperties)
{
    for (auto& invalidatedProperty: invalidatedProperties)
    {
        onGattCharacteristicPropertyRemoved(invalidatedProperty);
        m_gattCharacteristicProperties.erase(invalidatedProperty);
    }

    for (auto& changedProperty: changedProperties)
    {
        onGattCharacteristicPropertyChanged(changedProperty.first, changedProperty.second);
        m_gattCharacteristicProperties[changedProperty.first] = changedProperty.second;
    }
}

void BluetoothGattCharacteristicController::onGattCharacteristicPropertyRemoved(const std::string& invalidatedProperty)
{
    LOG_DEBUG("BluetoothGattCharacteristicController::onGattCharacteristicPropertyRemoved(" << invalidatedProperty << ")");
}

void BluetoothGattCharacteristicController::onGattCharacteristicPropertyChanged(const std::string& key, const Property& value)
{
    if (key == valuePropertyName && value.containsValueOfType<std::vector<uint8_t>>())
    {
        m_data = value.get<std::vector<uint8_t>>();
        onGattCharacteristicDataReceived(m_data);
    }
}

void BluetoothGattCharacteristicController::onGattCharacteristicDataReceived(const Bytes& data)
{
    LOG_DEBUG(
        "BluetoothGattCharacteristicController::onGattCharacteristicDataReceived("
        "size=" << data.size() << ","
        "data=" << Protocol::ByteArray::toHexString(Protocol::ByteArray::fromVector(data)) << ")");

    if (m_serverThread)
    {
        auto text = Protocol::ByteArray::toHexString(Protocol::ByteArray::fromVector(data)) + "\r\n";
        m_serverThread->broadcast(text);
    }
}

} /* namespace Lego */

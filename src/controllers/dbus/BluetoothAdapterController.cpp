#include <controllers/dbus/BluetoothAdapterController.hpp>

namespace Lego
{

const std::string BluetoothAdapterController::bluetoothAdapterInterfaceName { "org.bluez.Adapter1" };
const std::string BluetoothAdapterController::startDiscoveryMethodName { "StartDiscovery" };
const std::string BluetoothAdapterController::stopDiscoveryMethodName { "StopDiscovery" };

BluetoothAdapterController::BluetoothAdapterController(const std::string& bluetoothAdapterPath):
    GenericBluetoothController(bluetoothAdapterPath)
{
}

BluetoothAdapterController::~BluetoothAdapterController()
{
}

void BluetoothAdapterController::startDiscovery(void)
{
    m_bluetoothProxy->callMethod(startDiscoveryMethodName).onInterface(bluetoothAdapterInterfaceName).storeResultsTo();
}

void BluetoothAdapterController::stopDiscovery(void)
{
    m_bluetoothProxy->callMethod(stopDiscoveryMethodName).onInterface(bluetoothAdapterInterfaceName).storeResultsTo();
}

BluetoothAdapterController::ManagedObjects BluetoothAdapterController::getAdapters(void)
{
    return getAdapters(getManagedObjects());
}

BluetoothAdapterController::ManagedObjects BluetoothAdapterController::getAdapters(const ManagedObjects& input)
{
    return filterManagedObjectsByImplementedInterface(input, bluetoothAdapterInterfaceName);
}

} /* namespace Lego */

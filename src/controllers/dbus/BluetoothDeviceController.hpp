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
    BluetoothDeviceController(const std::string& bluetoothAdapterPath);
    virtual ~BluetoothDeviceController();


    const Properties& deviceProperties(void) const;
    bool connected(void) const;
    bool servicesResolved(void) const;
    const ManagedObjects& services(void) const;

    void refreshDeviceProperties(void);

    void connect(void);
    void disconnect(void);

    static ManagedObjects getDevices(void);
    static ManagedObjects getDevices(const ManagedObjects& input);
    static std::string getName(const Properties& properties);
    static bool hasLegoLwpService(const Properties& properties);
    static ManagedObjects filterDevicesByAdapter(const ManagedObjects& input, const std::string& adapterPath);
    static ManagedObjects filterDevicesByService(const ManagedObjects& input, const std::string& serviceUuid);

    static const std::string bluetoothDeviceInterfaceName;
    static const std::string connectMethodName;
    static const std::string disconnectMethodName;
    static const std::string legoLwpServiceUuid;

protected:

    Properties m_deviceProperties;
    bool m_connected;
    bool m_servicesResolved;
    ManagedObjects m_services;

    void onPropertiesChanged(
        const std::string& interface,
        const Properties& changedProperties,
        const StringList& invalidatedProperties) override;

    virtual void onDevicePropertiesChanged(
        const Properties& changedProperties,
        const StringList& invalidatedProperties);

    virtual void onDevicePropertyRemoved(const std::string& invalidatedProperty);
    virtual void onDevicePropertyChanged(const std::string& key, const Property& value);
    virtual void onDeviceServicesResolved(bool state);

private:

};

} /* namespace Lego */

#endif /* CONTROLLERS_DBUS_BLUETOOTHDEVICECONTROLLER_HPP_ */

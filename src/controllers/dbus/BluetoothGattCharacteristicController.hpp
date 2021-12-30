#ifndef CONTROLLERS_DBUS_BLUETOOTHGATTCHARACTERISTICCONTROLLER_HPP_
#define CONTROLLERS_DBUS_BLUETOOTHGATTCHARACTERISTICCONTROLLER_HPP_

#include <controllers/dbus/GenericBluetoothController.hpp>

namespace Lego
{

class BluetoothGattCharacteristicController: public GenericBluetoothController
{
public:
    typedef std::vector<uint8_t> Bytes;

    BluetoothGattCharacteristicController(const std::string& bluetoothServicePath);
    virtual ~BluetoothGattCharacteristicController();

    const Properties& gattCharacteristicProperties(void) const;
    const Bytes& data(void) const;

    void refreshGattCharacteristicProperties(void);

    static ManagedObjects getGattCharacteristics(void);
    static ManagedObjects getGattCharacteristics(const ManagedObjects& input);
    static ManagedObjects filterGattCharacteristicsByService(const ManagedObjects& input, const std::string& serviceUuid);
    static ManagedObjects filterGattCharacteristicsByCharacteristic(const ManagedObjects& input, const std::string& characteristicUuid);

    static const std::string bluetoothGattCharacteristicInterfaceName;
    static const std::string legoLwpCharacteristicUuid;

protected:

    Properties m_gattCharacteristicProperties;
    Bytes m_data;

    void onPropertiesChanged(
        const std::string& interface,
        const Properties& changedProperties,
        const StringList& invalidatedProperties) override;

    virtual void onGattCharacteristicPropertiesChanged(
        const Properties& changedProperties,
        const StringList& invalidatedProperties);

    virtual void onGattCharacteristicPropertyRemoved(const std::string& invalidatedProperty);
    virtual void onGattCharacteristicPropertyChanged(const std::string& key, const Property& value);
    virtual void onGattCharacteristicDataReceived(const Bytes& data);

private:

};

} /* namespace Lego */

#endif /* CONTROLLERS_DBUS_BLUETOOTHGATTCHARACTERISTICCONTROLLER_HPP_ */
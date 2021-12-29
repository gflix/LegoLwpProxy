#ifndef CONTROLLERS_DBUS_GENERICBLUETOOTHCONTROLLER_HPP_
#define CONTROLLERS_DBUS_GENERICBLUETOOTHCONTROLLER_HPP_

#include <map>
#include <string>
#include <sdbus-c++/sdbus-c++.h>

namespace Lego
{

class GenericBluetoothController
{
public:

    typedef std::vector<std::string> StringList;
    typedef sdbus::Variant Property;
    typedef std::map<std::string, Property> Properties;
    typedef std::map<std::string, Properties> ManagedObject;
    typedef std::map<sdbus::ObjectPath, ManagedObject> ManagedObjects;

    GenericBluetoothController(const std::string& bluetoothPath);
    virtual ~GenericBluetoothController();

    const std::string& path(void) const;
    Properties properties(const std::string& interface);

    static ManagedObjects getManagedObjects(void);
    static ManagedObjects filterManagedObjectsByImplementedInterface(
        const ManagedObjects& input,
        const std::string& implementedInterface);
    static const Property& getProperty(
        const ManagedObject& managedObject,
        const std::string& interface,
        const std::string& property);
    static const Property& getProperty(
        const Properties& properties,
        const std::string& property);

    static const std::string bluetoothBusName;
    static const std::string objectManagerInterfaceName;
    static const std::string propertiesInterfaceName;
    static const std::string getAllMethodName;
    static const std::string getManagedObjectsMethodName;
    static const std::string adapterPropertyName;
    static const std::string connectedPropertyName;
    static const std::string devicePropertyName;
    static const std::string namePropertyName;
    static const std::string notifyingPropertyName;
    static const std::string servicePropertyName;
    static const std::string servicesResolvedPropertyName;
    static const std::string uuidPropertyName;
    static const std::string uuidsPropertyName;
    static const std::string valuePropertyName;
    static const std::string propertiesChangedSignalName;

protected:

    std::string m_path;
    std::unique_ptr<sdbus::IProxy> m_bluetoothProxy;

    virtual void onPropertiesChanged(
        const std::string& interface,
        const Properties& changedProperties,
        const StringList& invalidatedProperties);

private:

};

} /* namespace Lego */

#endif /* CONTROLLERS_DBUS_GENERICBLUETOOTHCONTROLLER_HPP_ */

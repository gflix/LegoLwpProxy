#include <iostream>
#include <controllers/dbus/BluetoothAdapterController.hpp>
#include <controllers/dbus/BluetoothDeviceController.hpp>

using namespace std;

void printUsage(const std::string& programName)
{
    cout << "USAGE: " << programName << " COMMAND [ARGUMENTS]" << endl;
    cout << endl;
    cout << "Possible commands:" << endl;
    cout << "  adapters  List the available bluetooth adapters and exit;" << endl;
    cout << "            no arguments required" << endl;
    cout << "  devices   Scan for usable devices and exit;" << endl;
    cout << "            requires path to an adapter (i.e. /org/bluez/hci0)" << endl;
    cout << "  connect   Connect to a given device and provide its LWP service on a TCP port" << endl;
    cout << "            (i.e. /org/bluez/hci0/dev_90_84_2B_1C_99_10 4711)" << endl; 
    cout << endl;
}

void listAdapters(void)
{
    auto bluetoothAdapters = Lego::BluetoothAdapterController::getAdapters();
    cout << "Found " << bluetoothAdapters.size() << " bluetooth adapters:" << endl;
    for (auto& bluetoothAdapter: bluetoothAdapters)
    {
        cout << "  " << bluetoothAdapter.first << endl;
    }
}

void scanForUsableDevices(const vector<string>& arguments)
{
    if (arguments.empty())
    {
        throw std::invalid_argument("\"devices\" command requires an argument");
    }

    string adapterPath = arguments.front();
    Lego::BluetoothAdapterController bluetoothAdapterController { adapterPath };

    cout << "Discovering devices using \"" << adapterPath << "\"..." << endl;
    bluetoothAdapterController.startDiscovery();
    sleep(3);
    bluetoothAdapterController.stopDiscovery();

    auto bluetoothDevices = Lego::BluetoothDeviceController::filterDevicesByAdapter(
        Lego::BluetoothDeviceController::getDevices(),
        adapterPath);
    cout << "Found " << bluetoothDevices.size() << " devices in total." << endl;

    bluetoothDevices = Lego::BluetoothDeviceController::filterDevicesByService(
        bluetoothDevices,
        "00001623-1212-efde-1623-785feabcd123");
    cout << "Found " << bluetoothDevices.size() << " devices after filtering by LEGO LWP service:" << endl;
    for (auto& bluetoothDevice: bluetoothDevices)
    {
        string deviceName { "[unknown]" };
        try
        {
            auto property = Lego::BluetoothDeviceController::getProperty(
                bluetoothDevice.second,
                Lego::BluetoothDeviceController::bluetoothDeviceInterfaceName,
                "Name");
            if (property.containsValueOfType<std::string>())
            {
                deviceName = property.get<std::string>();
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        cout << "  " << bluetoothDevice.first << " -> " << deviceName << endl;
    }
}

void connectToDeviceAndStartProxy(const vector<string>& arguments)
{
    if (arguments.size() < 2)
    {
        throw std::invalid_argument("\"connect\" command requires two arguments, a device path and a TCP port");
    }
    string devicePath = arguments[0];
    int tcpServerPort = stoi(arguments[1]);

    cout << "Going to connect to device \"" << devicePath << " and starting the proxy on port " << tcpServerPort << "..." << endl;
}

int main(int argc, char const* argv[])
{
    string programName { argv[0] };
    string command;
    vector<string> arguments;

    if (argc >= 1)
    {
        command = { argv[1] };
        for (auto i = 2; i < argc; ++i)
        {
            arguments.push_back(argv[i]);
        }
    }

    try
    {
        if (command == "adapters")
        {
            listAdapters();
        }
        else if (command == "devices")
        {
            scanForUsableDevices(arguments);
        }
        else if (command == "connect")
        {
            connectToDeviceAndStartProxy(arguments);
        }
        else
        {
            printUsage(programName);
            return -1;
        }
    }
    catch(const std::exception& e)
    {
        cerr << "Caught an exception (" << e.what() << ")! Aborting." << endl;
        return -1;
    }

    return 0;
}

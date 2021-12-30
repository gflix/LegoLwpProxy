#include <signal.h>
#include <iostream>
#include <controllers/dbus/BluetoothAdapterController.hpp>
#include <controllers/dbus/BluetoothDeviceController.hpp>
#include <controllers/dbus/BluetoothGattCharacteristicController.hpp>
#include <controllers/dbus/BluetoothGattServiceController.hpp>
#include <controllers/ip/TcpServerSocket.hpp>
#include <controllers/threads/ServerThread.hpp>
#include <utils/Log.hpp>

#define MAIN_LOOP_SLEEP_USEC (3000000)

using namespace std;
using namespace Lego;

static bool terminationFlag = false;

void terminationHandler(int signal)
{
    LOG_NOTICE("Caught signal " << signal << ". Stopping threads.");
    terminationFlag = true;
}

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
    auto bluetoothAdapters = BluetoothAdapterController::getAdapters();
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
    BluetoothAdapterController bluetoothAdapterController { adapterPath };

    cout << "Discovering devices using \"" << adapterPath << "\"..." << endl;
    bluetoothAdapterController.startDiscovery();
    sleep(3);
    bluetoothAdapterController.stopDiscovery();

    auto bluetoothDevices = BluetoothDeviceController::filterDevicesByAdapter(
        BluetoothDeviceController::getDevices(),
        adapterPath);
    cout << "Found " << bluetoothDevices.size() << " devices in total." << endl;

    bluetoothDevices = BluetoothDeviceController::filterDevicesByService(
        bluetoothDevices,
        BluetoothDeviceController::legoLwpServiceUuid);
    cout << "Found " << bluetoothDevices.size() << " devices after filtering by LEGO LWP service:" << endl;
    for (auto& bluetoothDevice: bluetoothDevices)
    {
        string deviceName { "[unknown]" };
        try
        {
            auto property = BluetoothDeviceController::getProperty(
                bluetoothDevice.second,
                BluetoothDeviceController::bluetoothDeviceInterfaceName,
                BluetoothDeviceController::namePropertyName);
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

    LOG_INFO("Going to connect to device \"" << devicePath << " ...");

    BluetoothDeviceController bluetoothDeviceController { devicePath };
    auto deviceProperties = bluetoothDeviceController.deviceProperties();
    if (!bluetoothDeviceController.hasLegoLwpService(deviceProperties))
    {
        throw std::runtime_error("LEGO LWP was not advertised by the device");
    }
    bluetoothDeviceController.connect();
    LOG_NOTICE("Successfully connected to \"" << bluetoothDeviceController.getName(deviceProperties) << "\".");

    signal(SIGTERM, terminationHandler);
    signal(SIGHUP, terminationHandler);
    signal(SIGINT, terminationHandler);

    try
    {
        while (!terminationFlag && bluetoothDeviceController.connected() && !bluetoothDeviceController.servicesResolved())
        {
            usleep(MAIN_LOOP_SLEEP_USEC >> 2);
        }
        if (bluetoothDeviceController.servicesResolved())
        {
            LOG_INFO("Trying to connect to the LEGO LWP service...");
            auto gattServices = BluetoothGattServiceController::filterGattServicesByService(
                bluetoothDeviceController.services(),
                BluetoothDeviceController::legoLwpServiceUuid);
            if (gattServices.size() != 1)
            {
                throw std::runtime_error("error resolving the LEGO LWP GATT service");
            }

            auto characteristics = BluetoothGattCharacteristicController::filterGattCharacteristicsByCharacteristic(
                BluetoothGattCharacteristicController::filterGattCharacteristicsByService(
                    BluetoothGattCharacteristicController::getGattCharacteristics(),
                    gattServices.cbegin()->first),
                BluetoothGattCharacteristicController::legoLwpCharacteristicUuid);

            if (characteristics.size() != 1)
            {
                throw std::runtime_error("error resolving the LEGO LWP GATT characteristic");
            }

            BluetoothGattCharacteristicController legoLwpCharacteristic { characteristics.cbegin()->first };

            TcpServerSocket tcpServerSocket;
            LOG_INFO("Trying to open the TCP server port...");
            tcpServerSocket.open(tcpServerPort, IpSocket::Protocol::IPV6);
            ServerThread proxyServerThread { tcpServerSocket, legoLwpCharacteristic };

            proxyServerThread.start();

            while (!terminationFlag && bluetoothDeviceController.connected())
            {
                usleep(MAIN_LOOP_SLEEP_USEC);
            }

            proxyServerThread.stop();
            tcpServerSocket.close();
        }

        if (!bluetoothDeviceController.connected())
        {
            LOG_NOTICE("Connection to device lost. Shutting down.");
        }
    }
    catch(const std::exception& e)
    {
        bluetoothDeviceController.disconnect();
        LOG_INFO("Disconnected from \"" << bluetoothDeviceController.getName(deviceProperties) << "\".");
        throw;
    }

    bluetoothDeviceController.disconnect();
    LOG_INFO("Disconnected from \"" << bluetoothDeviceController.getName(deviceProperties) << "\".");
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

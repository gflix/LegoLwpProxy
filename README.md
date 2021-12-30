# LegoLwpProxy

## Intention

This proxy server connects to a Lego Bluetooth device and proxies the received notifications to the connected
TCP clients and vice versa sends the received messages back to the Lego device.

This proxy allows to separate the Bluetooth part from the protocol part which in turn allows to separate the Bluetooth
hardware from the controlling application. So the proxy itself does not have any protocol knowledge except of the two UUIDs required to identify the Lego LWP service and the GATT characteristic where every communication happens.

## Prerequisites

### Compiling

1. Get the source code
2. Ensure the following packages are installed:

    - C++ compiler which supports C++17
    - CMake
    - libsdbus-c++ (https://github.com/Kistler-Group/sdbus-cpp)
    - bluez
    - on Ubuntu 20.10 and up you can install the packages using

```
$ sudo apt install build-essential cmake libsdbus-c++-dev bluez
```

3. Within the source code directory run

```
$ cmake
```

4. Build the sources

```
$ make
```

### Required hardware

1. A PC, notebook or RaspberryPi-alike computer which has a BLE (Bluetooth Low Energy) compliant adapter
2. A Lego Bluetooth device like the Technic Hub (Technic sets), the Smart Hub (i.e. Train sets), Move Hub (i.e. Lego Boost), Super Mario etc.

## Setup

The typical setup will be like this:

```
[Lego device] <-> [Bluetooth Adapter] <-> [LegoLwpProxy] <-> [Application]
```

## Usage

The proxy currently uses the native names used within the DBus system. To establish a connection first the Bluetooth adapter needs to be known, then a device scan has to be done and at last the connection can be set up:

1. Show the available adapters:

```
$ ./src/lego-lwp-proxy adapters
Found 1 bluetooth adapters:
  /org/bluez/hci0
```

2. Scan for devices on a specific adapter:

```
$ ./src/lego-lwp-proxy devices /org/bluez/hci0
Discovering devices using "/org/bluez/hci0"...
Found 7 devices in total.
Found 4 devices after filtering by LEGO LWP service:
  /org/bluez/hci0/dev_04_EE_03_BE_36_xx -> Handset
  /org/bluez/hci0/dev_60_77_71_35_CD_xx -> LEGO Mario    
  /org/bluez/hci0/dev_90_84_2B_1C_99_xx -> Smart Hub
  /org/bluez/hci0/dev_90_84_2B_60_93_xx -> Technic Hub
```

3. Connect to a specific device and start the proxy on a specific port (here 4711):

```
$ ./src/lego-lwp-proxy connect /org/bluez/hci0/dev_90_84_2B_1C_99_xx 4711
2021-12-30 13:57:53 [I] Going to connect to device "/org/bluez/hci0/dev_90_84_2B_1C_99_10 ...
2021-12-30 13:57:55 [N] Successfully connected to "Smart Hub".
2021-12-30 13:57:56 [I] Trying to connect to the LEGO LWP service...
2021-12-30 13:57:56 [I] Trying to open the TCP server port...
2021-12-30 13:57:56 [I] Started thread "ServerThread"
```

4. Now the proxy is reachable through port 4711. After the connection was established the Lego device will send a number of notifications to indicate the current hardware state:

```
$ telnet ::1 4711
Trying ::1...
Connected to ::1.
Escape character is '^]'.
0F0004320117000000001000000010
0F00043B0115000200000002000000
0F00043C0114000200000002000000
```

5. A user may send back a message (i.e. to switch the LED color). Most of the messages are confirmed by the Lego device. So the first line is the command to switch the LED color and the second line is the response of the Lego device:

```
0800813211510005
050082320A
```

6. To shut down the proxy press CTRL+C or kill the process:

```
^C
2021-12-30 13:58:34 [N] Caught signal 2. Stopping threads.
2021-12-30 13:58:34 [D] Thread "ServerThread" received quit signal.
2021-12-30 13:58:34 [I] Stopped thread "ServerThread"
2021-12-30 13:58:36 [I] Disconnected from "Smart Hub".
```

## Hints

- The BlueZ bluetooth stack automatically removes devices from its cache when there was no successful communication in the past, so it can be necessary to scan for new devices again before connecting to a specific device
- When connecting to a Lego device the device must be switched on and in advertising state which is shown by a blinking white LED or the moving dots on Mario's belly
- When a Lego device is switched off or gets out of reach the Bluetooth connection will be closed and the proxy will shutdown; to reconnect the proxy must be restarted
- Whenever a TCP client connects to the proxy the Bluetooth GATT notifications are switched off and on for a short moment to force the Lego device to resend its current hardware state again; this might be a security issue as third party clients may get the information, that new clients are connected to the proxy

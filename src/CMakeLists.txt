include_directories(
    PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src
)

add_executable(
    lego-lwp-proxy
    LegoLwpProxy.cpp
    controllers/GenericDescriptor.cpp
    controllers/dbus/BluetoothAdapterController.cpp
    controllers/dbus/BluetoothDeviceController.cpp
    controllers/dbus/BluetoothGattCharacteristicController.cpp
    controllers/dbus/BluetoothGattServiceController.cpp
    controllers/dbus/GenericBluetoothController.cpp
    controllers/ip/IpSocket.cpp
    controllers/ip/TcpServerSocket.cpp
    controllers/ip/TcpSocket.cpp
    models/ByteArray.cpp
    models/LogLevel.cpp
    protocols/ByteArray.cpp
    utils/Log.cpp
)

target_link_libraries(
    lego-lwp-proxy
    PRIVATE
    SDBusCpp::sdbus-c++
)

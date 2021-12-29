include_directories(
    PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src
)

add_executable(
    lego-lwp-proxy
    LegoLwpProxy.cpp
    controllers/dbus/BluetoothAdapterController.cpp
    controllers/dbus/BluetoothDeviceController.cpp
    controllers/dbus/BluetoothGattCharacteristicController.cpp
    controllers/dbus/BluetoothGattServiceController.cpp
    controllers/dbus/GenericBluetoothController.cpp
)

target_link_libraries(
    lego-lwp-proxy
    PRIVATE
    SDBusCpp::sdbus-c++
)

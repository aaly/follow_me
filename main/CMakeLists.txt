set (CMAKE_CXX_STANDARD 11)

idf_component_register(
    SRCS "main.cpp" "services/RoverCService.cpp" "services/BluetoothService.cpp" "services/PanicService.cpp"
    INCLUDE_DIRS "lib"
)

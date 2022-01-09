set (CMAKE_CXX_STANDARD 11)

idf_component_register(
    SRCS "main.cpp" "services/Event.cpp" "services/Service.cpp" "services/RoverCService.cpp"
    "services/BluetoothService.cpp" "services/ServiceManager.cpp" "services/DisplayService.cpp"
    "services/IMUService.cpp" "lib/KalmanFilter/KalmanFilter.cpp"
    INCLUDE_DIRS "lib"
)

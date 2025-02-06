idf_component_register(
    SRCS "main.cpp" "services/Event.cpp" "services/Service.cpp" "services/RoverC.cpp"
    "services/Bluetooth.cpp" "services/ServiceManager.cpp" "services/Display.cpp"
    "services/IMU.cpp" "lib/KalmanFilter/KalmanFilter.cpp"
    INCLUDE_DIRS "lib"
    REQUIRES freertos arduino
)

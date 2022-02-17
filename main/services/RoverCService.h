#ifndef _ROVERC_H_
#define _ROVERC_H_

#include "Service.h"
#include "Event.h"
#include "../lib/KalmanFilter/KalmanFilter.h"
#include <valarray>
#include <algorithm>

namespace Services {

    template <typename T>
    T Clamp(const T& n, const T& lower, const T& upper) {
        return std::max<T>(lower, std::min<T>(n, upper));
    };

    class RoverCService : public Service {
        public:
            RoverCService(const std::string& name,  Services::ServiceRegistry* service_manager);
            Result<std::string> Init(const ParametersPack& config) override;

            Result<std::string> Forward(int8_t Speed);
            Result<std::string> Backward(int8_t Speed);
            Result<std::string> TurnLeft(int8_t Speed);
            Result<std::string> TurnRight(int8_t Speed);
            Result<std::string> SlideLeft(int8_t Speed);
            Result<std::string> SlideRight(int8_t Speed);
            Result<std::string> Stop();
            Result<std::string> Send_iic(uint8_t Register, uint8_t Speed);
            Result<std::string> Send_Motors_iic(const std::valarray<int32_t>& motors_values);
            Result<std::string> SetServoAngle(uint8_t Servo_ch, uint8_t degree);
            Result<std::string> Servo_pulse(uint8_t Servo_ch, uint16_t width);
            Result<std::string> Dance();
            Result<std::string> Follow(const std::string& master);

            /*
            * @brief move the rover at a specific angle with the specified moderated speed
            * @param angle with 0 pointing forward/north, 90 right, -90 left,  180 or -180 backward/south
            * @param speed between 0 and 100
            */
            Result<std::string> MoveAtAngle(const int32_t& angle, const int32_t& speed = 100);

            /*
            * @brief speed must be in range [0,100]
            */
            int32_t ModerateSpeed(const int32_t& speed);

            /**
             * @brief limit the acceleration to a maximum of5 steps per call. \
                    higher accelerations make the rover behaves "jumpy"
            * @note orignal code https://github.com/electricidea/Bluetooth-RoverC
            */
            // int8_t SpeedRamp(const int8_t actual_speed, const int8_t target_speed) {
            //     // acceleration not larger than 5 can be applied directly
            //     // so init calc_speed with target_speed and check if the 
            //     // acceleration is higher than 5
            //     int8_t calc_speed = target_speed;
            //     // acceleration larger than 5?
            //     if(abs(target_speed - actual_speed) > 5){
            //         if( target_speed > actual_speed) 
            //         calc_speed = actual_speed + 5;
            //         else
            //         calc_speed = actual_speed - 5;
            //     }
            //     // ensure that the result is between -100 and +100
            //     calc_speed = (calc_speed > 100) ? 100 : calc_speed;
            //     calc_speed = (calc_speed < -100) ? -100 : calc_speed;
            //     return calc_speed;
            // };

            static const int32_t ROVER_ADDRESS = 0X38;
        private:

            enum class RoverMode {
                IDLE = 0,
                DANCING,
                FOLLOWING,
            };
            
            RoverMode _mode {RoverMode::IDLE};
            std::string _master;
            Lib::KalmanFilter _kalman_filter;

            int32_t _prev_rssi_delta = 0;
            int32_t _prev_rssi = 0;
            int32_t _prev_direction = 0;
            const uint32_t _prev_rssi_delta_threshold = 3;

    };
} //namespace Services

#endif

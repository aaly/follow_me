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

    class SpeedCoefficients {
        public:
            double forward = 0;
            double backward = 0;
            double right = 0;
            double left = 0;
            double rotate_right = 0;
            double rotate_left = 0;

            std::valarray<double> GetCoefficientsVector(const double& speed = 100) {
                std::valarray<double> result;

                std::valarray<double> FORWARD = {speed, speed, speed, speed};
                std::valarray<double> LEFT = {-speed, speed, speed, -speed};
                std::valarray<double> BACKWARD = {-speed, -speed, -speed, -speed};
                std::valarray<double> RIGHT = {speed, -speed, -speed, speed};
                std::valarray<double> ROTATE_R = {-speed, speed, -speed, speed};
                std::valarray<double> ROTATE_L = {speed, -speed, speed, -speed};

                result = (FORWARD*forward) + (BACKWARD*backward) + (LEFT*left) + (RIGHT*right) + (ROTATE_R*rotate_right) + (ROTATE_L*rotate_left);

                for (auto& value: result) {
                    value = Clamp(value, -100.0, 100.0);
                }
                return result;
            };
        private:
            
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
            Result<std::string> Send_Motors_iic(const std::valarray<double>& motors_values);
            Result<std::string> SetServoAngle(uint8_t Servo_ch, uint8_t degree);
            Result<std::string> Servo_pulse(uint8_t Servo_ch, uint16_t width);
            Result<std::string> Dance();
            Result<std::string> Follow(const std::string& master);

            /*
            * @brief move the rover at a specific angle with the specified moderated speed
            * @param angle is [0 to 180] or [0 to -180] with 0 pointing forward/north, 90 right, -90 left,  180 or -180 backward/south
            * @param speed between 0 and 100
            */
            Result<std::string> MoveAtAngle(const int32_t& angle, const int32_t& speed = 100);

            /*
            * @brief rotate the rover to a specific angle then move forward
            * @param angle is [0 to 180] or [0 to -180] with 0 pointing forward/north, 90 right, -90 left,  180 or -180 backward/south
            * @param speed between 0 and 100
            */
            Result<std::string> RotateAndMove(const int32_t& angle, const int32_t& speed = 100);

            /*
            * @brief speed must be in range [0,100]
            */
            int32_t ModerateSpeed(const int32_t& speed);

            /*
            * @brief angle must be in range [-180,180]
            */
            double ModerateAngle(const double& angle);


            /**
             * @brief limit the acceleration to a maximum of5 steps per call. \
                    higher accelerations make the rover behaves "jumpy"
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

            /**
             * @brief setting the motor values to new speed under consideration \
                    of the maximum acceleration.
             */
            // void SetMotors(int8_t M1, int8_t M2, int8_t M3, int8_t M4) {
            //     motor_speeds[0] = speed_ramp(motor_speeds[0],M1); 
            //     motor_speeds[1] = speed_ramp(motor_speeds[1],M2);
            //     motor_speeds[2] = speed_ramp(motor_speeds[2],M3);
            //     motor_speeds[3] = speed_ramp(motor_speeds[3],M4);
            //     //Serial.print(motor_speeds[0]);Serial.print(" ");Serial.print(motor_speeds[1]);Serial.print(" ");
            //     //Serial.print(motor_speeds[2]);Serial.print(" ");Serial.println(motor_speeds[3]);
            //     // send the Motor speed to the Rover board via I2C
            //     Wire.beginTransmission(ROVERC_I2C_Address);
            //     short address = 0x00;
            //     Wire.write(address);
            //     Wire.write(motor_speeds[0]);
            //     Wire.write(motor_speeds[1]);
            //     Wire.write(motor_speeds[2]);
            //     Wire.write(motor_speeds[3]);
            //     Wire.endTransmission();
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

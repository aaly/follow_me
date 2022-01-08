#ifndef _ROVERC_H_
#define _ROVERC_H_

#include "Service.h"

namespace Services {
    class RoverCService : public Service {
        public:
            
            Result<std::string> Init(const ParametersPack& config) override;
            Result<std::string> Run() override;

            Result<std::string> Move_forward(int8_t Speed);
            Result<std::string> Move_back(int8_t Speed);
            Result<std::string> Move_turnleft(int8_t Speed);
            Result<std::string> Move_turnright(int8_t Speed);
            Result<std::string> Move_left(int8_t Speed);
            Result<std::string> Move_right(int8_t Speed);
            Result<std::string> Move_stop();
            Result<std::string> Send_iic(uint8_t Register, uint8_t Speed);
            Result<std::string> Servo_angle(uint8_t Servo_ch, uint8_t degree);
            Result<std::string> Servo_pulse(uint8_t Servo_ch, uint16_t width);

            static const int32_t ROVER_ADDRESS = 0X38;
    };
} //namespace Services

#endif

#include "RoverCService.h"
#include <M5StickCPlus.h>

namespace Services {

  Result<std::string> RoverCService::Init(const ParametersPack& config) {
      Result<std::string> result;
      Serial.printf("RoverCService::Init()\n");
      constexpr int sda = 0;
      constexpr int scl = 26;
      constexpr int frequency = 100;
      Wire.begin(static_cast<int>(sda), static_cast<int>(scl), static_cast<uint32_t>(frequency));

      AddCallback(std::string("forward"), [&](const ParametersPack& data) { 
        return Move_forward(data.GetParameter<int8_t>("speed"));
        });

      AddCallback(std::string("back"), [&](const ParametersPack& data) { 
        return Move_back(data.GetParameter<int8_t>("speed"));
        });

      AddCallback(std::string("right"), [&](const ParametersPack& data) { 
        return Move_right(data.GetParameter<int8_t>("speed"));
        });

      AddCallback(std::string("left"), [&](const ParametersPack& data) { 
        return Move_left(data.GetParameter<int8_t>("speed"));
        });

      AddCallback(std::string("turn_left"), [&](const ParametersPack& data) { 
        return Move_turnleft(data.GetParameter<int8_t>("speed"));
        });

      AddCallback(std::string("turn_right"), [&](const ParametersPack& data) { 
        return Move_turnright(data.GetParameter<int8_t>("speed"));
        });

      AddCallback(std::string("servo_angle"), [&](const ParametersPack& data) { 
        return Servo_angle(data.GetParameter<int8_t>("servo_ch"), data.GetParameter<int8_t>("degree"));
        });

        AddCallback(std::string("stop"), [&](const ParametersPack& data) { 
        return Move_stop();
        });

      result.SetStatus(StatusType::SUCCESS);
      return result;
  }

  Result<std::string> RoverCService::Run() {
    Result<std::string> result;
    result.SetStatus(StatusType::FAILURE);
    return result;
  }

  Result<std::string> RoverCService::Send_iic(const uint8_t Register, const uint8_t Speed) {
    Result<std::string> result;
    Wire.beginTransmission(ROVER_ADDRESS);
    Wire.write(Register);
    Wire.write(Speed);
    Wire.endTransmission();
    return result;
  }

  Result<std::string> RoverCService::Move_forward(const int8_t Speed) {
    Result<std::string> result;
    Send_iic(0x00, Speed );
    Send_iic(0x01, Speed );
    Send_iic(0x02, Speed );
    Send_iic(0x03, Speed );
    return result;
  }

  Result<std::string> RoverCService::Move_back(const int8_t Speed) {
    Result<std::string> result;
    Send_iic(0x00, (-1) * Speed );
    Send_iic(0x01, (-1) * Speed );
    Send_iic(0x02, (-1) * Speed );
    Send_iic(0x03, (-1) * Speed );
    return result;
  }

  Result<std::string> RoverCService::Move_turnleft(const int8_t Speed) {
    Result<std::string> result;
    Send_iic(0x00, Speed );
    Send_iic(0x01, (-1) * Speed );
    Send_iic(0x02, Speed );
    Send_iic(0x03, (-1) * Speed );
    return result;
  }

  Result<std::string> RoverCService::Move_turnright(const int8_t Speed) {
    Result<std::string> result;
    Send_iic(0x00, (-1) * Speed );
    Send_iic(0x01, Speed );
    Send_iic(0x02, (-1) * Speed );
    Send_iic(0x03, Speed );
    return result;
  }

  Result<std::string> RoverCService::Move_left(const int8_t Speed) {
    Result<std::string> result;
    Send_iic(0x00, (-1) * Speed );
    Send_iic(0x01, Speed );
    Send_iic(0x02, Speed );
    Send_iic(0x03, (-1) * Speed );
    return result;
  }

  Result<std::string> RoverCService::Move_right(const int8_t Speed) {
    Result<std::string> result;
    Send_iic(0x00, Speed );
    Send_iic(0x01, (-1) * Speed );
    Send_iic(0x02, (-1) * Speed );
    Send_iic(0x03, Speed );
    return result;
  }

  Result<std::string> RoverCService::Move_stop() {
    Result<std::string> result;
    Send_iic(0x00, 0 );
    Send_iic(0x01, 0 );
    Send_iic(0x02, 0 );
    Send_iic(0x03, 0 );
    return result;
  }

  Result<std::string> RoverCService::Servo_angle(const uint8_t Servo_ch, uint8_t degree) {
    Result<std::string> result;
    degree = min(90, int(degree));
    degree = max(0, int(degree));
    Send_iic((Servo_ch - 1) | 0x10 ,int(degree));
    return result;
  }

  Result<std::string> RoverCService::Servo_pulse(const uint8_t Servo_ch, uint16_t width) {
    Result<std::string> result;
    width = min(2500, int(width));
    width = max(500, int(width));
    Send_iic((Servo_ch - 1) | 0x20, width);
    return result;
  }
  
} //namespace Services
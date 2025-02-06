#include "RoverC.h"
namespace Services {

    RoverC::RoverC(const std::string& name,  Services::ServiceRegistry* service_manager): Service(name, service_manager) {
        ;
    }

    void RoverC::Init(const ParametersPack& config) {
        constexpr int sda = 0;
        constexpr int scl = 26;
        constexpr int frequency = 100;
        Wire.begin(static_cast<int>(sda), static_cast<int>(scl), static_cast<uint32_t>(frequency));

        On("rover.dance", &RoverC::Dance);
        On("rover.stop", &RoverC::Stop);
        On("rover.follow", &RoverC::Follow);
        On("rover.moveatangle", &RoverC::MoveAtAngle);
        On("bluetooth.scanner.results", &RoverC::UpdateMasterDistance);

        Stop(Services::ParametersPack(""));
    }

    void RoverC::UpdateMasterDistance(const Services::ParametersPack& parameters) {
        if(parameters.GetParameter<std::string>("name") == "GalaxyMaster") {
            if(!_kalman_filter.IsInitialized()) {
                _kalman_filter.SetState(parameters.GetParameter<int32_t>("rssi"));
                _prev_rssi = -1000;
                _prev_rssi_delta = 1000;
            }

            int32_t rssi = static_cast<int32_t>(_kalman_filter.CorrectAndGet(parameters.GetParameter<int32_t>("rssi")));
            Serial.printf("Rover has device [%s] RSSI [%ld]\n", parameters.GetParameter<std::string>("name").c_str(), rssi);
            Emit(Event("display.showtext", std::string("{\"text\":\"RSSI:") + std::to_string(rssi) + "\", \"clear\":true}"));
            _prev_rssi_delta = abs(_prev_rssi - rssi);
            _prev_rssi = rssi;

            //if (_prev_rssi_delta > _prev_rssi_delta_threshold) {
            if (_prev_rssi < -55) {
                Forward(100);
                delay(500);
                Stop(Services::ParametersPack(""));
            }
        }
    }

    Result<std::string> RoverC::Send_iic(const uint8_t Register, const uint8_t Speed) {
        Result<std::string> result;
        Wire.beginTransmission(ROVER_ADDRESS);
        Wire.write(Register);
        Wire.write(Speed);
        Wire.endTransmission();
        return result;
    }


    Result<std::string> RoverC::Send_Motors_iic(const std::valarray<int32_t>& motors_values) {
        Result<std::string> result;
        Wire.beginTransmission(ROVER_ADDRESS);
        Wire.write(0x00);
        Wire.write(ModerateSpeed(motors_values[0]));
        Wire.write(ModerateSpeed(motors_values[1]));
        Wire.write(ModerateSpeed(motors_values[2]));
        Wire.write(ModerateSpeed(motors_values[3]));
        Wire.endTransmission();
        return result;
    }

    int32_t RoverC::ModerateSpeed(const int32_t& speed) {
        int32_t result = Clamp(speed, static_cast<int32_t>(-100), static_cast<int32_t>(100));
        return result;
    }

    void RoverC::MoveAtAngle(const Services::ParametersPack& parameters) {
        const int32_t& angle = parameters.GetParameter<int32_t>("angle");
        const int32_t& speed = parameters.GetParameter<int32_t>("speed");

        const int32_t moderatedSpeed = ModerateSpeed(speed);

        int32_t x = std::round(moderatedSpeed*std::sin((angle*M_PI)/180));
        int32_t y = std::round(moderatedSpeed*std::cos((angle*M_PI)/180));
        
        Serial.printf("MoveAtAngle(%ld, %ld)\n",angle ,moderatedSpeed);
        Serial.printf("x,y(%ld, %ld)\n",x ,y);

        std::valarray<int32_t> motorsSpeed = {y+x, y-x, y-x, y+x};
        
        Serial.printf("motorsSpeed: %ld, %ld, %ld, %ld \n", motorsSpeed[0], motorsSpeed[1], motorsSpeed[2], motorsSpeed[3]);
         Send_Motors_iic(motorsSpeed);
    };

    void RoverC::Forward(const int8_t speed) {
        MoveAtAngle(Services::ParametersPack(R"({"angle":0, "speed":)" + std::to_string(speed) + "}"));
    }

    void RoverC::Backward(const int8_t speed) {
        MoveAtAngle(Services::ParametersPack(R"({"angle":180, "speed":)" + std::to_string(speed) + "}"));
    }

    Result<std::string> RoverC::TurnLeft(const int8_t speed) {
        Result<std::string> result;
        std::valarray<int32_t> motorsSpeed = {ModerateSpeed(speed), ModerateSpeed(speed*-1), ModerateSpeed(speed), ModerateSpeed(speed*-1)};
        result = Send_Motors_iic(motorsSpeed);
        return result;
    }

    Result<std::string> RoverC::TurnRight(const int8_t speed) {
        Result<std::string> result;
        std::valarray<int32_t> motorsSpeed = {ModerateSpeed(speed*-1), ModerateSpeed(speed), ModerateSpeed(speed*-1), ModerateSpeed(speed)};
        result = Send_Motors_iic(motorsSpeed);
        return result;
    }

    void RoverC::SlideLeft(const int8_t speed) {
        MoveAtAngle(Services::ParametersPack(R"({"angle":-90, "speed":)" + std::to_string(speed) + "}"));
    }

    void RoverC::SlideRight(const int8_t speed) {
        MoveAtAngle(Services::ParametersPack(R"({"angle":90, "speed":)" + std::to_string(speed) + "}"));
    }

    void RoverC::Stop(const Services::ParametersPack& parameters) {
        std::valarray<int32_t> motorsSpeed = {0, 0, 0, 0};
        Send_Motors_iic(motorsSpeed);
    }

    Result<std::string> RoverC::SetServoAngle(const uint8_t Servo_ch, uint8_t degree) {
        Result<std::string> result;
        degree = min(90, int(degree));
        degree = max(0, int(degree));
        Send_iic((Servo_ch - 1) | 0x10 ,int(degree));
        return result;
    }

    Result<std::string> RoverC::Servo_pulse(const uint8_t Servo_ch, uint16_t width) {
        Result<std::string> result;
        width = min(2500, int(width));
        width = max(500, int(width));
        Send_iic((Servo_ch - 1) | 0x20, width);
        return result;
    }

    void RoverC::Follow(const Services::ParametersPack& parameters) {
        _master = parameters.GetParameter<std::string>("master");
    }
    
    void RoverC::Dance(const Services::ParametersPack& parameters) {

        // Emit(Event("imu.startcollecting", "{\"interval\":" + std::to_string(500) + "}"));
        // Emit(Event("display.showtext", "{\"text\":\"Rover Dancing...\", \"clear\":true}"));
        // Stop(Services::ParametersPack(""));

        // M5.Imu.Init();

        


        // float accelerationX, accelerationY, accelerationZ = 0.0;
        // float x, y, z = 0.0;
        // while(true) {
        //     TurnLeft(50);
        //     M5.IMU.getAhrsData(&x, &y, &z);
        //     M5.IMU.getAccelData(&accelerationX, &accelerationY, &accelerationZ);
        //     float yaw = 180 * atan (accelerationX/sqrt(accelerationY*accelerationY + accelerationZ*accelerationZ))/M_PI;
        //     Emit(Event("display.showtext", "{\"text\":\"theirs: " + std::to_string(z) + "our:" + std::to_string(yaw) +"\", \"clear\":true}"));
        //     Stop(Services::ParametersPack(""));
        //     delay(800);
        // }


        
        // long start = xTaskGetTickCount();
        
        // M5.IMU.getAhrsData(&x, &y, &z);
        // float x2 = x;
        // float  y2 = y;
        // float z2 = 100000;

        // uint64_t counter = 0;
        // uint64_t cum = 0;
        // //while(std::abs(z-z2) != 0) {
        // while(counter < 1000/20) {
        //     delay(20);
        //     z = z2;
        //     M5.IMU.getAhrsData(&x2, &y2, &z2);
        //     Serial.printf("Z: %f deg/s \n", std::abs(z-z2));
        //     counter++;
        //     //cum += std::abs(z-z2);
        //     cum += z;
        // }
        // Stop(Services::ParametersPack(""));

        // //Emit(Event("display.showtext", "{\"text\":\"" + std::to_string(cum/counter) +" deg/s\", \"clear\":true}"));

        // Serial.printf("x: %f, X2:%f \n", x, x2);
        // Serial.printf("y: %f, Y2:%f \n", y, y2);
        // Serial.printf("Z: %f, Z2:%f \n", z, z2);
        

        // long stop = xTaskGetTickCount() - start;

        // auto avg_yaw_velocity = cum/counter;
        // //Emit(Event("display.showtext", "{\"text\":\"" + std::to_string(pdTICKS_TO_MS(stop)) +"\", \"clear\":true}"));
        // Emit(Event("display.showtext", "{\"text\":\"avg:" + std::to_string(avg_yaw_velocity) +"\", \"clear\":true}"));
        
        // delay(8000);

        // Emit(Event("display.showtext", "{\"text\":\"waiting:" + std::to_string((360*1000)/avg_yaw_velocity) +"\", \"clear\":true}"));
        // TurnLeft(25);
        
        // M5.IMU.getAhrsData(&x, &y, &z);
        
        // while(std::abs(z-z2) < ((360*1000)/avg_yaw_velocity)) {
        //     delay(20);
        //     z = z2;
        //     M5.IMU.getAhrsData(&x2, &y2, &z2);
        //     Serial.printf("Z: %f deg/s \n", std::abs(z-z2));
        //     counter++;
        //     //cum += std::abs(z-z2);
        //     cum += z;
        // }

        // delay((360*1000)/avg_yaw_velocity);
        // Stop(Services::ParametersPack(""));
        // return result;

        Stop(Services::ParametersPack(""));
        
        SetServoAngle(1, 90);
        delay(1000);

        Backward(50);
        delay(2000);

        SlideLeft(50);
        delay(2000);

        SlideRight(50);
        delay(2000);

        TurnLeft(30);
        delay(2000);

        TurnRight(30);
        delay(2000);

        Forward(50);
        //delay(2000);

        Stop(Services::ParametersPack(""));
        SetServoAngle(1, 0);
        Emit(Event("display.showtext", "{\"text\":\"Done Dancing...\", \"clear\":true}"));

        Emit(Event("imu.stopcollecting", ""));
    }
    
} //namespace Services
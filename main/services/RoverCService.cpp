#include "RoverCService.h"
#include <cmath>
namespace Services {

    RoverCService::RoverCService(const std::string& name,  Services::ServiceRegistry* service_manager): Service(name, service_manager) {
        ;
    }

    Result<std::string> RoverCService::Init(const ParametersPack& config) {
        Result<std::string> result;
        constexpr int sda = 0;
        constexpr int scl = 26;
        constexpr int frequency = 100;
        Wire.begin(static_cast<int>(sda), static_cast<int>(scl), static_cast<uint32_t>(frequency));

        On("bluetooth.scanner.results", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result;
            if(parameters.GetParameter<std::string>("name") == "GalaxyMaster") {
                if(!_kalman_filter.IsInitialized()) {
                    _kalman_filter.SetState(parameters.GetParameter<int32_t>("rssi"));
                    _prev_rssi = -1000;
                    _prev_rssi_delta = 1000;
                }

                int32_t rssi = static_cast<int32_t>(_kalman_filter.CorrectAndGet(parameters.GetParameter<int32_t>("rssi")));
                Serial.printf("Rover has device [%s] RSSI [%d]\n", parameters.GetParameter<std::string>("name").c_str(), rssi);
                Emit(Event("display.showtext", std::string("{\"text\":\"RSSI:") + std::to_string(rssi) + "\", \"clear\":true}"));
                _prev_rssi_delta = abs(_prev_rssi - rssi);
                _prev_rssi = rssi;

                //if (_prev_rssi_delta > _prev_rssi_delta_threshold) {
                if (_prev_rssi < -55) {
                    Forward(100);
                    delay(500);
                    Stop();
                }
            }
            return result;
        });

        On("rover.dance", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result;
            result = Dance();
            return result;
        });


        On("rover.moveatangle", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result;
            result = MoveAtAngle(parameters.GetParameter<int32_t>("angle"), parameters.GetParameter<int32_t>("speed"));
            return result;
        });

        On("rover.rotateandmove", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result;
            result = RotateAndMove(parameters.GetParameter<int32_t>("angle"), parameters.GetParameter<int32_t>("speed"));
            return result;
        });
        

        On("rover.follow", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result = Follow(parameters.GetParameter<std::string>("master"));
            return result;
        });

        On("rover.stop", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result = Stop();
            return result;
        });

        result = Stop();
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


    Result<std::string> RoverCService::Send_Motors_iic(const std::valarray<double>& motors_values) {
        Result<std::string> result;
        Send_iic(0x00, static_cast<unsigned int>(motors_values[0]));
        Send_iic(0x01, static_cast<unsigned int>(motors_values[1]));
        Send_iic(0x02, static_cast<unsigned int>(motors_values[2]));
        Send_iic(0x03, static_cast<unsigned int>(motors_values[3]));
        return result;
    }

    int32_t RoverCService::ModerateSpeed(const int32_t& speed) {
        int32_t result = Clamp(speed, 0, 100);
        return result;
    }

    double RoverCService::ModerateAngle(const double& angle) {
        double result = Clamp(angle, -180.0, 180.0);
        /*if(result < 0) {
            result = 360 - std::abs(result);
        }*/
        return result;
    }

    Result<std::string> RoverCService::MoveAtAngle(const int32_t& angle, const int32_t& speed) {
        Result<std::string> result;
        
        const int32_t moderatedSpeed = ModerateSpeed(speed);
        const int32_t moderatedAngle = ModerateAngle(angle);

        double x = moderatedSpeed*std::sin(moderatedAngle*M_PI/180);
        double y = moderatedSpeed*std::cos(moderatedAngle*M_PI/180);

        SpeedCoefficients speedCoefficients;
        
        Serial.printf("MoveAtAngle(%d, %d)\n",moderatedAngle ,moderatedSpeed);
        Serial.printf("x,y(%f, %f)\n",x ,y);

        if(x >= 0) { // right
            speedCoefficients.right = std::abs(x)*(1.0/moderatedSpeed);
        }
        else { //left
            speedCoefficients.left = std::abs(x)*(1.0/moderatedSpeed);
        }

        if(y >= 0) { // forward
            speedCoefficients.forward = std::abs(y)*(1.0/moderatedSpeed);
        }
        else { //backward
            speedCoefficients.backward = std::abs(y)*(1.0/moderatedSpeed);
        }

        Serial.printf("speedCoefficients: %f, %f, %f, %f \n", speedCoefficients.forward, speedCoefficients.backward, speedCoefficients.left, speedCoefficients.right);
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        
        Serial.printf("appliedCoefficients: %f, %f, %f, %f \n", appliedCoefficients[0], appliedCoefficients[1], appliedCoefficients[2], appliedCoefficients[3]);
        result = Send_Motors_iic(appliedCoefficients);
        return result;
    };


    Result<std::string> RoverCService::RotateAndMove(const int32_t& angle, const int32_t& speed) {
        Result<std::string> result;
        
        const int32_t moderatedSpeed = ModerateSpeed(speed);
        const int32_t moderatedAngle = ModerateAngle(angle);

        SpeedCoefficients speedCoefficients;
        
        Serial.printf("MoveAtAngle(%d, %d)\n",moderatedAngle ,moderatedSpeed);

        if(moderatedAngle >= 0) { // rotate right
            speedCoefficients.rotate_right = moderatedSpeed/100;
        }
        else { // rotate left
            speedCoefficients.rotate_left = moderatedSpeed/100;
        }

        Serial.printf("speedCoefficients: %f, %f, %f, %f \n", speedCoefficients.forward, speedCoefficients.backward, speedCoefficients.left, speedCoefficients.right);
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        Serial.printf("appliedCoefficients: %f, %f, %f, %f \n", appliedCoefficients[0], appliedCoefficients[1], appliedCoefficients[2], appliedCoefficients[3]);
        Send_Motors_iic(appliedCoefficients);
        delay(1000);
        Forward(moderatedSpeed);
        delay(1000);
        return result;
    };

    Result<std::string> RoverCService::Forward(const int8_t speed) {
        Result<std::string> result;
        const int32_t moderatedSpeed = ModerateSpeed(speed);
        SpeedCoefficients speedCoefficients;
        speedCoefficients.forward = 1.0;
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        result = Send_Motors_iic(appliedCoefficients);
        return result;
    }

    Result<std::string> RoverCService::Backward(const int8_t speed) {
        Result<std::string> result;
        const int32_t moderatedSpeed = ModerateSpeed(speed);
        SpeedCoefficients speedCoefficients;
        speedCoefficients.backward = 1.0;
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        result = Send_Motors_iic(appliedCoefficients);
        return result;
    }

    Result<std::string> RoverCService::TurnLeft(const int8_t speed) {
        Result<std::string> result;
        const int32_t moderatedSpeed = ModerateSpeed(speed);
        SpeedCoefficients speedCoefficients;
        speedCoefficients.rotate_left = 1.0;
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        result = Send_Motors_iic(appliedCoefficients);
        return result;
    }

    Result<std::string> RoverCService::TurnRight(const int8_t speed) {
        Result<std::string> result;
        const int32_t moderatedSpeed = ModerateSpeed(speed);
        SpeedCoefficients speedCoefficients;
        speedCoefficients.rotate_right = 1.0;
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        result = Send_Motors_iic(appliedCoefficients);
        return result;
    }

    Result<std::string> RoverCService::SlideLeft(const int8_t speed) {
        Result<std::string> result;
        const int32_t moderatedSpeed = ModerateSpeed(speed);
        SpeedCoefficients speedCoefficients;
        speedCoefficients.left = 1.0;
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        result = Send_Motors_iic(appliedCoefficients);
        return result;
    }

    Result<std::string> RoverCService::SlideRight(const int8_t speed) {
        Result<std::string> result;
        const int32_t moderatedSpeed = ModerateSpeed(speed);
        SpeedCoefficients speedCoefficients;
        speedCoefficients.right = 1.0;
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        result = Send_Motors_iic(appliedCoefficients);
        return result;
    }

    Result<std::string> RoverCService::Stop() {
        Result<std::string> result;
        const int32_t moderatedSpeed = ModerateSpeed(0);
        SpeedCoefficients speedCoefficients; // all coefficients initiliazed to 0, that is stop :)
        auto appliedCoefficients = speedCoefficients.GetCoefficientsVector(moderatedSpeed);
        result = Send_Motors_iic(appliedCoefficients);
        return result;
    }

    Result<std::string> RoverCService::SetServoAngle(const uint8_t Servo_ch, uint8_t degree) {
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

    Result<std::string> RoverCService::Follow(const std::string& master) {
        Result<std::string> result;
        _master = master;
        return result;
    }
    Result<std::string> RoverCService::Dance() {
        Result<std::string> result;

        // Emit(Event("imu.startcollecting", "{\"interval\":" + std::to_string(500) + "}"));
        // Emit(Event("display.showtext", "{\"text\":\"Rover Dancing...\", \"clear\":true}"));
        // Stop();

        // M5.Imu.Init();

        


        // float accelerationX, accelerationY, accelerationZ = 0.0;
        // float x, y, z = 0.0;
        // while(true) {
        //     TurnLeft(50);
        //     M5.IMU.getAhrsData(&x, &y, &z);
        //     M5.IMU.getAccelData(&accelerationX, &accelerationY, &accelerationZ);
        //     float yaw = 180 * atan (accelerationX/sqrt(accelerationY*accelerationY + accelerationZ*accelerationZ))/M_PI;
        //     Emit(Event("display.showtext", "{\"text\":\"theirs: " + std::to_string(z) + "our:" + std::to_string(yaw) +"\", \"clear\":true}"));
        //     Stop();
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
        // Stop();

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
        // Stop();
        // return result;

        Stop();
        
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

        Stop();
        SetServoAngle(1, 0);
        Emit(Event("display.showtext", "{\"text\":\"Done Dancing...\", \"clear\":true}"));

        Emit(Event("imu.stopcollecting", ""));

        result.Success("");
        return result;
    }
    
} //namespace Services
#include "IMUService.h"

namespace Services {

    IMUService::IMUService(const std::string& name, Services::ServiceRegistry* service_manager): Service(name, service_manager) {
    }

    Result<std::string> IMUService::Init(const ParametersPack& config) {
        Result<std::string> result;

        m_isCollecting.store(false);
        
        if (0 == M5.Imu.Init()) {
            result.Success("");

            On("imu.start", [&](const Services::ParametersPack& parameters) {
                Result<std::string> result;
                if(!m_isCollecting) {
                    m_isCollecting.store(true);
                    Lib::Timer dataTimer = Lib::Timer("dataTimer", parameters.GetParameter<uint32_t>("interval"), [this](){
                        IMUData data;
                        M5.IMU.getGyroData(&data.gyroX, &data.gyroY, &data.gyroZ);
                        M5.IMU.getAccelData(&data.accX, &data.accY, &data.accZ);
                        M5.IMU.getAhrsData(&data.pitch, &data.roll, &data.yaw);
                        M5.IMU.getTempData(&data.temp);
                        ProcessData(data);
                    }, false);
                    
                    result = dataTimer.Start();
                }
                return result;
            });

            On("imu.stop", [&](const Services::ParametersPack& parameters) {
                Result<std::string> result;
                m_isCollecting.store(false);
                dataTimer.Stop();
                return result;
            });
        }
        else {
            result.Failure("Failed to Initialize IMU Service");
        }

        return result;
    }

    void IMUService::ProcessData(const IMUData& data) {
        //m_IMUInformation
    }
    
} //namespace Services
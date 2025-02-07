#include "IMU.h"

namespace Services {

    IMU::IMU(const std::string& name, Services::ServiceRegistry* service_manager): Service(name, service_manager) {
    }

    void IMU::Init(const ParametersPack& config) {
        m_isCollecting.store(false);
        if (M5.Imu.init() == 0) {
            On("imu.start", &IMU::Start);
            On("imu.stop", &IMU::Stop);
        }
        else {
            //TODO emit error
            //result.Failure("Failed to Initialize IMU Service");
        }
    }

    void IMU::Start(const Services::ParametersPack& parameters) {
        if(!m_isCollecting) {
            m_isCollecting.store(true);
            Lib::Timer dataTimer = Lib::Timer("dataTimer", parameters.GetParameter<uint32_t>("interval"), [this](){
                IMUData data;
                M5.Imu.getGyroData(&data.gyroX, &data.gyroY, &data.gyroZ);
                M5.Imu.getAccelData(&data.accX, &data.accY, &data.accZ);
                //M5.Imu.getAhrsData(&data.pitch, &data.roll, &data.yaw);
                //M5.Imu.getTempData(&data.temp);
                ProcessData(data);
            }, false);
            
            dataTimer.Start();
        }
    }

    void IMU::Stop(const Services::ParametersPack& parameters) {
        m_isCollecting.store(false);
        dataTimer.Stop();
    }

    void IMU::ProcessData(const IMUData& data) {
        //m_IMUInformation
    }
    
} //namespace Services
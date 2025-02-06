#ifndef _SERVICE_IMU_H
#define _SERVICE_IMU_H

#include "Service.h"
#include "Event.h"
#include <atomic>
#include <vector>
#include <Timer.h>

namespace Services {

    class IMUData {
    public:
        float accX = 0.0F;
        float accY = 0.0F;
        float accZ = 0.0F;

        float gyroX = 0.0F;
        float gyroY = 0.0F;
        float gyroZ = 0.0F;

        float pitch = 0.0F;
        float roll  = 0.0F;
        float yaw   = 0.0F;

        float temp = 0;
    };


    class IMUInformation {
    public:
        uint64_t counter = 0;
        uint64_t speed = 0;
        uint64_t temp = 0;
    };

    class IMU : public Service {
        public:
            IMU(const std::string& name,  Services::ServiceRegistry* service_manager);
            void Init(const ParametersPack& config) override;
            void Start(const Services::ParametersPack&);
            void Stop(const Services::ParametersPack&);
        private:
            std::atomic<bool> m_isCollecting;
            IMUInformation m_IMUInformation;
            Lib::Timer dataTimer;
            void ProcessData(const IMUData& data);
            
    };
} //namespace Services

#endif //_SERVICE_IMU_H

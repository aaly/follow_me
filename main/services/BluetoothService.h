#ifndef _BLUETOOTH_SERVICE
#define _BLUETOOTH_SERVICE

#include "Service.h"
#include "Event.h"
namespace Services {
    class Connection {
        public:
            std::string name;
            int32_t rssi;
    };

    class BluetoothService : public Service{
        public:
            BluetoothService(const std::string& name, Services::ServiceRegistry* service_manager);
            Result<std::string> Init(const ParametersPack& config) override;

            static Result<std::vector<Connection>> Scan(const uint32_t scanTime = 100, const std::string& device_name = "");
    };
} //namespace Services

#endif //_BLUETOOTH_SERVICE

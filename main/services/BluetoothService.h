#ifndef _BLUETOOTH_SERVICE
#define _BLUETOOTH_SERVICE

#include "Service.h"
#include <vector>
#include <string>

namespace Services {

    struct Connection {
        public:
            std::string name;
    };

    class BluetoothService : public Service{
        public:
            Result<std::string> Init(const ParametersPack& config) override;
            Result<std::string> Run() override;

            static Result<std::vector<Connection>> Scan();
            Result<std::string> Subscribe(const std::string& server_name);
        
    };

} //namespace Services

#endif //_BLUETOOTH_SERVICE

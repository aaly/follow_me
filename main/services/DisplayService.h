#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "Service.h"
#include "Event.h"

namespace Services {
    class DisplayService : public Service {
        public:
            DisplayService(const std::string& name,  Services::ServiceRegistry* service_manager);
            Result<std::string> Init(const ParametersPack& config) override;
    };
} //namespace Services

#endif

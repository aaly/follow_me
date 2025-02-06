#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "Service.h"
#include "Event.h"

namespace Services {
    class Display : public Service {
        public:
            Display(const std::string& name,  Services::ServiceRegistry* service_manager);
            void Init(const ParametersPack& config) override;
            void Showtext(const Services::ParametersPack& parameters);
            void Clear(const Services::ParametersPack& parameters);
    };
} //namespace Services

#endif

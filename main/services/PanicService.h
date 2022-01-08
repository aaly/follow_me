#ifndef _PANIC_SERVICE
#define _PANIC_SERVICE

#include "Service.h"
#include <string>

namespace Services {

    class PanicService : public Service{
        public:
            Result<std::string> Init(const ParametersPack& config) override;
            Result<std::string> Run() override;
    };

} //namespace Services

#endif //_PANIC_SERVICE

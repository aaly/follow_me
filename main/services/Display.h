#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "Service.h"
#include "Event.h"
#include <M5GFX.h>

namespace Services {
    class Display : public Service {
        public:
            Display(const std::string& name,  Services::ServiceRegistry* service_manager);
            void Init(const ParametersPack& config) override;
            void Showtext(const Services::ParametersPack& parameters);
            void Clear(const Services::ParametersPack& parameters);
            void Animate(const Services::ParametersPack& parameters);
            void GetProperties(const Services::ParametersPack& parameters);
            void ProgressBar(const Services::ParametersPack& parameters);
            void Showimage(const Services::ParametersPack& parameters);
            void SetBrightness(const Services::ParametersPack& parameters);
        private:
            M5GFX display;
    };
} //namespace Services

#endif




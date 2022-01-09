#include "DisplayService.h"

namespace Services {

    DisplayService::DisplayService(const std::string& name, Services::ServiceRegistry* service_manager): Service(name, service_manager) {
    }

    Result<std::string> DisplayService::Init(const ParametersPack& config) {
        Result<std::string> result;
        M5.begin();
        result.Success("");
        
        On("display.showtext", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result;
            if(parameters.GetParameter<bool>("clear")) {
                M5.Lcd.fillScreen(BLACK);
            }
            M5.Lcd.setRotation(3);
            M5.Lcd.setTextColor(BLUE);
            M5.Lcd.setCursor(80, 60, 4);
            M5.Lcd.printf(parameters.GetParameter<std::string>("text").c_str());
            return result;
        });

        On("display.clear", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result;
            M5.Lcd.fillScreen(BLACK);
            return result;
        });

        return result;
    }
    
} //namespace Services
#include "Display.h"

namespace Services {

    Display::Display(const std::string& name, Services::ServiceRegistry* service_manager): Service(name, service_manager) {
    }

    void Display::Init(const ParametersPack& config) {
        M5.begin();
        
        On("display.showtext", &Display::Showtext);
        On("display.clear", &Display::Clear);
    }

    void Display::Showtext(const Services::ParametersPack& parameters) {
            if(parameters.GetParameter<bool>("clear")) {
                M5.Lcd.fillScreen(BLACK);
            }
            M5.Lcd.setRotation(3);
            M5.Lcd.setTextColor(BLUE);
            M5.Lcd.setCursor(80, 60, 4);
            M5.Lcd.printf(parameters.GetParameter<std::string>("text").c_str());
    }

    void Display::Clear(const Services::ParametersPack& parameters) {
            M5.Lcd.fillScreen(BLACK);
    }
    
} //namespace Services
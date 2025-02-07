#include "Display.h"

namespace Services {

    Display::Display(const std::string& name, Services::ServiceRegistry* service_manager) : Service(name, service_manager) {
    }

    void Display::Init(const ParametersPack& config) {
        display.init();
        On("display.showtext", &Display::Showtext);
        On("display.showimage", &Display::Showimage);
        On("display.clear", &Display::Clear);
        On("display.setbrightness", &Display::SetBrightness);
        On("display.animate", &Display::Animate);
        On("display.getproperties", &Display::GetProperties);
        On("display.progressbar", &Display::ProgressBar);
    }

    void Display::Showtext(const Services::ParametersPack& parameters) {
         if(parameters.GetParameter<bool>("clear")) {
            display.clear();
        }
        
        display.setRotation(parameters.GetParameter<int>("rotation"));
        display.setTextColor(
            parameters.GetParameter<uint16_t>("color"),
            parameters.GetParameter<uint16_t>("bg_color")
        );
        
        display.setTextSize(parameters.GetParameter<uint8_t>("size"));
        display.setCursor(
            parameters.GetParameter<int16_t>("x"),
            parameters.GetParameter<int16_t>("y")
        );
        
        display.printf(parameters.GetParameter<std::string>("text").c_str());

    }

    void Display::Showimage(const Services::ParametersPack& parameters) {
        std::string imagePath = parameters.GetParameter<std::string>("path");
        int x = parameters.GetParameter<int>("x");
        int y = parameters.GetParameter<int>("y");

        if (imagePath.empty()) {
            return;
        }

        // Assuming an image loading function exists (e.g., from SPIFFS or SD card)
        //M5.Lcd.drawJpgFile(SD, imagePath.c_str(), x, y);
    }

    void Display::Clear(const Services::ParametersPack& parameters) {
        display.clear();
    }

    void Display::SetBrightness(const Services::ParametersPack& parameters) {
        display.setBrightness(parameters.GetParameter<int>("level"));
    }

    void Display::Animate(const Services::ParametersPack& parameters) {
        int duration = parameters.GetParameter<int>("duration");
        for (int i = 0; i < duration; i += 100) {
            display.fillScreen(random(0xFFFF));
            delay(100);
        }
    }

    void Display::GetProperties(const Services::ParametersPack& parameters) {
        /*Services::ParametersPack response;
        response.SetParameter("width", M5.Lcd.width());
        response.SetParameter("height", M5.Lcd.height());
        response.SetParameter("brightness", M5.Axp.GetLcdVoltage());
        SendResponse(response);*/
    }

    void Display::ProgressBar(const Services::ParametersPack& parameters) {
        int x = parameters.GetParameter<int>("x");
        int y = parameters.GetParameter<int>("y");
        int width = parameters.GetParameter<int>("width");
        int height = parameters.GetParameter<int>("height");
        int progress = parameters.GetParameter<int>("progress");
        
        display.drawRect(x, y, width, height, WHITE);
        display.fillRect(x, y, (width * progress) / 100, height, GREEN);
    }

} // namespace Services




#include "services/ServiceManager.h"
#include "services/Bluetooth.h"
#include "services/Display.h"
#include "services/RoverC.h"
#include "services/IMU.h"
#include <M5Unified.hpp>
#include "lib/Timer.h"
#include "services/Event.h"

extern "C" void app_main()
{
    initArduino();
    Serial.begin(115200);

    while (false) {
                Serial.printf("testfailed to start dance timer! \n\n\n\n\n");

        delay(4000);
    }
    
    Services::ServiceRegistry& serviceManager = Services::ServiceRegistry::GetInstance();
    //serviceManager.RegisterService<Services::Bluetooth>("bluetooth scanner", "{\"mode\":\"scanner\"}");
    serviceManager.RegisterService<Services::Display>("display", "");
    //serviceManager.RegisterService<Services::IMUService>("imu", "");
    serviceManager.RegisterService<Services::RoverC>("rover", "");

    // {
    //     Lib::Timer bluetoothTimer("bluetoothTimer", 5000, [&](){
    //         std::string ble_name = "GalaxyMaster";
    //         serviceManager.RegisterEvent(Services::Event("bluetooth.scanner.scan", 
    //         std::string("{\"continuous\": true, \"device\": \"") + ble_name + "\"}"));
    //     }, true);
        
    //     if(!bluetoothTimer.Start().Succeded()) {
    //         Serial.printf("failed to start bluetooth timer! \n\n\n\n\n");
    //     }
    // }
    delay(1000);
    serviceManager.RegisterEvent(
        Services::Event(
            "display.setbrightness", 
            std::string("{\"level\": 100}")
        )
    );


    {    
        Lib::Timer danceTimer("dance", 5000, [&](){
            serviceManager.RegisterEvent(Services::Event("rover.dance", std::string("{\"angle\": 220, \"speed\": 100}")));
            delay(1000);
        }, false);
        
        if(!danceTimer.Start().Succeded()) {
            Serial.printf("failed to start dance timer! \n\n\n\n\n");
        }
    }

   

    // {    
    //     Lib::Timer danceTimer("moveatangle", 8000, [&](){
    //         serviceManager.RegisterEvent(Services::Event("rover.dance", ""));
    //         delay(1000);

    //         serviceManager.RegisterEvent(Services::Event("rover.rotateandmove", 
    //         std::string("{\"angle\": -90, \"speed\": 100}")));
    //         delay(1000);
    //         serviceManager.RegisterEvent(Services::Event("rover.rotateandmove", 
    //         std::string("{\"angle\": 90, \"speed\": 100}")));
    //         delay(1000);
    //         serviceManager.RegisterEvent(Services::Event("rover.stop", ""));
    //     }, false);
        
    //     if(!danceTimer.Start().Succeded()) {
    //         Serial.printf("failed to start moveatangle timer! \n\n\n\n\n");
    //     }
    // }
    

    serviceManager.Run();
}
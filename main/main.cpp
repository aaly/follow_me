#include "services/ServiceManager.h"
#include "services/BluetoothService.h"
#include "services/DisplayService.h"
#include "services/RoverCService.h"
#include "services/IMUService.h"
#include <M5StickCPlus.h>
#include "lib/Timer.h"
#include "services/Event.h"

extern "C" void app_main()
{
    initArduino();
    Serial.begin(115200);
    
    Services::ServiceRegistry& serviceManager = Services::ServiceRegistry::GetInstance();
    serviceManager.RegisterService<Services::BluetoothService>("bluetooth scanner", "{\"mode\":\"scanner\"}");
    serviceManager.RegisterService<Services::DisplayService>("display", "");
    //serviceManager.RegisterService<Services::IMUService>("imu", "");
    serviceManager.RegisterService<Services::RoverCService>("rover", "");

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


    {    
        Lib::Timer danceTimer("moveatangle", 16000, [&](){
            serviceManager.RegisterEvent(Services::Event("rover.dance", ""));
            delay(1000);
        }, false);
        
        if(!danceTimer.Start().Succeded()) {
            Serial.printf("failed to start moveatangle timer! \n\n\n\n\n");
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
#include "services/ServiceManager.h"
#include "services/BluetoothService.h"
#include "services/RoverCService.h"
#include <M5StickCPlus.h>

void loop() {
    Services::ServiceRegistry& serviceManager = Services::ServiceRegistry::GetInstance();
    M5.update();
    if (M5.BtnA.wasReleased()) {
        serviceManager["rover"]["servo_angle"]("{\"servo_ch\":1, \"degree\":90}");
        delay(1000);

        serviceManager["rover"]["back"]("{\"speed\":50}");
        delay(2000);

        serviceManager["rover"]["left"]("{\"speed\":50}");
        delay(2000);

        serviceManager["rover"]["right"]("{\"speed\":50}");
        delay(2000);

        serviceManager["rover"]["turn_left"]("{\"speed\":30}");
        delay(2000);

        serviceManager["rover"]["turn_right"]("{\"speed\":30}");
        delay(2000);

        serviceManager["rover"]["forward"]("{\"speed\":50}");
        delay(2000);

        serviceManager["rover"]["stop"]("");
        serviceManager["rover"]["servo_angle"]("{\"servo_ch\":1, \"degree\":0}");
        delay(1000); 
    }
    serviceManager["bluetooth"]["scan"]("");
}

extern "C" void app_main()
{
    initArduino();
    M5.begin();

    M5.Lcd.setRotation(3);
    M5.Lcd.setTextColor(BLUE);
    M5.Lcd.setCursor(80, 60, 4);
    M5.Lcd.printf("RoverC");

    Serial.begin(115200);
    
    Services::ServiceRegistry& serviceManager = Services::ServiceRegistry::GetInstance();
    serviceManager.RegisterService<Services::RoverCService>("rover", "");
    serviceManager.RegisterService<Services::BluetoothService>("bluetooth scanner", "{\"mode\":\"scanner\"}");
    
    serviceManager["rover"]["stop"]("");
    delay(1500); 

    int bootcount = 0;
    while(true) {
        Serial.printf("start ESP32 %d\n", bootcount++);
        loop();
        
    }
}
#include "Bluetooth.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "../lib/Timer.h"
#include "utility/In_eSPI.h"

//https://circuitdigest.com/microcontroller-projects/esp32-based-bluetooth-ibeacon

namespace Services {

    class IBeaconAdvertised: public BLEAdvertisedDeviceCallbacks {
        public:
            // Callback when BLE is detected
            void onResult(BLEAdvertisedDevice device) {
                if (isIBeacon(device)) {
                    printIBeacon(device);
                }
            }
        private:
            // iBeacon packet judgment
            bool isIBeacon(BLEAdvertisedDevice device) {
                    return true;
                if (device.getManufacturerData().length() < 25) {
                    return false;
                }
                if (getCompanyId(device) != 0x1234) {
                    return false;
                }
                if (getIBeaconHeader(device) != 0x1234) {
                    return false;
                }
                return true;
            }
            // CompanyId acquisition
            unsigned short getCompanyId(BLEAdvertisedDevice device) {
                const unsigned short* pCompanyId = (const unsigned short*)&device.getManufacturerData().c_str()[0];
                return *pCompanyId;
            }
            // Get iBeacon Header
            unsigned short getIBeaconHeader(BLEAdvertisedDevice device) {
                const unsigned short* pHeader = (const unsigned short*)&device.getManufacturerData().c_str()[2];
                return *pHeader;
            }
            // Get UUID
            String getUuid(BLEAdvertisedDevice device) {
                const char* pUuid = &device.getManufacturerData().c_str()[4];
                char uuid[64] = {0};
                sprintf(
                    uuid,
                    "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                    pUuid[0], pUuid[1], pUuid[2], pUuid[3], pUuid[4], pUuid[5], pUuid[6], pUuid[7],
                    pUuid[8], pUuid[9], pUuid[10], pUuid[11], pUuid[12], pUuid[13], pUuid[14], pUuid[15]
                );
                return String(uuid);
            }
            // Get TxPower
            signed char getTxPower(BLEAdvertisedDevice device) {
                const signed char* pTxPower = (const signed char*)&device.getManufacturerData().c_str()[24];
                return *pTxPower;
            }
            // Serial output of iBeacon information
            void printIBeacon(BLEAdvertisedDevice device) {
                if(device.getName() == "GalaxyMaster") {
                    Serial.printf("Advertised Device: %s \n", device.toString().c_str());
                    Serial.printf("RSSI: %d \n", device.getRSSI());
                    
                }
                return;
                Serial.printf("addr:%s rssi:%d uuid:%s power:%d\r\n",
                                            device.getAddress().toString().c_str(),
                                            device.getRSSI(),
                                            getUuid(device).c_str(),
                                            *(signed char*)&device.getManufacturerData().c_str()[24]);
            }
    };

    Bluetooth::Bluetooth(const std::string& name, Services::ServiceRegistry* service_manager): Service(name, service_manager) {
    };

    void Bluetooth::Init(const ParametersPack& config) {
         BLEDevice::init(config.GetParameter<std::string>("name").c_str());

        //Serial.printf("Bluetooth service [%s] set mode to [%s]\n",
        //_name.c_str(), 
        //config.GetParameter<std::string>("mode").c_str());

        if("scanner" == config.GetParameter<std::string>("mode")) {
            //Dispatch(Event("bluetooth.scanner.scan",""));
        }

        On("bluetooth.scanner.scan", &Bluetooth::handleScan);
    }

    
    void Bluetooth::handleScan(const Services::ParametersPack& parameters) {
        if(parameters.GetParameter<bool>("continuous")) {
            Dispatch(Event("bluetooth.scanner.scan", parameters.AsString()));
        }

        Result<std::vector<Connection>> scan_result = 
        Scan(40, parameters.GetParameter<std::string>("device"));

        if(scan_result.Status() == StatusType::SUCCESS) {
            for(const auto& connection: scan_result.Data()) {
                Serial.printf("Bluetooth service sharing scan result for [%s]\n", connection.name.c_str());
                Serial.flush();
                Emit(Event("bluetooth.scanner.results", "{\"name\":\"" + connection.name + "\", \"rssi\":" + std::to_string(connection.rssi) + "}"));
            }
        }
    }

    Result<std::vector<Connection>> Bluetooth::Scan(const uint32_t scanTime, const std::string& device_name) {
        Result<std::vector<Connection>> result;

        BLEScan* scan = BLEDevice::getScan();
        //scan->setAdvertisedDeviceCallbacks(new IBeaconAdvertised(), true);
        scan->setAdvertisedDeviceCallbacks(nullptr);
        scan->setActiveScan(false);

        Lib::Timer bluetoothScannerStopper("bluetoothScannerStopper", scanTime, [&](){
            scan->stop();
            }, true);
        auto timerResult = bluetoothScannerStopper.Start();

        if(timerResult.Succeded()) {
            auto scan_result = scan->start(10);
            std::vector<Connection> connections_vector;

            for(uint32_t i =0; i < scan_result->getCount(); i++) {
                auto ble_device = scan_result->getDevice(i);

                if(device_name.size()) {
                        if (strcmp(ble_device.getName().c_str(), device_name.c_str()) == 0) {
                        Connection ble_connection;
                        ble_connection.name = ble_device.getName().c_str();
                        ble_connection.rssi = ble_device.getRSSI();
                        connections_vector.push_back(ble_connection);
                        break;
                    }
                }
                else {
                    Connection ble_connection;
                    ble_connection.name = ble_device.getName().c_str();
                    ble_connection.rssi = ble_device.getRSSI();
                    connections_vector.push_back(ble_connection);
                }
            }
            result.Success(connections_vector);
        }
        else {
                Serial.printf("BLE Timer Error [%s]\n", timerResult.Data().c_str());
        }
        return result;
    }
} //namespace Services


// #define uS_PER_SEC 1000000LL
// #define GPIO_DEEP_SLEEP_DURATION_SECOND    uS_PER_SEC // sleep x seconds and then wake up
// #define GPIO_DEEP_SLEEP_DURATION_MILLISECOND uS_PER_SEC/1000 // sleep x milliseconds and then wake up

// RTC_DATA_ATTR static time_t last;                // remember last boot in RTC Memory
// RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

// // See the following for generating UUIDs:
// // https://www.uuidgenerator.net/
// BLEAdvertising *pAdvertising;     // BLE Advertisement type
// struct timeval now;

// #define BEACON_UUID "87b99b2c-90fd-11e9-bc42-526af7764f64" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)

// void setBeacon() {

//         BLEBeacon oBeacon = BLEBeacon();
//         oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
//         oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
//         oBeacon.setMajor((bootcount & 0xFFFF0000) >> 16);
//         oBeacon.setMinor(bootcount & 0xFFFF);
//         oBeacon.setMinor(bootcount & 0xFFFF);
//         BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
//         BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

//         oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04

//         std::string strServiceData = "";

//         strServiceData += (char)26;         // Len
//         strServiceData += (char)0xFF;     // Type
//         strServiceData += oBeacon.getData();
//         oAdvertisementData.addData(strServiceData);

//         pAdvertising->setAdvertisementData(oAdvertisementData);
//         //pAdvertising->setScanResponseData(oScanResponseData);
// }

// void setup() {

//         Serial.begin(115200);
//         gettimeofday(&now, NULL);
//         Serial.printf("start ESP32 %d\n", bootcount++);
//         Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n", now.tv_sec, now.tv_sec - last);
//         last = now.tv_sec;

//         // Create the BLE Device
//         BLEDevice::init("ESP32 as iBeacon");
//         //BLEDevice::setPower(ESP_PWR_LVL_P9);
//         BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_ADV);
//         // Create the BLE Server
//         pAdvertising = BLEDevice::getAdvertising();
//         BLEDevice::startAdvertising();

//         pAdvertising->setMinInterval(0x20);
//         pAdvertising->setMaxInterval(0x30);
//         setBeacon();
//         // Start advertising
//         pAdvertising->start();
//         Serial.println("Advertizing started...");
    
// }

// void loop() {

//         delay(200);
//         return;



        
//         esp_sleep_enable_timer_wakeup(GPIO_DEEP_SLEEP_DURATION_MILLISECOND * 10);
//         Serial.printf("enter light sleep\n");

//         /* To make sure the complete line is printed before entering sleep mode,
//         * need to wait until UART TX FIFO is empty:
//         */
//         uart_wait_tx_idle_polling(GPIO_DEEP_SLEEP_DURATION_MILLISECOND * 10);

        
        
//         /* Get timestamp before entering sleep */
//         int64_t t_before_us = esp_timer_get_time();
//         esp_light_sleep_start();
        
        
//         /* Get timestamp after waking up from sleep */
//         int64_t t_after_us = esp_timer_get_time();

//         /* Determine wake up reason */
//         const char* wakeup_reason;
//         switch (esp_sleep_get_wakeup_cause()) {
//                 case ESP_SLEEP_WAKEUP_TIMER:
//                         wakeup_reason = "timer";
//                         break;
//                 case ESP_SLEEP_WAKEUP_GPIO:
//                         wakeup_reason = "pin";
//                         break;
//                 default:
//                         wakeup_reason = "other";
//                         break;
//         }

//         printf("Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
//                         wakeup_reason, t_after_us / 1000, (t_after_us - t_before_us) / 1000);
                        
//         //Serial.printf("enter deep sleep\n");
//         //esp_deep_sleep();
//         //esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
//         //Serial.printf("in deep sleep\n");
// }

















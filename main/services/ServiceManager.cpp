#include "ServiceManager.h"
#include "Service.h"
#include <M5Unified.hpp>
#include "Service.h"
namespace Services {
    ServiceRegistry& ServiceRegistry::GetInstance()
    {
        static ServiceRegistry instance; // Guaranteed to be destroyed.
        return instance;
    };

    ServiceRegistry::ServiceRegistry() {
    };


    Result<std::string> ServiceRegistry::RegisterEvent(const Services::Event& event) {
        return _events.Send(event);
    };

    void ServiceRegistry::Run() {
        while(_keepRunning) {
            //Serial.printf("Internal Total heap %d, internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
            //Serial.printf("SPIRam Total heap %d, SPIRam Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram()); 
            //Serial.printf("ChipRevision %d, Cpu Freq %d, SDK Version %s\n",ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
            //Serial.printf("Flash Size %d, Flash Speed %d\n",ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
            //Serial.flush();

            Result<Event> recv_result = _events.Receive();
            if(recv_result.Status() == StatusType::SUCCESS) {
                const Event e = recv_result.Data();
                if ("subscribe" == e.Name()) {
                    _subscriptions[e.Parameters().GetParameter<std::string>("event")][e.Parameters().GetParameter<std::string>("service")] = true;
                }
                else {
                    if(_subscriptions.count(e.Name())) {
                        for(const auto& service: _subscriptions[e.Name()]) {
                            if(_services.count(service.first)) {
                                _services[service.first]->Dispatch(e);
                            }
                        }
                    }
                }
            }
            else {
                Serial.printf("[ERROR] receiving events! aborting!\n");
                Serial.flush();
                abort();
            }
        }
    };
} //namespace Services
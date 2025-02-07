#include "Service.h"
#include "ServiceManager.h"

namespace Services {

    Service::Service(const std::string& name, Services::ServiceRegistry* service_manager):
    _name(name), _service_manager(service_manager), _ev_loop_thread(_name, std::bind(&Service::Run, this), _stackSize){
        auto result = _ev_loop_thread.Run();

        On("shutdown", &Service::Shutdown);
        On("init", &Service::Init);
    };

    void Service::Shutdown(const Services::ParametersPack& parameters) {
        _keepRunning = false;
    }


    Result<std::string> Service::Dispatch(const Event& event) {
        Serial.printf("Dispatching event [%s] to [%s]\n", event.Name().c_str(), _name.c_str());
        Serial.flush();
        return _events.Send(event, Lib::QueueDelayType::IMMEDIATE);
    };

    void Service::Run() {
        while(_keepRunning) {
            Result<Event> recv_result = _events.Receive();
            if(recv_result.Status() == StatusType::SUCCESS) {
                 auto eventName = recv_result.Data().Name();
                Serial.printf("Dispatching event [%s]\n", eventName.c_str());
                Serial.flush();
                auto parameters = recv_result.Data().Parameters();

                if (_event_handlers.find(eventName) != _event_handlers.end()) {
                    _event_handlers[eventName](parameters);
                } else {
                    ESP_LOGW("Service", "No handler found for event: %s", eventName.c_str());
                }
            }
            else {
                //TODO report error
            }
        }
    };

    Result<std::string> Service::Emit(const Event& e) {
        Result<std::string> result = _service_manager->RegisterEvent(e);
        if(result.Status() != StatusType::SUCCESS) {
            result.Failure("Could not emit event ["+ e.Name() +"] from service [" + _name + "]");
        }
        return result;
    }

    Result<std::string> Service::Subscribe(const std::string& event_name) {
        const Event e("subscribe", "{\"event\":\"" + event_name + "\", \"service\":\"" + _name + "\"}");
        return Emit(e);
    }
} //namespace Services
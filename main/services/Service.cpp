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
        //Serial.printf("Dispatching event [%s] to [%s]\n", event.Name().c_str(), _name.c_str());
        //Serial.flush();
        return _events.Send(event, Lib::QueueDelayType::IMMEDIATE);
    };

    void Service::Run() {
        while(_keepRunning) {
            Result<Event> recv_result = _events.Receive();
            if(recv_result.Status() == StatusType::SUCCESS) {
                _event_handlers[recv_result.Data().Name()](recv_result.Data().Parameters());
            }
            else {
                //TODO report error
            }
        }
    };

    Result<std::string> Service::Emit(const Event& e) {
        Result<std::string> result;
        if(_service_manager->RegisterEvent(e).Status() != StatusType::SUCCESS) {
            result.Failure("Could not emit event ["+ e.Name() +"] from service [" + _name + "]");
        }
        return result;
    }

    Result<std::string> Service::Subscribe(const std::string& event_name) {
        const Event e("subscribe", "{\"event\":\"" + event_name + "\", \"service\":\"" + _name + "\"}");
        return Emit(e);
    }
} //namespace Services
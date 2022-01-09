#include "Service.h"
#include "ServiceManager.h"

namespace Services {

    Service::Service(const std::string& name, Services::ServiceRegistry* service_manager):
    _name(name), _service_manager(service_manager), _ev_loop_thread(_name, std::bind(&Service::Run, this), _stackSize){
        auto result = _ev_loop_thread.Run();

        On("shutdown", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result;
            _keepRunning = false;
            result.Success("");
            return result;
        });

        On("init", [&](const Services::ParametersPack& parameters) {
            Result<std::string> result = Init(parameters);
            return result;
        });
    };

    Result<std::string> Service::Dispatch(const Event& event) {
        //Serial.printf("Dispatching event [%s] to [%s]\n", event.Name().c_str(), _name.c_str());
        //Serial.flush();
        return _events.Send(event, Lib::QueueDelayType::IMMEDIATE);
    };


    Result<std::string> Service::On(const std::string& event_name, std::function<Result<std::string>(const Services::ParametersPack&)> handler) {
        Result<std::string> result;
        if(!_event_handlers.count(event_name)) {
            result = Subscribe(event_name);
            _event_handlers[event_name] = handler;
        }
        else {
            Serial.printf("ON 3\n");
            result.Failure("Failed to register event handler");
        }        
        return result;
    }

    void Service::Run() {
        while(_keepRunning) {
            Result<Event> recv_result = _events.Receive();
            if(recv_result.Status() == StatusType::SUCCESS) {
                //auto handle_result = HandleEvent(recv_result.Data());
                auto handle_result = _event_handlers[recv_result.Data().Name()](recv_result.Data().Parameters());
                //Serial.printf("Service [%s] handled event [%s] successfully? %d\n", _name.c_str(), recv_result.Data().Name().c_str(), (int)handle_result.Status());
                //Serial.flush();
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
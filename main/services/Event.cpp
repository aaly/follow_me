#include "Event.h"

namespace Services {

    Event::Event() {
        std::fill(_eventName, _eventName+_eventNameSize, '\0');
        std::fill(_eventParameters, _eventParameters+_eventParametersSize, '\0');
    };

    Event::Event(const Event& e): Event() {

        std::string buffer = e.Name();
        int32_t i = 0;
        for (const auto& ch: buffer) {
            _eventName[i] = ch;
            i++;
        }

        buffer = e.Parameters().AsString();
        i = 0;
        for (const auto& ch: buffer) {
            _eventParameters[i] = ch;
            i++;
        }
    };

    Event::Event(const std::string& name, const std::string& parameters): Event() {
        std::fill(_eventName, _eventName+_eventNameSize, '\0');
        std::fill(_eventParameters, _eventParameters+_eventParametersSize, '\0');

        int32_t i = 0;
        for (const auto& ch: name) {
            _eventName[i] = ch;
            i++;
        }

        i = 0;
        for (const auto& ch: parameters) {
            _eventParameters[i] = ch;
            i++;
        }
    }

    std::string Event::Name() const {
        return std::string(_eventName);
    };

    ParametersPack Event::Parameters() const {
        return std::string(_eventParameters);
    };

    ParametersPack::ParametersPack(const std::string& parameters): _parameters(Parse(parameters)){
    };

    ParametersPack::ParametersPack(const char* parameters): _parameters(Parse(std::string(parameters))){
    };

    
    bool ParametersPack::HasParameter(const std::string& parameter) const {
        bool result = false;
        if(_parameters.find(parameter) != _parameters.end()) {
            result = true;
        }
        return result;
    };


    std::string ParametersPack::AsString() const {
        return _parameters.dump();
    };

    nlohmann::json ParametersPack::Parse(const std::string& json) {
        nlohmann::json result;
        if(json.size()){
            result = nlohmann::json::parse(json);
        }
        return result;
    };
} //namespace Services
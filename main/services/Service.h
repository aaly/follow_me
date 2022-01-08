#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "../common.h"
#include <string>
#include <map>
#include <functional>
#include <json.hpp>
#include <M5StickCPlus.h>

namespace Services {

    class ParametersPack {
        public:

            /**
             * @brief parameters passed around to callbacks using this class, to reduce json parsing to one time only...
             */
            ParametersPack(const std::string& parameters): _parameters(Parse(parameters)){};
            ParametersPack(const char* parameters): _parameters(Parse(std::string(parameters))){};

            /**
             * @brief get a parameter value map from a json string
             */
            template<typename T>
            T GetParameter(const std::string& parameter) const {
                T result = _parameters[parameter];
                return result;
            };
        private:
            nlohmann::json Parse(const std::string& json) {
                nlohmann::json result;
                if(json.size()){
                    result = nlohmann::json::parse(json);
                }
                return result;
            };
            nlohmann::json _parameters;
    };

    class Service {

        public:

            /**
             * @brief set the service name for debugging purposes...
             */
            void SetName(const std::string& name) {
                _name = name;
            };

            /**
             * @brief Initilization of the service
             * @param config parameters for the service
             * @return Result with a status string in case of error
             */
            virtual Result<std::string> Init(const ParametersPack& config) = 0;

            
            /**
             * @brief Run the service, this will not start it's own thread/task, not my responsibility bro!
             * @return Result with a status string in case of error
             */
            virtual Result<std::string> Run() = 0;

            /**
             * @brief rerieve a callback function
             * a callback will be registered by the service, and it does send and receive json objects :s, 
             * this is not embedded anymore (or is it?)
             */
            std::function<Result<std::string>(const ParametersPack&)> operator [](const std::string& method) {
                std::function<Result<std::string>(const ParametersPack&)> result = [&](const ParametersPack&) {
                    Serial.printf("Method [%s] not found for service [%s]\n", method.c_str(), _name.c_str());
                    Serial.flush();
                    return Result<std::string>();
                };

                if (_callbacks.count(method)) {
                    result = _callbacks[method];
                }
                
                return result;
            };

            /**
             * @brief add a callback to our callbacks map list
             */
            void AddCallback(const std::string& name, const std::function<Result<std::string>(const ParametersPack&)> &f) {
                _callbacks[name] = f;
            };

        private:
            std::map<std::string, std::function<Result<std::string>(const ParametersPack&)>> _callbacks;
            std::string _name;
    };
} //namespace Services

#endif //#ifndef
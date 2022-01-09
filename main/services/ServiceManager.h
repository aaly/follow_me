#ifndef _SERVICEMANAGER_H_
#define _SERVICEMANAGER_H_


#include <memory>
#include <map>
#include <string>
#include "../common.h"
#include <Queue.h>
#include "Event.h"
#include "Service.h"

namespace Services {
    class PanicService;
    class ServiceRegistry {
        public:

            /**
             * @brief get a singleton reference to ServiceRegistry
             * @return a reference to the singleton instance
             */
            static ServiceRegistry& GetInstance();

            /**
             * @brief register a service
             * @return Result with a status string in case of error
             */        
            template<typename T>
            Result<std::string> RegisterService(const std::string& service, const Services::ParametersPack& config) {
                Result<std::string> result;
                if (!_services.count(service)) {
                    _services[service] = std::shared_ptr<T>(new T(service, this));
                    Services::Event e("init", config.AsString());
                    result = _services[service]->Dispatch(e);

                    if(result.Status() == StatusType::FAILURE) {
                        Serial.printf("Error dispatching to Service [%s] Handling event [%s]\n", service.c_str(), e.Name().c_str());
                    }
                }
                else {
                    result.SetData(std::string("Service name already exists"));
                }
                return result;
            };

            /**
             * @brief retrieve a service
             */
            Service& operator[](const std::string& service);

            /**
             * @brief retrieve a service
             */
            Result<std::string> RegisterEvent(const Event& event);
            
            /**
             * @brief run the service manager and process all events and dispatch them etc...
             */
            void Run();

        private:
            ServiceRegistry();
            ServiceRegistry(ServiceRegistry const&);
            void operator=(ServiceRegistry const&);

            std::map<std::string, std::shared_ptr<Service>> _services;
            std::map<std::string, std::map<std::string, bool>> _subscriptions;
            Lib::Queue<Services::Event> _events;

            bool _keepRunning = true;
    };
} //namespace Services

#endif //#ifndef _SERVICEMANAGER_H_
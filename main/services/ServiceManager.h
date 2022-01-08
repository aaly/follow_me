#ifndef _SERVICEMANAGER_H_
#define _SERVICEMANAGER_H_

#include "Service.h"
#include "PanicService.h"
#include <memory>
#include <map>
#include <string>

namespace Services {
    class ServiceRegistry {
        public:

            /**
             * @brief get a singleton reference to ServiceRegistry
             * @return a reference to the singleton instance
             */
            static ServiceRegistry& GetInstance()
            {
                static ServiceRegistry instance; // Guaranteed to be destroyed.
                return instance;
            };

            /**
             * @brief register a service
             * @return Result with a status string in case of error
             */
            template<typename T>
            Result<std::string> RegisterService(const std::string& service, const ParametersPack& config) {
                Result<std::string> result;
                if (!_services.count(service)) {
                    _services[service] = std::shared_ptr<T>(new T);
                    _services[service]->SetName(service);
                    result = _services[service]->Init(config);
                }
                else {
                    result.SetData(std::string("Service name already exists"));
                }
                return result;
            };

            /**
             * @brief retrieve a service
             */
            Service& operator[](const std::string& service) {
                Service& result =  *_services["panic"].get();
                if (_services.count(service)) {
                    result = *_services[service].get();
                }
                return result;
            };

        private:
            ServiceRegistry() {RegisterService<Services::PanicService>("panic", "");};
            ServiceRegistry(ServiceRegistry const&);
            void operator=(ServiceRegistry const&);

            std::map<std::string, std::shared_ptr<Service>> _services;
    };
} //namespace Services

#endif //#ifndef _SERVICEMANAGER_H_
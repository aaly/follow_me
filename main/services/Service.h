#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "../common.h"
#include <Queue.h>
#include <Thread.h>
#include "Event.h"
#include <string>
#include <map>
#include <functional>
#include <M5StickCPlus.h>
namespace Services {
    class ServiceRegistry;
    class Service {

        public:
            /**
             * @brief basically, run the Run in a thread
             */
            Service(const std::string& name,  Services::ServiceRegistry* service_manager);

            /**
             * @brief Initilization of the service
             * @param config parameters for the service
             * @return Result with a status string in case of error
             */
            virtual Result<std::string> Init(const ParametersPack& config) = 0;

            /**
             * @brief Dispatch event to the event queue
             * @param event the event to be added
             * @return Result of adding the event to the queue
             */
            Result<std::string> Dispatch(const Event& event);

            /**
             * @brief Run the service, this will not start it's own thread/task, not my responsibility bro!
             * @return Result with a status string in case of error
             */
            void Run();

            /**
             * @brief rerieve a callback function
             * a callback will be registered by the service, and it does send and receive json objects :s, 
             * this is not embedded anymore (or is it?)
             */
            std::function<Result<std::string>(const ParametersPack&)> operator [](const std::string& method);

            /**
             * @brief Emit one event
             * @result status indicating why it failed if it does ;)
             */
            Result<std::string> Emit(const Event& e);

            /**
             * @brief subscribe to events
             * @result status indicating why it failed if it does ;)
             */
            Result<std::string> Subscribe(const std::string& event_name);


            /**
             * @brief subscribe to events and define how to handle them
             */
            Result<std::string> On(const std::string& event_name, std::function<Result<std::string>(const Services::ParametersPack&)> handler);

        protected:
            std::string _name;
            uint32_t _stackSize = 10000;
            Lib::Queue<Event> _events;
            ServiceRegistry* _service_manager = nullptr;
            Lib::Thread _ev_loop_thread;
            std::map<std::string, std::function<Result<std::string>(const Services::ParametersPack&)>> _event_handlers;

            bool _keepRunning = true;
    };
} //namespace Services

#endif //#ifndef
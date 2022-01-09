#ifndef _EVENT_H_
#define _EVENT_H_

#include "../common.h"
#include <string>
#include <json.hpp>

namespace Services {

    class ParametersPack {
        public:
            /**
             * @brief parameters passed around to callbacks using this class, to reduce json parsing to one time only...
             */
            ParametersPack(const std::string& parameters);
            ParametersPack(const char* parameters);

            /**
             * @brief get a parameter value map from a json string
             */
            template<typename T>
            T GetParameter(const std::string& parameter) const {
                T result{0};
                if(_parameters.find(parameter) != _parameters.end()) {
                    result = _parameters[parameter];
                }
                return result;
            };

            /**
             * @brief find out if a parameter is in the map or not
             */
            bool HasParameter(const std::string& parameter) const;


            /**
             * @brief get the internal object as a string
             */
            std::string AsString() const;
        private:
            nlohmann::json Parse(const std::string& json);
            nlohmann::json _parameters;
    };

    struct Event {
        public:
            Event();
            Event(const Event&);
            Event(const std::string& name, const std::string& parameters);

            /**
             * @brief get event name
             */
            std::string Name() const;

            /**
             * @brief get event parameters pack
             */
            ParametersPack Parameters() const;
            
        private:
            static const int32_t _eventNameSize{50};
            char _eventName[_eventNameSize];

            static const int32_t _eventParametersSize{500};
            char _eventParameters[_eventParametersSize];
    };
} //namespace Services

#endif //#ifndef _EVENT_H_
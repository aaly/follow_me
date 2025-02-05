#ifndef _MESSAGEQUEUE_LIB
#define _MESSAGEQUEUE_LIB

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "../common.h"
#include <string>

namespace Lib {
    

    enum class QueueDelayType : uint32_t {
                IMMEDIATE = 0,
                MAX = portMAX_DELAY,
    };

    template <typename T>
    class Queue {
        public:
            
            /**
             * @brief cctor, crrate a queue with the size
             */

            Queue() {
                _queue = xQueueCreate(_queueSize, sizeof(T));
            }

            /**
             * @brief receive a message, if queue und block forever until a messga earrives
             * @param delay, default is block for forever
             * @return Result with the message
             */
            Result<T> Receive(const QueueDelayType& delay = QueueDelayType::MAX) {
                Result<T> result;
                T message;
                auto recv_result = xQueueReceive(_queue, &message, static_cast<TickType_t>(delay));
                if(pdTRUE == recv_result) {
                    result.Success(message);
                }
                else {
                    result.SetStatus(StatusType::FAILURE);
                }
                return result;
            };

            /**
             * @brief add message to the queue
             * @param message the message to be added
             * @param wait is the time to wait for the queue to be free
             * @return Result of adding the message to the queue
             */
            Result<std::string> Send(const T message, const QueueDelayType& wait = QueueDelayType::MAX) {
                Result<std::string> result;
                auto send_result = xQueueSend(_queue, &message, static_cast<TickType_t>(wait));
                if(pdTRUE == send_result) {
                    result.Success("Success to send message to queue");
                }
                else {
                    result.Failure("Failure to send message to queue");
                }
                return result;
            };

        private:
            QueueHandle_t _queue;
            uint8_t _queueSize = 30;
    };

} //namespace Lib
#endif //_MESSAGEQUEUE_LIB